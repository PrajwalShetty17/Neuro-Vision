#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF INT_MAX
#define MAX_NODES 6

// Graph definition: 6 Core Global Datacenter Hubs
// 0: San Francisco, 1: New York, 2: London, 3: Frankfurt, 4: Bengaluru, 5: Tokyo
int graph[MAX_NODES][MAX_NODES] = {
    {0, 45, INF, INF, INF, 80},   // 0: San Francisco
    {45, 0, 55, INF, INF, INF},   // 1: New York
    {INF, 55, 0, 15, 110, INF},   // 2: London
    {INF, INF, 15, 0, 95, INF},   // 3: Frankfurt
    {INF, INF, 110, 95, 0, 65},   // 4: Bengaluru
    {80, INF, INF, INF, 65, 0}    // 5: Tokyo
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

    // Handle dead server/node bypass
    if (src == dead_node || dest == dead_node) {
        printf("ERROR|Source or destination node is down.\n");
        return;
    }

    dist[src] = 0;

    for (int count = 0; count < MAX_NODES - 1; count++) {
        int min = INF, u = -1;

        // Find minimum distance node
        for (int v = 0; v < MAX_NODES; v++) {
            if (!visited[v] && dist[v] <= min && v != dead_node) {
                min = dist[v];
                u = v;
            }
        }

        if (u == -1) break;
        visited[u] = 1;

        // Update neighbors
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

    // Reconstruct shortest path array
    int path[MAX_NODES];
    int path_index = 0;
    int curr = dest;
    
    while (curr != -1) {
        path[path_index++] = curr;
        curr = parent[curr];
    }

    // Format output string for Flask: RESULT|total_latency|node,node,node
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