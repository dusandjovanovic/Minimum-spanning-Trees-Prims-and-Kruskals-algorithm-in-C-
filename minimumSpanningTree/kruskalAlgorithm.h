#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////////////////////////// Graph implementacija

struct Edge
{
	int src, dest, weight;
};

struct GraphK
{
	// V-> broj cvorova, E-> broj potega
	int V;
	int E;

	// lista potega :: bidirekcioni src->dest dest->src
	struct Edge* edge;
};

struct GraphK* createGraphK(int V, int E)
{
	struct GraphK* graph = (struct GraphK*) malloc(sizeof(struct GraphK));
	graph->V = V;
	graph->E = E;

	// odvojena lista potega :: kasnije sortirana po tezinama
	graph->edge = (struct Edge*) malloc(graph->E * sizeof(struct Edge));

	return graph;
}

bool edgeExistsInGraph(int src, int dest, GraphK* graph) {
	for (int i = 0; i < graph->E; i++)
		if ((graph->edge[i].src == src && graph->edge[i].dest == dest) || (graph->edge[i].src == dest && graph->edge[i].dest == src))
			return true;
	return false;
}

// Struktura za predstavljanje podskupova
struct subset
{
	int parent;
	int rank;
};

// Nalazenje podskupa elementa i
int find(struct subset subsets[], int i)
{
	if (subsets[i].parent != i)
		subsets[i].parent = find(subsets, subsets[i].parent);

	return subsets[i].parent;
}

// Unija podskupa x sa y
void Union(struct subset subsets[], int x, int y)
{
	int xroot = find(subsets, x);
	int yroot = find(subsets, y);

	if (subsets[xroot].rank < subsets[yroot].rank)
		subsets[xroot].parent = yroot;
	else if (subsets[xroot].rank > subsets[yroot].rank)
		subsets[yroot].parent = xroot;
	else
	{
		subsets[yroot].parent = xroot;
		subsets[xroot].rank++;
	}
}

int myComp(const void* a, const void* b)
{
	struct Edge* a1 = (struct Edge*)a;
	struct Edge* b1 = (struct Edge*)b;
	return a1->weight > b1->weight;
}

void mstContructKruskal(struct GraphK* graph, int edgeNumber, bool condition) {
	int V = graph->V;
	struct Edge* mst = new Edge[V];		// minimalno sprezno stablo
	int e = 0;
	int i = 0;

	// Sortiranje grana u rastuci redosled po tezinama
	qsort(graph->edge, graph->E, sizeof(graph->edge[0]), myComp);

	// Inicijalizacije strukture podskupova, broj podskupova jednak broju cvorova V
	struct subset *subsets = (struct subset*) malloc(V * sizeof(struct subset));
	// Pocetno svi skupovi po jedan elemenat :: za svaki elemenat grafa
	for (int v = 0; v < V; ++v){
		subsets[v].parent = v;
		subsets[v].rank = 0;
	}

	while (i < edgeNumber) {		// razmatranje 0...V-1 grana e-index
		struct Edge nextEdge = graph->edge[i++]; // prva  sledeca grana sa najmanjom tezinom

		// nalazenje source/dest skupova kojima pripadaju src/dst cvorovi
		int sourceSubset = find(subsets, nextEdge.src);
		int destSubset = find(subsets, nextEdge.dest);
		
		// ako grana ne oformi ciklus u rezultantnom grafu ukljucuje se u minimalno sprezno stablo
		if (sourceSubset != destSubset) {
			mst[e++] = nextEdge;
			Union(subsets, sourceSubset, destSubset);
		}
		// u suprotnom grana uzrokuje ciklus i ne razmatra se.. prelazak na sledecu granu po tezini
	}

	if (condition) {
		printf("Minimal spanning tree :: src -- dst == weight\n");
		for (i = 0; i < e; ++i)
			printf("%d -- %d == %d\n", mst[i].src, mst[i].dest, mst[i].weight);
	}
}