// Centrality Measures ADT interface
// COMP2521 Assignment 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "CentralityMeasures.h"
#include "FloydWarshall.h"
#include "Graph.h"
#include "assert.h"

static int pathCheck(Vertex src, Vertex dest, int **next, int *path);

/**
 * Finds  the  edge  betweenness  centrality  for each edge in the given
 * graph and returns the results in a  EdgeValues  structure.  The  edge
 * betweenness centrality of a non-existant edge should be set to -1.0.
 */
EdgeValues edgeBetweennessCentrality(Graph g) {
	EdgeValues e = {0};
	ShortestPaths sps = FloydWarshall(g);
	
	e.numNodes = GraphNumVertices(g);

	// Allocate sufficient memory for e.values
	e.values = (double **)calloc(GraphNumVertices(g), sizeof(double *));
	assert(e.values);
	for (int i = 0; i < GraphNumVertices(g); i++) {
		e.values[i] = (double *)calloc(GraphNumVertices(g),  sizeof(double));
		assert(e.values[i]);
	}
	
	for (int src = 0; src < GraphNumVertices(g); src++) {
		for (int dest = 0; dest < GraphNumVertices(g); dest++) {
			int path[GraphNumVertices(g)];
			int path_len = pathCheck(src, dest, sps.next, path);

			// Path does not exist
			if (path_len == 0) {
				e.values[src][dest] = -1;
				continue;
			}

			// Path exists but vertices are not adjacent
			if (!GraphIsAdjacent(g, src, dest)) {
				e.values[src][dest] = -1;
			}

			// Increment Edge Betweenness of Edge Vertices in Path
			for (int i = 0; i < path_len - 1; i++) {
				e.values[path[i]][path[i + 1]] += 1;
			}
		}
	}
	
	freeShortestPaths(sps);
	return e;
}

/** Modifies path array provided in argument with path from next
 *  array returned in the Floyd Warshall theorem. Returns the length
 *  of the path.
 */
static int pathCheck(Vertex src, Vertex dest, int **next, int *path) {
    if (next[src][dest] == -1) return 0;
	path[0] = src;
	int i = 1;
    while (src != dest) {
        src = next[src][dest];
        path[i] = src;
		i++;
	}
	return i;
}

/**
 * Frees all memory associated with the given EdgeValues  structure.  We
 * will call this function during testing, so you must implement it.
 */
void freeEdgeValues(EdgeValues evs) {
	for (int i = 0; i < evs.numNodes; i++) {
		free(evs.values[i]);
	}
	free(evs.values);
}


