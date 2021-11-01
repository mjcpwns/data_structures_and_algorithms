// calculateViralTransmission.c ... implementation of
// calculateViralTransmission function

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "Graph.h"
#include "Queue.h"

/*
    You will submit only this one file.

    Implement the function "calculateViralTransmission" below.
    Read the exam paper for a detailed specification and
    description of your task.

    - DO NOT modify the code in any other files except for debugging
      purposes.
    - If you wish, you can add static variables and/or helper functions
      to this file.
    - DO NOT add a "main" function to this file.
*/

// This power function (O(logn)) has been borrowed from tutorial resources.
static double power(double x, unsigned int n)
{
	if (n == 0) return 1;
	if (n == 1) return x;
	if (n % 2 == 0) { // n is even
		return power(x * x, n / 2);
	} else { // n is odd
		return x * power(x * x, n / 2);
	}
}

void calculateViralTransmission(Graph g, int src, int srcViralLoad,
                                double *trasmissionArray) {

  // First create the shortest distance array
  int numVertices = GraphNumVertices(g);
	int *visited = malloc(sizeof(int) * numVertices);
	int dist[GraphNumVertices(g)];
	for (int v = 0; v < numVertices; v++) visited[v] = -1, dist[v] = INT_MAX;

	visited[src] = src;
	dist[src] = 0;

	Queue q = QueueNew();
	QueueEnqueue(q, src);
	while(QueueSize(q) != 0) {
		src = QueueDequeue(q);
		for (int v = 0; v < numVertices; v++) {
			if (GraphIsAdjacent(g, src, v) && visited[v] == -1) {
				if (dist[src] + 1 < dist[v]) dist[v] = dist[src] + 1;
				visited[v] = src;
				QueueEnqueue(q, v);
			}
		}
	}
	QueueFree(q);
	free(visited);

  for (int v = 0; v < numVertices; v++) {
    if (dist[v] != INT_MAX) { // Shortest distance exists
      trasmissionArray[v] = srcViralLoad * power(0.6, dist[v]);
      if (trasmissionArray[v] < 10) trasmissionArray[v] = 0;
    } else {
      trasmissionArray[v] = -1;
    }
  }
}

