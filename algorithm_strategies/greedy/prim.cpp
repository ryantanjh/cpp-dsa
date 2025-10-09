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
