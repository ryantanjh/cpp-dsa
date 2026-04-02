//
// Created by ryan tan on 1/4/26.
//

/*
 * RAII: tie resource life cycle to object lifetime ie. constructor acquire, destructor release
 */

#include "fstream"
#include "iostream"
#include "mutex"

using namespace std;

class FileHandler {
    ofstream file_;
public:
    FileHandler(const string& path) : file_(path){
        if (!file_.is_open()) throw runtime_error("Cannot open " + path);
        cout << "File opened \n";
    }
    void write(const string& data) {
        file_ << data;
    }
    ~FileHandler() {
        cout << "File closed \n";
    }
};

class Buffer {
    int* data_;
    size_t size_;
public:
    Buffer(size_t size) : size_(size) {
        data_ = new int[size];   // allocates on heap
        std::cout << "Allocated " << size << " ints\n";
    }

    void set(size_t i, int val) { data_[i] = val; }
    int  get(size_t i)          { return data_[i]; }

    ~Buffer() {
        delete[] data_;          // YOU must free it — nothing else will
        std::cout << "Freed heap memory\n";
    }
};