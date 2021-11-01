// Floyd Warshall ADT interface
// COMP2521 Assignment 2

// Note: You MUST NOT modify this file.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "FloydWarshall.h"
#include "Graph.h"

/**
 * Finds all shortest paths between all pairs of nodes.
 * 
 * The  function  returns  a 'ShortestPaths' structure with the required
 * information:
 * - the number of vertices in the graph
 * - distance matrix
 * - matrix of intermediates (see description above)
 */
ShortestPaths FloydWarshall(Graph g) {
	ShortestPaths sps = {0};
	sps.numNodes = GraphNumVertices(g);

	// Allocate sufficient memory for sps.dist and sps.next matrices
	sps.dist = (int **)malloc(GraphNumVertices(g) * sizeof(int *));
	assert(sps.dist);
	for (int i = 0; i < GraphNumVertices(g); i++) {
		sps.dist[i] = (int *)malloc(GraphNumVertices(g) * sizeof(int));
		assert(sps.dist[i]);
	}
	sps.next = (int **)malloc(GraphNumVertices(g) * sizeof(int *));
	assert(sps.next);
	for (int i = 0; i < GraphNumVertices(g); i++) {
		sps.next[i] = (int *)malloc(GraphNumVertices(g) * sizeof(int));
		assert(sps.next[i]);
	}

	// Initialise sps.dist to INFINITY (and 0 for reflexive path) and sps.next
	// to -1
	for (int src = 0; src < GraphNumVertices(g); src++) {
		AdjList src_edges = GraphOutIncident(g, src);
		for (int dest = 0; dest < GraphNumVertices(g); dest++) {
			sps.dist[src][dest] = (src == dest) ? 0 : INFINITY;
			sps.next[src][src] = -1;
		}
		// For all outgoing edges of all vertices, set the distance to the edge
		// weight and set the next node from the src node
		while (src_edges != NULL) {
			sps.dist[src][src_edges->v] = src_edges->weight;
			sps.next[src][src_edges->v] = src_edges->v;
			src_edges = src_edges->next;
		}
	}

	// Floyd Warshall Theorem - Wikipedia
	for (int mid = 0; mid < GraphNumVertices(g); mid++) {
	  	for (int src = 0; src < GraphNumVertices(g); src++) {
			for (int dest = 0; dest < GraphNumVertices(g); dest++) {
				// Edge Relaxation (while avoiding integer overflow addition)
				if (sps.dist[src][mid] != INFINITY && sps.dist[mid][dest] != INFINITY && 
	 				sps.dist[src][mid] + sps.dist[mid][dest] < sps.dist[src][dest]) {
					sps.dist[src][dest] = sps.dist[src][mid] + sps.dist[mid][dest];
					sps.next[src][dest] = sps.next[src][mid];
				}
			}
		}
	}

	// If the shortest path between two vertices does not exist, set the next
	// node to -1
	for (int src = 0; src < GraphNumVertices(g); src++) {
		for (int dest = 0; dest < GraphNumVertices(g); dest++) {
			if (sps.dist[src][dest] == INFINITY)
				sps.next[src][dest] = -1;
		}
	}

	return sps;
}

/**
 * This  function  is  for  you to print out the ShortestPaths structure
 * while you are debugging/testing your implementation. 
 * 
 * We will not call this function during testing, so you may  print  out
 * the  given  ShortestPaths  structure in whatever format you want. You
 * may choose not to implement this function.
 */
void showShortestPaths(ShortestPaths sps) {
	for (int src = 0; src < sps.numNodes; src++) {
		for (int dest = 0; dest < sps.numNodes; dest++) {
			if (sps.dist[src][dest] == INFINITY) {
				printf("%5s", "INF");
			} else {
				printf("%5d", sps.dist[src][dest]);
			}
		}
		printf("\n");
	}
}

/**
 * Frees  all  memory associated with the given ShortestPaths structure.
 * We will call this function during testing, so you must implement it.
 */
void freeShortestPaths(ShortestPaths sps) {
	// Free sps.dist and sps.next matrices
	for (int i = 0; i < sps.numNodes; i++)
		free(sps.dist[i]);
	free(sps.dist);
	for (int i = 0; i < sps.numNodes; i++)
		free(sps.next[i]);
	free(sps.next);
}

