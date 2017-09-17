#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

////////////////////////// Graph implementacija

struct AdjListNode
{
	int dest;
	int weight;
	struct AdjListNode* next;
};

struct AdjList
{
	struct AdjListNode *head;
};

struct Graph {
	int V;	// broj cvorova :: vertices
	struct AdjList* array;
};

struct AdjListNode* newAdjListNode(int dest, int weight){
	struct AdjListNode* newNode = (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
	newNode->dest = dest;
	newNode->weight = weight;
	newNode->next = NULL;
	return newNode;
}

struct Graph* createGraph(int V)
{
	struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
	graph->V = V;
	graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));	// lista potega
	for (int i = 0; i < V; ++i)
		graph->array[i].head = NULL;
	return graph;
}

void addEdge(struct Graph* graph, int src, int dest, int weight)	// vertex na pocetak grafa :: src->dest (weight) poteg
{
	struct AdjListNode* newNode = newAdjListNode(dest, weight);
	newNode->next = graph->array[src].head;
	graph->array[src].head = newNode;	// poteg na pocetak (head) liste potega src vertex-a :: graph->array[src]

	// bidirekcioni graf dest->src src->dest
	newNode = newAdjListNode(src, weight);
	newNode->next = graph->array[dest].head;
	graph->array[dest].head = newNode;
}

////////////////////////// Min-heap implementacija

struct MinHeapNode
{
	int  v;
	int key;
};

struct MinHeap
{
	int size;
	int capacity;
	int *pos;
	struct MinHeapNode **array;
};

struct MinHeapNode* newMinHeapNode(int v, int key)
{
	struct MinHeapNode* minHeapNode = (struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
	minHeapNode->v = v;
	minHeapNode->key = key;
	return minHeapNode;
}

struct MinHeap* createMinHeap(int capacity)
{
	struct MinHeap* minHeap = (struct MinHeap*) malloc(sizeof(struct MinHeap));
	minHeap->pos = (int *)malloc(capacity * sizeof(int));
	minHeap->size = 0;
	minHeap->capacity = capacity;
	minHeap->array = (struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*));
	return minHeap;
}

void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
	struct MinHeapNode* t = *a;
	*a = *b;
	*b = t;
}

void minHeapify(struct MinHeap* minHeap, int idx)
{
	int smallest, left, right;
	smallest = idx;
	left = 2 * idx + 1;
	right = 2 * idx + 2;

	if (left < minHeap->size && minHeap->array[left]->key < minHeap->array[smallest]->key)
		smallest = left;

	if (right < minHeap->size && minHeap->array[right]->key < minHeap->array[smallest]->key)
		smallest = right;

	if (smallest != idx)
	{
		MinHeapNode *smallestNode = minHeap->array[smallest];
		MinHeapNode *idxNode = minHeap->array[idx];
		minHeap->pos[smallestNode->v] = idx;
		minHeap->pos[idxNode->v] = smallest;
		swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
		minHeapify(minHeap, smallest);
	}
}

int isEmpty(struct MinHeap* minHeap)
{
	return minHeap->size == 0;
}

struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
	if (isEmpty(minHeap))
		return NULL;
	struct MinHeapNode* root = minHeap->array[0];
	struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
	minHeap->array[0] = lastNode;
	minHeap->pos[root->v] = minHeap->size - 1;
	minHeap->pos[lastNode->v] = 0;
	--minHeap->size;
	minHeapify(minHeap, 0);
	return root;
}

// promena kljuca cvora v
void decreaseKey(struct MinHeap* minHeap, int v, int key)
{
	// index v u heapu
	int i = minHeap->pos[v];
	// promena key vrdnosti cvora
	minHeap->array[i]->key = key;
	// heapify cele strukture bottom-up
	while (i && minHeap->array[i]->key < minHeap->array[(i - 1) / 2]->key)
	{
		minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
		minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
		swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

bool isInMinHeap(struct MinHeap *minHeap, int v)
{
	if (minHeap->pos[v] < minHeap->size)
		return true;
	return false;
}

void printArr(int* arr, int n)
{
	printf("Minimal spanning tree :: child <-- parent\n");
	for (int i = 1; i < n; ++i)
		printf("%d <-- %d\n", arr[i], i);
}

////////////////////////// Minimum Spanning Tree konstrukcija

void mstConstructPrim(struct Graph* graph, bool condition) {
	int V = graph->V;		// broj cvorova
	const int nodeNumber = V;
	int* mst = new int[nodeNumber];		// niz za napravljeno minimalno sprezno stablo mst
	int* key = new int[nodeNumber];			// key vrednosti :: koriste se za biranje potega najmanje tezine u podskupu

	// inicijalizacija minHeap-a svim cvorovima :: inicijalno key vrednosti infinite, osim prvog cvora
	struct MinHeap* minHeap = createMinHeap(V);
	for (int v = 1; v < V; ++v) {
		mst[v] = -1;
		key[v] = INT_MAX;	// key vrednost svakog cvora u minHeapu inicijalno infinite :: key predstavlja najmanju tezinu grane do tog cvora
		minHeap->array[v] = newMinHeapNode(v, key[v]);	// cvor v :: key infinite
		minHeap->pos[v] = v;
	}
	// 0-ti cvor inicijalno key vrednost 0 :: ekstraktovan prvi iz minHeap-a
	// pocetni cvor za algoritam :: nije nijednom drugom cvoru parent u mst stablu (na kraju)
	key[0] = 0;
	minHeap->array[0] = newMinHeapNode(0, key[0]);
	minHeap->pos[0] = 0;

	minHeap->size = V;
	// minHeap sadrzi sve cvorove koji jos uvek nisu prododati minimalnom spreznom stablu MST
	while (!isEmpty(minHeap)) {		// MST konstruisano kada su preuzeti svi cvorovi
		// ekstrakcija cvora sa najmajom key vrednoscu
		struct MinHeapNode* minHeapNode = extractMin(minHeap);
		int u = minHeapNode->v;		// broj ekstraktovanog cvora

		// extractMin :: za prolazak redom susednih covorova cvora u
		// traverse svih susednih cvorova :: promena key vrednosti svakog :: graph->array[u] za pristup ekstraktovanom cvoru iz minHeap strukture i njegovim susedima
		struct AdjListNode* Edge = graph->array[u].head;	// lista potega cvora u
		while (Edge != NULL) {
			int v = Edge->dest; // traverse je tekuci poteg u-... gde je ... redom jedan od susednih cvorova cvora u
			// v je odredisni cvor grane :: ako cvor v nije u mst stablu (jeste u heapu) i tezina u-v grane
			// je manja od key vrednosti cvora v, promena key i parent vrednosti cvora v :: parent vrednost -> mst ; key vrednost -> tezina grane do tog cvora
			if (isInMinHeap(minHeap, v) && Edge->weight < key[v]) {		// ako je tezina grane manja od trenutne tezine do tog cvora (key) zamena
				key[v] = Edge->weight;
				mst[v] = u;		// parent cvora postaje cvor od kog ide grana traverse (u) :: to je grana sa manjom tezinom nego prethodna
				decreaseKey(minHeap, v, Edge->weight);	// smanjenje kljuca na traverse->weight
			}
			Edge = Edge->next;
		}
	}

	if(condition)
		printArr(mst, V);	// prikaz grana minimalnog spreznog stabla
}