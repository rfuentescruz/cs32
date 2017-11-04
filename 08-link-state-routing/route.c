#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define PQ_HEAP_MAX_SIZE 10000

typedef struct Edge {
    int vertex;
    int cost;
    struct Edge *next;
} Edge;

typedef struct Vertex {
    int id;
    int color;
    int distance;
    int path;
} Vertex;

typedef struct Node {
    Vertex *vertex;
    struct Node *left;
    struct Node *right;
    int ts;
} Node;

typedef struct PriorityQueue {
    Node **heap;
    int size;
    int capacity;
    int ts;
} PriorityQueue;

Node *node_create(Vertex *vertex);
void node_destroy(Node *n);

PriorityQueue *pq_init(int capacity);
void pq_insert(PriorityQueue *pq, Node *);
Node *pq_extract(PriorityQueue *pq);
void pq_destroy(PriorityQueue *pq);

void heapify(PriorityQueue *pq, int root);
void heapsort(PriorityQueue *pq);
int check_order(Node *a, Node *b);

void dijkstra(Edge **edges, Vertex **vertices, int start, int num_vertices);

void link_append(Edge **links, int a, int b, int cost);
void link_destroy(Edge *edge);

Vertex *vertex_create(int id);

int main(int argc, char *argv[]) {
    int n, l, s;
    while (1) {
        scanf("%d %d %d", &n, &l, &s);

        if (n == 0 && l == 0 && s == -1) {
            return 0;
        }

        Edge *links[n];
        Vertex *vertices[n];
        for (int i = 0; i < n; i++) {
            links[i] = NULL;
            vertices[i] = NULL;
        }

        int a, b, cost;
        for (int i = 0; i < l; i++) {
            scanf("%d %d %d", &a, &b, &cost);
            if (vertices[a] == NULL) {
                vertices[a] = vertex_create(a);
            }

            if (vertices[b] == NULL) {
                vertices[b] = vertex_create(b);
            }

            link_append(links, a, b, cost);
            link_append(links, b, a, cost);
        }
        dijkstra(links, vertices, s, n);
        #if DEBUG
            printf("CASE: %d %d %d: ", n, l, s);
        #endif

        printf("(");
        for (int i = 0; i < n; i++) {
            int ancestor = -1;
            if (vertices[i]->id != s) {
                ancestor = i;
                while (vertices[ancestor]->path != s) {
                    ancestor = vertices[ancestor]->path;
                }
            }
            printf("%d", ancestor);
            if (i + 1 != n) {
                printf(",");
            }
        }
        printf(")\n");

        for (int i = 0; i < n; i++) {
            #if DEBUG
                printf("%d: ", i);
            #endif
            link_destroy(links[i]);
            #if DEBUG
                printf("\n");
                printf(
                    "Distance from %d: %d through %d\n",
                    s, vertices[i]->distance, vertices[i]->path
                );
            #endif
            free(vertices[i]);
        }
    }
}

void dijkstra(Edge **edges, Vertex **vertices, int start, int num_vertices) {
    PriorityQueue *pq = pq_init(num_vertices);
    for (int i = 0; i < num_vertices; i++) {
        if (vertices[i]->id == start) {
            vertices[i]->distance = 0;
        }
        pq_insert(pq, node_create(vertices[i]));
    }

    Node *n = NULL;
    while (pq->size > 0) {
        n = pq_extract(pq);
        Vertex *from = n->vertex;
        Edge *e = edges[n->vertex->id];
        from->color = 1;
        while (e != NULL) {
            Vertex *to = vertices[e->vertex];
            int new_distance = from->distance + e->cost;
            if (to->color != 1 && new_distance < to->distance) {
                to->distance = from->distance + e->cost;
                to->path = from->id;
                heapsort(pq);
            }
            e = e->next;
        }
        #if DEBUG
            printf("Node: %d\n", n->vertex->id);
        #endif
        free(n);
    }
    pq_destroy(pq);
}

void link_append(Edge **links, int a, int b, int cost) {
    Edge *edge = malloc(sizeof(Edge));
    edge->vertex = b;
    edge->cost = cost;
    edge->next = *(links + a);
    *(links + a) = edge;
}

void link_destroy(Edge *n) {
    if (n == NULL) {
        return;
    }

    Edge *temp = n->next;
    #if DEBUG
        printf("{%d, %d} ", n->vertex, n->cost);
    #endif

    while (temp != NULL) {
        #if DEBUG
            printf("{%d, %d} ", temp->vertex, temp->cost);
        #endif
        free(n);
        n = temp;
        temp = n->next;
    }

    free(n);
}

Vertex *vertex_create(int id) {
    Vertex *vertex = malloc(sizeof(Vertex));
    vertex->id = id;
    vertex->color = 0;
    vertex->distance = INT_MAX;
    vertex->path = -1;

    return vertex;
}

Node *node_create(Vertex *vertex) {
    Node *n = malloc(sizeof(Node));
    n->vertex = vertex;
    n->left = NULL;
    n->right = NULL;
    n->ts = 0;
    return n;
}

void node_destroy(Node *n) {
    if (n->left != NULL) {
        node_destroy(n->left);
    }
    if (n->right != NULL) {
        node_destroy(n->right);
    }
    free(n);
}

PriorityQueue *pq_init(int capacity) {
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    pq->size = 0;
    pq->capacity = capacity;
    pq->ts = 0;
    pq->heap = malloc(sizeof(Node *) * capacity);
    for (int i = 0; i < capacity; i++) {
        pq->heap[i] = NULL;
    }

    return pq;
}

void pq_insert(PriorityQueue *pq, Node *n) {
    if (pq->size >= pq->capacity) {
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
    for (int i = 0; i < pq->capacity; i++) {
        if (pq->heap[i] != NULL) {
            node_destroy(pq->heap[i]);
        }
    }
    free(pq->heap);
    free(pq);
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
    int a_distance = a->vertex->distance,
        b_distance = b->vertex->distance;

    if (b_distance == a_distance && b->ts < a->ts) {
        return 1;
    }

    if (b_distance == INT_MAX) {
        return -1;
    } else if (a_distance == INT_MAX) {
        return 1;
    }

    if (b_distance < a_distance) {
        return 1;
    }

    return -1;
}
