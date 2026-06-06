//
// Created by ryan tan on 6/6/26.
//
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <atomic>
#include <thread>
#include <vector>
#include <functional>
#include <chrono>
#include <algorithm>

// networking headers
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>

// ─── ASSERT macro ─────────────────────────────────────────────────────────────
#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "ASSERT failed: " << (msg) << "\n" \
                      << "  at " << __FILE__ << ":" << __LINE__ << "\n"; \
            std::abort(); \
        } \
    } while (0)

using Nanos = int64_t;
constexpr Nanos NANOS_TO_MICROS = 1000;
constexpr Nanos NANOS_TO_SECS   = 1000000000LL;

inline Nanos getCurrentNanos() noexcept {
return std::chrono::duration_cast<std::chrono::nanoseconds>(
std::chrono::system_clock::now().time_since_epoch()).count();
}

// ─── Socket helpers ───────────────────────────────────────────────────────────

// Looks up the IP address of a named network interface (e.g. "lo", "eth0").
auto getIfaceIP(const std::string& iface) -> std::string {
    char buf[NI_MAXHOST] = {'\0'};
    ifaddrs* ifaddr = nullptr;
    if (getifaddrs(&ifaddr) != -1) {
        for (ifaddrs* ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr &&
                ifa->ifa_addr->sa_family == AF_INET &&
                iface == ifa->ifa_name) {
                getnameinfo(ifa->ifa_addr, sizeof(sockaddr_in),
                            buf, sizeof(buf), nullptr, 0, NI_NUMERICHOST);
                break;
            }
        }
        freeifaddrs(ifaddr);
    }
    return buf;
}

// Makes a socket non-blocking: read/write return immediately instead of waiting.
auto setNonBlocking(int fd) -> bool {
    const auto flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return false;
    if (flags & O_NONBLOCK) return true;
    return (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1);
}

// Disables Nagle's algorithm so packets are sent immediately without batching.
auto setNoDelay(int fd) -> bool {
    int one = 1;
    return (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
                       reinterpret_cast<void*>(&one), sizeof(one)) != -1);
}

// Enables kernel-level receive timestamps on each incoming packet.
auto setSOTimestamp(int fd) -> bool {
    int one = 1;
    return (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP,
                       reinterpret_cast<void*>(&one), sizeof(one)) != -1);
}

// Returns true if the last socket error is non-fatal (no data available yet).
auto wouldBlock() -> bool {
    return (errno == EWOULDBLOCK || errno == EINPROGRESS);
}

constexpr int MaxTCPServerBacklog = 1024;

// Creates and configures a socket file descriptor.
// Handles TCP client, TCP server, UDP sender, and UDP receiver.
auto createSocket(const std::string& t_ip, const std::string& iface,
                  int port, bool is_udp, bool is_blocking,
                  bool is_listening, int /*ttl*/, bool needs_so_timestamp) -> int {

    const auto ip = t_ip.empty() ? getIfaceIP(iface) : t_ip;

    addrinfo hints{};
    hints.ai_family   = AF_INET;
    hints.ai_socktype = is_udp ? SOCK_DGRAM  : SOCK_STREAM;
    hints.ai_protocol = is_udp ? IPPROTO_UDP : IPPROTO_TCP;
    hints.ai_flags    = is_listening ? AI_PASSIVE : 0;
    if (!ip.empty() && std::isdigit(ip[0]))
        hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_flags |= AI_NUMERICSERV;

    addrinfo* result = nullptr;
    const auto rc = getaddrinfo(ip.empty() ? nullptr : ip.c_str(),
                                std::to_string(port).c_str(),
                                &hints, &result);
    if (rc) {
        std::cerr << "getaddrinfo() failed: " << gai_strerror(rc) << "\n";
        return -1;
    }

    int fd = -1;
    for (addrinfo* rp = result; rp; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1) continue;

        if (!is_blocking) {
            setNonBlocking(fd);
            if (!is_udp) setNoDelay(fd);
        }

        if (!is_listening) {
            // Client: connect to the server
            connect(fd, rp->ai_addr, rp->ai_addrlen);
        } else {
            // Server: bind to the port and start listening
            int one = 1;
            setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
            if (bind(fd, rp->ai_addr, rp->ai_addrlen) == -1) {
                close(fd); fd = -1; continue;
            }
            if (!is_udp)
                listen(fd, MaxTCPServerBacklog);
        }
        break;
    }

    freeaddrinfo(result);

    if (fd != -1 && needs_so_timestamp)
        setSOTimestamp(fd);

    return fd;
}

// ─── TCPSocket ────────────────────────────────────────────────────────────────
// Wraps a single TCP connection with send/receive buffers and a data callback.
// The hot path stages data into send_buffer_ via send(), then flushes everything
// to the wire in one syscall via sendAndRecv() — minimising kernel round-trips.

constexpr size_t TCPBufferSize = 64 * 1024 * 1024;  // 64 MB per buffer

struct TCPSocket {
    int    fd_                    = -1;
    char*  send_buffer_           = nullptr;
    size_t next_send_valid_index_ = 0;
    char*  rcv_buffer_            = nullptr;
    size_t next_rcv_valid_index_  = 0;
    bool   send_disconnected_     = false;
    bool   recv_disconnected_     = false;

    // Called whenever new data arrives. Application registers its handler here.
    std::function<void(TCPSocket*, Nanos)> recv_callback_;

    TCPSocket() {
        send_buffer_ = new char[TCPBufferSize];
        rcv_buffer_  = new char[TCPBufferSize];
        recv_callback_ = [](TCPSocket*, Nanos) {};  // default: do nothing
    }

    ~TCPSocket() {
        if (fd_ != -1) close(fd_);
        delete[] send_buffer_;
        delete[] rcv_buffer_;
    }

    TCPSocket(const TCPSocket&)            = delete;
    TCPSocket(TCPSocket&&)                 = delete;
    TCPSocket& operator=(const TCPSocket&) = delete;
    TCPSocket& operator=(TCPSocket&&)      = delete;

    // Establish a connection (client) or start listening (server).
    int connect(const std::string& ip, const std::string& iface,
                int port, bool is_listening) {
        if (fd_ != -1) { close(fd_); fd_ = -1; }
        fd_ = createSocket(ip, iface, port,
                /*is_udp=*/false, /*is_blocking=*/false,
                           is_listening, /*ttl=*/0, /*so_timestamp=*/true);
        return fd_;
    }

    // Stage outgoing data into send_buffer_. No syscall — just a memcpy.
    // The actual write to the wire happens in sendAndRecv().
    void send(const void* data, size_t len) noexcept {
        if (len > 0) {
            memcpy(send_buffer_ + next_send_valid_index_, data, len);
            next_send_valid_index_ += len;
        }
    }

    // Receive any incoming data and flush staged outgoing data.
    // Returns true if data was received.
    bool sendAndRecv() noexcept {
        // ── Receive ──────────────────────────────────────────────────────────
        iovec iov;
        iov.iov_base = rcv_buffer_ + next_rcv_valid_index_;
        iov.iov_len  = TCPBufferSize - next_rcv_valid_index_;

        char ctrl[CMSG_SPACE(sizeof(struct timeval))];
        msghdr msg{};
        msg.msg_control    = ctrl;
        msg.msg_controllen = sizeof(ctrl);
        msg.msg_iov        = &iov;
        msg.msg_iovlen     = 1;

        const auto n_rcv = recvmsg(fd_, &msg, MSG_DONTWAIT);
        Nanos kernel_time = 0;

        if (n_rcv > 0) {
            next_rcv_valid_index_ += static_cast<size_t>(n_rcv);

            // Extract the kernel-level receive timestamp from the control message.
            for (cmsghdr* cmsg = CMSG_FIRSTHDR(&msg); cmsg;
                 cmsg = CMSG_NXTHDR(&msg, cmsg)) {
                if (cmsg->cmsg_level == SOL_SOCKET &&
                    cmsg->cmsg_type  == SCM_TIMESTAMP) {
                    struct timeval tv;
                    memcpy(&tv, CMSG_DATA(cmsg), sizeof(tv));
                    kernel_time = tv.tv_sec * NANOS_TO_SECS
                                  + tv.tv_usec * NANOS_TO_MICROS;
                }
            }
            recv_callback_(this, kernel_time);
        }

        // ── Send ─────────────────────────────────────────────────────────────
        // Flush everything staged in send_buffer_ to the wire.
        // Loop because the OS may not accept all bytes in one call.
        ssize_t n_send = static_cast<ssize_t>(next_send_valid_index_);
        size_t  offset = 0;
        while (n_send > 0) {
            const int flags = MSG_DONTWAIT | MSG_NOSIGNAL;
            auto n = ::send(fd_, send_buffer_ + offset,
                            static_cast<size_t>(n_send), flags);
            if (n < 0) {
                if (!wouldBlock()) send_disconnected_ = true;
                break;
            }
            n_send -= n;
            offset += static_cast<size_t>(n);
        }
        next_send_valid_index_ = 0;

        return (n_rcv > 0);
    }
};

// ─── TCPServer ────────────────────────────────────────────────────────────────
// Manages multiple simultaneous client connections using epoll.
// epoll lets the server ask the OS "which sockets have activity right now?"
// so we only spend time on sockets that actually have data — not idle ones.

struct TCPServer {
    int         efd_             = -1;   // epoll file descriptor
    TCPSocket   listener_socket_;        // accepts new connections
    epoll_event events_[1024];

    std::vector<TCPSocket*> sockets_;              // all connected clients
    std::vector<TCPSocket*> receive_sockets_;      // clients with data to read
    std::vector<TCPSocket*> send_sockets_;         // clients ready to receive sends
    std::vector<TCPSocket*> disconnected_sockets_; // clients to clean up

    std::function<void(TCPSocket*, Nanos)> recv_callback_;
    std::function<void()>                  recv_finished_callback_;

    TCPServer() {
        recv_callback_          = [](TCPSocket*, Nanos) {};
        recv_finished_callback_ = []() {};
    }

    ~TCPServer() {
        if (efd_ != -1) close(efd_);
        for (auto* s : sockets_) delete s;
    }

    // Register a socket with epoll so we get notified when it has activity.
    bool epoll_add(TCPSocket* socket) {
        epoll_event ev{};
        ev.events   = EPOLLET | EPOLLIN;
        ev.data.ptr = reinterpret_cast<void*>(socket);
        return (epoll_ctl(efd_, EPOLL_CTL_ADD, socket->fd_, &ev) != -1);
    }

    // Unregister and delete a client socket.
    void del(TCPSocket* socket) {
        epoll_ctl(efd_, EPOLL_CTL_DEL, socket->fd_, nullptr);
        sockets_.erase(std::remove(sockets_.begin(), sockets_.end(), socket),
                       sockets_.end());
        delete socket;
    }

    // Start listening for incoming connections on the given interface and port.
    void listen(const std::string& iface, int port) {
        if (efd_ != -1) close(efd_);
        efd_ = epoll_create(1);
        ASSERT(efd_ != -1, "epoll_create failed");

        listener_socket_.connect("", iface, port, /*is_listening=*/true);
        ASSERT(listener_socket_.fd_ != -1, "Failed to create listener socket");

        epoll_add(&listener_socket_);
        std::cout << "[server] listening on port " << port << "\n";
    }

    // Ask epoll what happened, sort events into buckets, accept new connections.
    void poll() noexcept {
        // Clean up disconnected sockets from the last cycle.
        for (auto* socket : disconnected_sockets_) del(socket);
        disconnected_sockets_.clear();
        receive_sockets_.clear();
        send_sockets_.clear();

        const int n = epoll_wait(efd_, events_,
                                 static_cast<int>(1 + sockets_.size()), 0);

        bool have_new_connection = false;

        for (int i = 0; i < n; ++i) {
            auto& event = events_[i];
            auto* socket = reinterpret_cast<TCPSocket*>(event.data.ptr);

            if (event.events & EPOLLIN) {
                if (socket == &listener_socket_)
                    have_new_connection = true;
                else
                    receive_sockets_.push_back(socket);
            }
            if (event.events & EPOLLOUT)
                send_sockets_.push_back(socket);
            if (event.events & (EPOLLERR | EPOLLHUP))
                disconnected_sockets_.push_back(socket);
        }

        // Accept any new incoming connections.
        while (have_new_connection) {
            sockaddr_storage addr;
            socklen_t addr_len = sizeof(addr);
            int fd = accept(listener_socket_.fd_,
                            reinterpret_cast<sockaddr*>(&addr), &addr_len);
            if (fd == -1) break;

            setNonBlocking(fd);
            setNoDelay(fd);

            auto* new_socket = new TCPSocket();
            new_socket->fd_            = fd;
            new_socket->recv_callback_ = recv_callback_;

            epoll_add(new_socket);
            sockets_.push_back(new_socket);
            receive_sockets_.push_back(new_socket);

            std::cout << "[server] accepted new client (fd=" << fd << ")\n";
        }
    }

    // Read from all sockets with data, flush sends on all sockets that are ready.
    void sendAndRecv() noexcept {
        bool received = false;
        for (auto* socket : receive_sockets_)
            if (socket->sendAndRecv()) received = true;

        if (received) recv_finished_callback_();

        for (auto* socket : send_sockets_)
            socket->sendAndRecv();
    }
};

// ─── Main ─────────────────────────────────────────────────────────────────────
int main() {
    using namespace std::literals::chrono_literals;

    // ── Server callback: echo back what was received ──────────────────────────
    auto serverRecvCallback = [](TCPSocket* socket, Nanos) noexcept {
            const std::string msg(socket->rcv_buffer_, socket->next_rcv_valid_index_);
            socket->next_rcv_valid_index_ = 0;  // mark receive buffer consumed

            std::cout << "[server] received: \"" << msg << "\"\n";

            const std::string reply = "ACK: " + msg;
            socket->send(reply.data(), reply.size());  // stage the reply
    };

    // ── Client callback: print the server's reply ─────────────────────────────
    auto clientRecvCallback = [](TCPSocket* socket, Nanos) noexcept {
            const std::string reply(socket->rcv_buffer_, socket->next_rcv_valid_index_);
            socket->next_rcv_valid_index_ = 0;
            std::cout << "[client] got reply: \"" << reply << "\"\n";
    };

    // ── Start the server ──────────────────────────────────────────────────────
    TCPServer server;
    server.recv_callback_ = serverRecvCallback;
    server.listen("lo", 12345);

    // ── Connect 3 clients ─────────────────────────────────────────────────────
    std::vector<TCPSocket*> clients(3);
    for (size_t i = 0; i < clients.size(); ++i) {
        clients[i] = new TCPSocket();
        clients[i]->recv_callback_ = clientRecvCallback;
        clients[i]->connect("127.0.0.1", "lo", 12345, /*is_listening=*/false);
        std::cout << "[client " << i << "] connected\n";

        // Poll after each connect so the server can accept() before the next
        // client tries to connect.
        std::this_thread::sleep_for(50ms);
        server.poll();
    }

    std::cout << "\n--- starting message exchange ---\n\n";

    // ── 3 rounds of send/receive ──────────────────────────────────────────────
    for (int round = 0; round < 3; ++round) {
        for (size_t i = 0; i < clients.size(); ++i) {
            const std::string msg = "client-" + std::to_string(i)
                                    + " round-" + std::to_string(round);

            // Stage the message and flush it to the wire.
            clients[i]->send(msg.data(), msg.size());
            clients[i]->sendAndRecv();
        }

        std::this_thread::sleep_for(100ms);

        // Server reads incoming messages, fires callbacks, stages replies.
        server.poll();
        server.sendAndRecv();

        std::this_thread::sleep_for(100ms);

        // Clients read the server's replies.
        for (auto* client : clients)
            client->sendAndRecv();

        std::cout << "\n";
    }

    // ── Cleanup ───────────────────────────────────────────────────────────────
    for (auto* client : clients) delete client;

    std::cout << "[main] done.\n";
    return 0;
}