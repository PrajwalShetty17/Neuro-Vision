#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF INT_MAX
#define MAX_NODES 10

// Graph definition: 10 Core Global Datacenter Hubs
// 0: San Francisco, 1: New York, 2: London, 3: Frankfurt, 4: Bengaluru, 
// 5: Tokyo, 6: Sydney, 7: Sao Paulo, 8: Cape Town, 9: Dubai
int graph[MAX_NODES][MAX_NODES] = {
    {0, 45, INF, INF, INF, 80, 120, INF, INF, INF},  // 0: SF
    {45, 0, 55, INF, INF, INF, INF, 90, INF, INF},   // 1: NY
    {INF, 55, 0, 15, 110, INF, INF, INF, 130, INF},  // 2: London
    {INF, INF, 15, 0, 95, INF, INF, INF, INF, 60},   // 3: Frankfurt
    {INF, INF, 110, 95, 0, 65, INF, INF, INF, 45},   // 4: Bengaluru
    {80, INF, INF, INF, 65, 0, 95, INF, INF, INF},   // 5: Tokyo
    {120, INF, INF, INF, INF, 95, 0, INF, INF, INF}, // 6: Sydney
    {INF, 90, INF, INF, INF, INF, INF, 0, 140, INF}, // 7: Sao Paulo
    {INF, INF, 130, INF, INF, INF, INF, 140, 0, 110},// 8: Cape Town
    {INF, INF, INF, 60, 45, INF, INF, INF, 110, 0}   // 9: Dubai
};

void dijkstra(int src, int dest, int dead_node) {
    int dist[MAX_NODES];
    int visited[MAX_NODES];
    int parent[MAX_NODES];

    for (int i = 0; i < MAX_NODES; i++) {
        dist[i] = INF;
        visited[i] = 0;
        parent[i] = -1;
    }

    if (src == dead_node || dest == dead_node) {
        printf("ERROR|Source or destination node is down.\n");
        return;
    }

    dist[src] = 0;

    for (int count = 0; count < MAX_NODES - 1; count++) {
        int min = INF, u = -1;

        for (int v = 0; v < MAX_NODES; v++) {
            if (!visited[v] && dist[v] <= min && v != dead_node) {
                min = dist[v];
                u = v;
            }
        }

        if (u == -1) break;
        visited[u] = 1;

        for (int v = 0; v < MAX_NODES; v++) {
            if (!visited[v] && graph[u][v] && graph[u][v] != INF && v != dead_node) {
                if (dist[u] + graph[u][v] < dist[v]) {
                    dist[v] = dist[u] + graph[u][v];
                    parent[v] = u;
                }
            }
        }
    }

    if (dist[dest] == INF) {
        printf("ERROR|No routing path available due to node failure.\n");
        return;
    }

    int path[MAX_NODES];
    int path_index = 0;
    int curr = dest;
    
    while (curr != -1) {
        path[path_index++] = curr;
        curr = parent[curr];
    }

    printf("RESULT|%d|", dist[dest]);
    for (int i = path_index - 1; i >= 0; i--) {
        printf("%d", path[i]);
        if (i > 0) printf(",");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("ERROR|Missing parameters.\n");
        return 1;
    }

    int src = atoi(argv[1]);
    int dest = atoi(argv[2]);
    int dead_node = (argc == 4) ? atoi(argv[3]) : -1;

    dijkstra(src, dest, dead_node);
    return 0;
}

    
    

    