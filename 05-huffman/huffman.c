#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PQ_HEAP_MAX_SIZE 52
typedef struct Node {
    char c;
    int freq;
    struct Node *left;
    struct Node *right;
    int tag;
} Node;

typedef struct PriorityQueue {
    Node *heap[PQ_HEAP_MAX_SIZE];
    int size;
} PriorityQueue;

Node *new_node(char c, int freq);
void destroy_node(Node *n);

PriorityQueue *pq_init(void);
void pq_insert(PriorityQueue *pq, Node *);
Node *pq_extract(PriorityQueue *pq);
void pq_destroy(PriorityQueue *pq);

void heapify(PriorityQueue *pq, int root);
void heapsort(PriorityQueue *pq);

int main(int argc, char *argv[]) {
    int char_count = 0;
    int map[256];

    PriorityQueue *pq = pq_init();

    for (int i = 0; i < 256; i++) {
        map[i] = 0;
    }

    char c = '\0';
    while ((c = fgetc(stdin)) != '#') {
        if (isalpha(c)) {
            map[(int) c]++;
            char_count++;
        }
    }

    float freq = 0.0;
    for (int i = 0; i < 256; i++) {
        if (isalpha((char) i)) {
            freq = (float) map[i] / char_count;
            freq *= 10000; // 100.00
            if (map[i]) {
                Node *n = new_node((char) i, (int) freq);
                pq_insert(pq, n);
            }
        }
    }

    while (pq->size > 1) {
        Node *left = pq_extract(pq);
        left->tag = 0;
        Node *right = pq_extract(pq);
        right->tag = 1;
        Node *n = new_node('\0', left->freq + right->freq);
        n->left = left;
        n->right = right;
        pq_insert(pq, n);
    }
    Node *n = pq_extract(pq);
    destroy_node(n);
    pq_destroy(pq);
}

Node *new_node(char c, int freq) {
    Node *n = malloc(sizeof(Node));
    n->c = c;
    n->freq = freq;
    n->left = NULL;
    n->right = NULL;
    n->tag = 0;
    return n;
}

PriorityQueue *pq_init() {
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    pq->size = 0;
    for (int i = 0; i < PQ_HEAP_MAX_SIZE; i++) {
        pq->heap[i] = NULL;
    }

    return pq;
}

void pq_insert(PriorityQueue *pq, Node *n) {
    if (pq->size == PQ_HEAP_MAX_SIZE) {
        return;
    }
    pq->heap[pq->size] = n;
    pq->size++;
    heapsort(pq);
}

Node *pq_extract(PriorityQueue *pq) {
    if (pq->size == 0) {
        return NULL;
    }

    Node *n = pq->heap[0];
    pq->size--;
    pq->heap[0] = pq->heap[pq->size];
    pq->heap[pq->size] = NULL;
    heapify(pq, 0);
    return n;
}

void pq_destroy(PriorityQueue *pq) {
    for (int i = 0; i < PQ_HEAP_MAX_SIZE; i++) {
        if (pq->heap[i] != NULL) {
            destroy_node(pq->heap[i]);
        }
    }

    free(pq);
}

void destroy_node(Node *n) {
    if (n->left != NULL) {
        destroy_node(n->left);
    }
    if (n->right != NULL) {
        destroy_node(n->right);
    }
    free(n);
}

void heapify(PriorityQueue *pq, int root) {
    Node *root_node = pq->heap[root];
    int child = (root * 2) + 1;

    while (child < pq->size) {
        if (child + 1 < pq->size && pq->heap[child + 1]->freq < pq->heap[child]->freq) {
            child += 1;
        }

        if (pq->heap[child]->freq < root_node->freq) {
            pq->heap[root] = pq->heap[child];
            root = child;
            child = (root * 2) + 1;
        } else {
            break;
        }
    }
    pq->heap[root] = root_node;
}

void heapsort(PriorityQueue *pq) {
    for (int i = (pq->size - 1) / 2; i >= 0; --i) {
        heapify(pq, i);
    }
}
