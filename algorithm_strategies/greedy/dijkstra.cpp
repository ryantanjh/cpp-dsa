#include <iostream>
#include <vector>
using namespace std;

vector<int> dijkstra(vector<vector<int> >& cost, int n, int s) {
    /**
     * n is number of vertice
     * s is index of starting vertex starting from index 1
     * time: O(n^2)
     */
     vector<int> d(n+1, INT_MAX);
     vector<bool> selected(n+1, false);
     d[s] = 0;
     for (int i = 1; i <= n; i++) {
         if (i != s) {
             d[i] = cost[s][i]; // cost from starting vertex to vertex
             // if no link, it assigns to int_max
         }
     }
     selected[s] = true;

     // repeating step
     for (int k = 1; k < n; k++) {
         // select min vertice
         int u;
         int min = INT_MAX;
         for (int i = 1; i <= n; i++) {
             if (!selected[i] && d[i] < min) {
                 min = d[i];
                 u = i;
             }
         }
         selected[u] = true;
         // now relax all those connected to u
         for (int v = 1; v <= n; v++) {
             if (!selected[v] && cost[u][v] != INT_MAX && d[u] + cost[u][v] < d[v]) {
                 d[v] = d[u] + cost[u][v];
             }
         }
     }
    return d;
}


int main() {
    vector<vector<int> > cost(5, vector<int>(5, INT_MAX));
    // Add edges
    cost[1][2] = cost[2][1] = 10;
    cost[1][3] = cost[3][1] = 6;
    cost[3][2] = cost[2][3] = 1;
    cost[1][4] = cost[4][1] = 20;
    cost[2][4] = cost[4][2] = 0;
    cost[3][4] = cost[4][3] = 4;
    vector<int> res = dijkstra(cost, 4, 1);
    for (int i = 1; i <= 4; i++) {
        cout << "Vertex " << i << ": " << res[i] << endl;
    }
    return 0;
}
