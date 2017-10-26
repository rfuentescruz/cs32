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
    int ts;
} Node;

typedef struct PriorityQueue {
    Node *heap[PQ_HEAP_MAX_SIZE];
    int size;
    int ts;
} PriorityQueue;

typedef struct PrefixStack {
    char stack[PQ_HEAP_MAX_SIZE];
    int head;
} PrefixStack;

Node *new_node(char c, int freq);
void destroy_node(Node *n);

PriorityQueue *pq_init(void);
void pq_insert(PriorityQueue *pq, Node *);
Node *pq_extract(PriorityQueue *pq);
void pq_destroy(PriorityQueue *pq);

void build_prefixes(Node *root, PrefixStack *prefixes, char *map[]);
void heapify(PriorityQueue *pq, int root);
void heapsort(PriorityQueue *pq);
int check_order(Node *a, Node *b);

int main(int argc, char *argv[]) {
    int char_count = 0;
    int map[256];
    char *prefix_map[256];

    PriorityQueue *pq = pq_init();

    for (int i = 0; i < 256; i++) {
        map[i] = 0;
    }

    for (int i = 0; i < 256; i++) {
        prefix_map[i] = NULL;
    }

    char c = '\0';
    while ((c = fgetc(stdin)) != '#') {
        if (isalpha(c)) {
            map[(int) c]++;
            char_count++;
        }
    }

    for (int i = 0; i < 256; i++) {
        if (isalpha((char) i)) {
            if (map[i]) {
                #if DEBUG
                    printf("%c: %d\n", (char) i, map[i]);
                #endif
                Node *n = new_node((char) i, map[i]);
                pq_insert(pq, n);
            }
        }
    }
    
    while (pq->size > 1) {
        Node *left = pq_extract(pq);
        Node *right = pq_extract(pq);
        #if DEBUG
            printf("Left: (%c) %.2f\n", left->c, left->freq);
            printf("Right: (%c) %.2f\n", right->c, right->freq);
        #endif
        Node *n = new_node('\0', left->freq + right->freq);
        n->left = left;
        n->right = right;
        pq_insert(pq, n);
    }
    Node *n = pq_extract(pq);

    PrefixStack *prefixes = malloc(sizeof(PrefixStack));
    for (int i = 0; i < PQ_HEAP_MAX_SIZE; i++) {
        prefixes->stack[i] = '\0';
    }
    prefixes->head = -1;

    build_prefixes(n, prefixes, prefix_map);
    free(prefixes);
    destroy_node(n);
    pq_destroy(pq);

    for (int i = 0; i < 256; i++) {
        if (prefix_map[i] != NULL) {
            printf("%c %s\n", (char) i, prefix_map[i]);
            free(prefix_map[i]);
        }
    }
}

Node *new_node(char c, int freq) {
    Node *n = malloc(sizeof(Node));
    n->c = c;
    n->freq = freq;
    n->left = NULL;
    n->right = NULL;
    n->ts = 0;
    return n;
}

PriorityQueue *pq_init() {
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    pq->size = 0;
    pq->ts = 0;
    for (int i = 0; i < PQ_HEAP_MAX_SIZE; i++) {
        pq->heap[i] = NULL;
    }

    return pq;
}

void pq_insert(PriorityQueue *pq, Node *n) {
    if (pq->size == PQ_HEAP_MAX_SIZE) {
        return;
    }
    n->ts = pq->ts;
    pq->heap[pq->size] = n;
    pq->size++;
    pq->ts++;
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

void build_prefixes(Node *root, PrefixStack *prefixes, char *map[]) {
    if (root->c != '\0') {
        int len = strlen(prefixes->stack);
        if (len) {
            map[(int) root->c] = (char *) malloc(len + 1);
            strcpy(map[(int) root->c], prefixes->stack);
        }
    }

    if (root->left != NULL) {
        prefixes->head++;
        prefixes->stack[prefixes->head] = '0';
        build_prefixes(root->left, prefixes, map);
        prefixes->stack[prefixes->head] = '\0';
        prefixes->head--;
    }
    
    if (root->right != NULL) {
        prefixes->head++;
        prefixes->stack[prefixes->head] = '1';
        build_prefixes(root->right, prefixes, map);
        prefixes->stack[prefixes->head] = '\0';
        prefixes->head--;
    }
}

void heapify(PriorityQueue *pq, int root) {
    Node *root_node = pq->heap[root];
    int child = (root * 2) + 1;

    Node *child_node = NULL,
         *sibling = NULL;
    while (child < pq->size) {
        child_node = pq->heap[child];
        if (child + 1 < pq->size) {
            sibling = pq->heap[child + 1];
            if (check_order(child_node, sibling) == 1) {
                child_node = sibling;
                child += 1;
            }
        } else {
            sibling = NULL;
        }

        if (check_order(root_node, child_node) == 1) {
            pq->heap[root] = child_node;
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

/**
 * check_order(Node *a, Node *b)
 * 
 * Compare two nodes and determine which should be a root node when in an
 * almost heap
 * 
 * Returns 1 if b should be root and -1 if a should be root
 */
int check_order(Node *a, Node *b) {
    if (b->freq < a->freq) {
        return 1;
    }

    if (b->freq == a->freq && b->ts < a->ts) {
        return 1;
    }

    return -1;
}
