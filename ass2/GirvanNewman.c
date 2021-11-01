// Girvan-Newman Algorithm for community discovery
// COMP2521 Assignment 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "CentralityMeasures.h"
#include "GirvanNewman.h"
#include "Graph.h"

#define vertex(d)  ((d)->vertex)
#define left(d)  ((d)->left)
#define right(d) ((d)->right)

static Dendrogram doGirvanNewman(Graph g, Dendrogram d, EdgeValues evs, 
								int *v, int len);
static void removeMaxEdges(Graph g, EdgeValues edgeCentrality);
static Dendrogram createDendrogram();
static void connectedComponentArray(Graph g, int *cc);
static void dfsComponent(Graph g, Vertex v, int componentID, int *cc);
static Dendrogram merge(Dendrogram d, Dendrogram d1, Dendrogram d2);

/**
 * Generates  a Dendrogram for the given graph g using the Girvan-Newman
 * algorithm. This function is used as a wrapper function for the 
 * recursive doGirvanNewman() function.
 * 
 * The function returns a 'Dendrogram' structure.
 */
Dendrogram GirvanNewman(Graph g) {
	int num_vertices = GraphNumVertices(g);
	// Create an array holding all vertices
	int vertices[num_vertices];
	for (int v = 0; v < num_vertices; v++)
		vertices[v] = v;
		
	if (num_vertices == 0) return NULL;

	// Create an empty dendrogram
	Dendrogram d = createDendrogram();

	// Calculate edge betweenness values of graph structure
	EdgeValues evs = edgeBetweennessCentrality(g);

	// Construct dendrogram using recursive function
	d = doGirvanNewman(g, d, evs, vertices, num_vertices);

	// Free edge betweenness values
	freeEdgeValues(evs);

	return d;
}

// Creates a new dendrogram node with vertex initialised to -1 (NULL)
static Dendrogram createDendrogram() {
   Dendrogram new = malloc(sizeof(DNode));
   assert(new != NULL);
   vertex(new) = -1;
   left(new) = right(new) = NULL;
   return new;
}

// Recursive function implementing Girvan Newman algorithm
static Dendrogram doGirvanNewman(Graph g, Dendrogram d, EdgeValues evs, 
								int *v, int len) {
	
	// Base Cases
	if (len == 1) { // One vertex
		vertex(d) = v[0];
		return d;
	} else if (len == 2) { // Two vertices
		if (!left(d)) left(d) = createDendrogram();
		if (!right(d)) right(d) = createDendrogram();
		vertex(left(d)) = v[0];
		vertex(right(d)) = v[1];
		return d;
	}

	// Remove edges with the highest edge betweenness
	removeMaxEdges(g, evs);

	// Construct connected components array
	int cc[GraphNumVertices(g)];
	connectedComponentArray(g, cc);

	// Declare arrays to hold vertices that split from the
	// original graph structure
	int newSubGraph1[len];
	int newSubGraph2[len];
	int newLen1 = 0, newLen2 = 0;
	
	for (int i = 0; i < len; i++) {
		if (cc[v[i]] != cc[v[0]]) { // Split component if 
									// componentID does not 
									// match that of v[0]
			newSubGraph1[newLen1++] = v[i];
		} else { // Same Connected Component ID
			newSubGraph2[newLen2++] = v[i];
		}
	}
	
	// Construct dendrograms for the two split component arrays
	Dendrogram d1 = createDendrogram();
	doGirvanNewman(g, d1, evs, newSubGraph1, newLen1);
	Dendrogram d2 = createDendrogram();
	doGirvanNewman(g, d2, evs, newSubGraph2, newLen2);
	
	// Return a merged dendrogram with d1 and d2 as left and right
	// children
	return merge(d, d1, d2);
}

/** Create a connected components array updating the cc array provided
 *  as function argument.
 */
static void connectedComponentArray(Graph g, int *cc) {
	// Initialise cc array to -1 (marking each vertex as unvisited)
	for (int i = 0; i < GraphNumVertices(g); i++) {
		cc[i] = -1;
	}

	// Iterate through each connected component and set componentID in
	// cc array
	int componentID = 0;
	for (int i = 0; i < GraphNumVertices(g); i++) {
		if (cc[i] == -1) {
			dfsComponent(g, i, componentID, cc);
			componentID++;
		}
	}
}

/** Find all components that can be traced from given vertex and update
 *  the corresponding componentID in the connected components array.
 */
static void dfsComponent(Graph g, Vertex v, int componentID, int *cc) {
	cc[v] = componentID;
	AdjList out_edges = GraphOutIncident(g, v);
	while (out_edges != NULL) {
		if (cc[out_edges->v] == -1) {
			dfsComponent(g, out_edges->v, componentID, cc);
		}
		out_edges = out_edges->next;
	}
	
	AdjList in_edges = GraphInIncident(g, v);
	while (in_edges != NULL) {
		if (cc[in_edges->v] == -1) {
			dfsComponent(g, in_edges->v, componentID, cc);
		}
		in_edges = in_edges->next;
	}
}

/** Remove the edges with the maximum edge betweenness values from 
 *  graph structure.
 */
static void removeMaxEdges(Graph g, EdgeValues edgeCentrality) {
	assert(g != NULL);
	int max = 0;
	
	// Find Max Edge Betweenness Value
    for (int src = 0; src < edgeCentrality.numNodes; src++) {
		for (int dest = 0; dest < edgeCentrality.numNodes; dest++ ) {
			if (edgeCentrality.values[src][dest] > max) {
				max = edgeCentrality.values[src][dest];
			}
		}
	}

	// Remove Edges with Max Edge Betweenness and reset Edge Betweenness
	// value to -1
    for (int src = 0; src < edgeCentrality.numNodes; src++) {
		for (int dest = 0; dest < edgeCentrality.numNodes; dest++ ) {
			if (edgeCentrality.values[src][dest] == max) {
				GraphRemoveEdge(g, src, dest);
				edgeCentrality.values[src][dest] = -1;
			}
		}
	}
}

// Merge dendrograms d1 and d2 into d
static Dendrogram merge(Dendrogram d, Dendrogram d1, Dendrogram d2) {
   	d->vertex = -1;
	d->left = d1;
	d->right = d2;
	return d;
}

/**
 * Frees all memory associated with the given Dendrogram structure. We
 * will call this function during testing, so you must implement it.
 */
void freeDendrogram(Dendrogram d) {

	if (left(d)) freeDendrogram(left(d));
	if (right(d))freeDendrogram(right(d));
	if (d) free(d);

}

