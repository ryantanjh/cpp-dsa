#include <iostream>
#include <queue>

using namespace std;

struct Edge {
    int u;
    int v;
    int weight;

    bool operator>(const Edge &other) const {
        return weight > other.weight;
    }
};

void Union(int A[], int u, int v) {
    /**
     * Combine 2 parents
     */
    if (A[u] < A[v]) {
        // u becomes parent
        A[u] = A[u] + A[v];
        A[v] = u;
    } else {
        A[v] = A[u] + A[v];
        A[u] = v;
    }
}


int Find(int A[], int i) {
    /**
     * Returns index of parent node
     * A is disjoint subset, i is node index
     */
     while (A[i] > 0) {
         i = A[i];
     }
     return i;
}

int kruskal(int edges[][3], int num_edge, int num_vertice) {
    /**
     * edges: edge list
     * n is number of vertices
     */
    priority_queue<Edge, vector<Edge>, greater<Edge> > pq;
    for (int i = 0; i < num_edge; i++) {
        Edge e = {edges[i][0], edges[i][1], edges[i][2]};
        pq.push(e);
    }
    int min_cost = 0;
    int edge_count = 0;

    // initialise disjoint subset
    int DS[num_vertice];
    for (int i = 0; i < num_vertice; i++) {
        DS[i] = -1;
    }
    // select min edge one by one
    while (edge_count < num_vertice - 1 && !pq.empty()){
        Edge e = pq.top();
        pq.pop();
        int root_u = Find(DS, e.u);
        int root_v = Find(DS, e.v);
        // check cycle, if no cycle combine
        if (root_u != root_v) {
            Union(DS, root_u, root_v);
            min_cost += e.weight;
            edge_count++;
        }
    }
    return min_cost;
}

int main() {
    // Example connected graph with 4 vertices (0, 1, 2, 3)
    // Edge format: {vertex1, vertex2, weight}
    int edges[][3] = {
            {0, 1, 10},
            {0, 2, 6},
            {0, 3, 5},
            {1, 3, 15},
            {2, 3, 4}
    };

    int numEdges = 5;
    int numVertices = 4;

    int result = kruskal(edges, numEdges, numVertices);
    cout << "Min Cost: " << result << endl;

    return 0;
}