#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
    int vertex;
    struct Node *next;
} Node;

typedef struct Queue {
    Node *head;
    Node *tail;
    int size;
} Queue;

void adjacency_list_destroy(Node **list, int num_vertices);

Queue *queue_init(void);
void queue_destroy(Queue *q);
void enqueue(Queue *q, int vertex);
int dequeue(Queue *q);

int main(int argc, char *argv[]) {
    int num_cases = 0;
    scanf("%d", &num_cases);
    for (int i = 0; i < num_cases; i++) {
        int num_vertices = 0, num_edges = 0;
        scanf("%d\n%d\n", &num_vertices, &num_edges);
        num_vertices++;
        Node *adj_list[num_vertices];
        int pred[num_vertices];

        for (int j = 0; j < num_vertices; j++) {
            adj_list[j] = NULL;
            pred[j] = 0;
        }

        for (int j = 0; j < num_edges; j++) {
            int a = 0, b = 0;
            scanf("(%d, %d)\n", &a, &b);

            Node *e = malloc(sizeof(Node));
            e->vertex = b;
            e->next = NULL;
            if (adj_list[a] != NULL) {
                e->next = adj_list[a];
            }
            adj_list[a] = e;
            pred[b]++;
        }

        Queue *q = queue_init();
        for (int j = 1; j < num_vertices; j++) {
            #if DEBUG
                printf("%d: %d\n", j, pred[j]);
            #endif
            if (pred[j] == 0) {
                enqueue(q, j);
            }
        }

        while (q->size > 0) {
            int v = dequeue(q),
                target = 0;

            Node *n = adj_list[v];
            while (n != NULL) {
                target = n->vertex;
                pred[target]--;
                if (pred[target] <= 0) {
                    enqueue(q, target);
                }
                Node *tmp = n->next;
                free(n);
                n = tmp;
            }
            printf("%d", v);
            if (q->size > 0) {
                printf(", ");
            }
        }
        printf("\n");

        queue_destroy(q);
    }
}

void adjacency_list_destroy(Node **list, int num_vertices) {
    Node *e = NULL;
    for (int i = 0; i < num_vertices; i++) {
        e = *(list + i);
        while (e != NULL) {
            Node *tmp = e->next;
            free(e);
            e = tmp;
        }
    }
}

Queue *queue_init() {
    Queue *q = malloc(sizeof(Queue));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

void queue_destroy(Queue *q) {
    while (q->size > 0) {
        dequeue(q);
    }
    free(q);
}

void enqueue(Queue *q, int vertex) {
    Node *n = malloc(sizeof(Node));
    n->vertex = vertex;
    n->next = NULL;

    if (q->tail != NULL) {
        q->tail->next = n;
    } else {
        q->head = n;
    }

    q->tail = n;
    q->size++;
}

int dequeue(Queue *q) {
    Node *head = q->head;
    if (head == NULL) {
        return -1;
    }
    int vertex = head->vertex;
    q->head = head->next;
    free(head);
    q->size--;

    if (q->head == NULL) {
        q->tail = NULL;
    }

    return vertex;
}
