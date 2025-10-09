#include <iostream>
#include <climits>
using namespace std;

int prim(int** cost, int n) {
    /**
     * n is number of vertice
     * cost is the adjacency matrix representing graphs
     */
     int min_cost = 0;
     // near array where index is each vertice,
     // and value either u or v, depending on which one is closer to that vertice
     int* near = new int[n + 1]; // starting from index 1 to n
     bool* selected = new bool[n + 1];

     // Initialize selected array
     for (int i = 1; i <= n+1; i++) {
         selected[i] = false;
     }

     // initial step
     int u = 1; // u is the currently selected vertice. select vertice 1 first
     int v; // v is the min cost vertice
     int min = INT_MAX;

     // select min cost vertice connected to 1, and update the near and sorted arr
     for (int i = 1; i <= n; i++) {
         if (cost[1][i] < min) {
             min = cost[1][i];
             v = i;
         }
     }
     selected[u] = selected[v] = true;
     min_cost += cost[u][v];
     for (int i = 1; i <= n; i++) { // scan through linked vertices
         // for each vertice, record of it is nearer to u or v
         if (cost[u][i] < cost[v][i]) {
             near[i] = u;
         } else {
             near[i] = v;
         }
     }

     // repeating step
     // MST needs n-1 edges. but we already added one edge, so iterate n-2 times
     for (int i = 1; i <= n-2; i++) {
         // iterate through the near array and assign v to the next min cost vertice
         // this is the vertice nearest to EITHER U or V
         min = INT_MAX;
         for (int j = 1; j <= n; j++) {
             if (!selected[j] && cost[j][near[j]] < min) {
                 min = cost[j][near[j]];
                 v = j;
             }
         }
         // v is now a new vertice
         // u is now the vertice connected to that new vertice
         u = near[v];
         min_cost += cost[u][v];
         selected[v] = true;

         // update the near array
         for (int k = 1; k <= n; k++) {
             // if vertice not yet selected
             // compare cost of that vertice connected to V
             // AGAINST
             // cost of that vertice connected to its currently nearest vertice
             // if that vertice is nearer to v than its currently nearest vertice
             // update near[that vertice] to v
             if (!selected[k] && cost[k][v] < cost[k][near[k]]) {
                 near[k] = v;
             }
         }
     }
     return min_cost;
}

int main() {
    // Same graph as in kruskal.cpp
    // Kruskal uses 0-indexed (0,1,2,3), Prim uses 1-indexed (1,2,3,4)
    int n = 4;

    // Create adjacency matrix (1-indexed)
    int** cost = new int*[n + 1];
    for (int i = 0; i <= n; i++) {
        cost[i] = new int[n + 1];
        for (int j = 0; j <= n; j++) {
            cost[i][j] = INT_MAX;
        }
    }

    // Add edges (matching kruskal.cpp graph)
    // Kruskal: {0, 1, 10} -> Prim: 1-2 with weight 10
    cost[1][2] = cost[2][1] = 10;
    // Kruskal: {0, 2, 6} -> Prim: 1-3 with weight 6
    cost[1][3] = cost[3][1] = 6;
    // Kruskal: {0, 3, 5} -> Prim: 1-4 with weight 5
    cost[1][4] = cost[4][1] = 5;
    // Kruskal: {1, 3, 15} -> Prim: 2-4 with weight 15
    cost[2][4] = cost[4][2] = 15;
    // Kruskal: {2, 3, 4} -> Prim: 3-4 with weight 4
    cost[3][4] = cost[4][3] = 4;

    int result = prim(cost, n);
    cout << "Min Cost: " << result << endl;

    // Free memory
    for (int i = 0; i <= n; i++) {
        delete[] cost[i];
    }
    delete[] cost;

    return 0;
}
