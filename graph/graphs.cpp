#include <iostream>
#include "queue"

using namespace std;

void BFS(int G[7][7], int start, int n) {
    // initialise queue and visited array
    int visited[7] = {}; // all set to 0 ie. not visited
    queue<int> q;

    // set starting vertice
    visited[start] = 1;
    q.push(start);
    cout << start << endl;

    while (!q.empty()) {
        // pop first element
        int v = q.front();
        q.pop();
        for (int j = 1; j < n; j++) {
            if (G[v][j] == 1 && visited[j] == 0) {
                cout << j << endl;
                visited[j] = 1;
                q.push(j);
            }
        }
    }
}

void DFS(int G[7][7], int start, int n) {
    static int visited[7] = {}; // static so same array can be accessed at every call
    if (visited[start] == 0) {
        cout << start << endl;
        visited[start] = 1;
        for (int j = 1; j < n; j++) {
            if (G[start][j] == 1 && visited[j] == 0) {
                DFS(G, j, n);
            }
        }
    }
}

int main() {
    int G[7][7] = {
            {0,0,0,0,0,0,0},
            {0,0,1,1,0,0,0},
            {0,1,0,0,1,0,0},
            {0,1,0,0,1,0,0},
            {0,0,1,1,0,1,1},
            {0,0,0,0,1,0,0},
            {0,0,0,0,1,0,0}
    };
//    BFS(G, 4, 7);
    DFS(G, 1, 7);
    return -1;
}