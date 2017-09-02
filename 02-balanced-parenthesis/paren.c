#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

typedef struct Node {
    char data;
    struct Node *next;
} Node;

typedef struct Stack {
    Node *head;
} Stack;

int parse_case(char *, Stack *);

void push(Stack *, char);
char pop(Stack *);
void empty(Stack *);

int main(int argc, char *argv[]) {
    int case_number = 0, n = 0;

    Stack *stack = malloc(sizeof(Stack));
    stack->head = NULL;

    char *line = malloc((size_t) BUFFER_SIZE);
    while (fgets(line, BUFFER_SIZE, stdin) != NULL) {
        if (sscanf(line, "CASE %d:", &n) == 1) {
            case_number = n;
        }

        // We're starting to parse a case or we're in the middle of parsing one
        if (case_number) {
            int case_status = parse_case(line, stack);

            if (case_status != 0) {
                printf("CASE %d: %s\n",
                    case_number,
                    case_status == 1 ? "BALANCED" : "NOT BALANCED"
                );
                empty(stack);
                case_number = 0;
            }
        }
    }
    free(line);

    empty(stack);
    free(stack);

    // We cool? We cool.
    return 0;
}

int parse_case(char *line, Stack *stack) {
    // These lines come from fgets which is assured to have a terminating NULL
    // byte.
    for (char c = *line; c != '\0'; c = *(++line)) {
        if (c == '{' || c == '[' || c == '(') {
            push(stack, c);
        } else if (c == '}' || c == ']' || c == ')') {
            char match = pop(stack);
            if (
                (match == '{' && c != '}') ||
                (match == '[' && c != ']') ||
                (match == '(' && c != ')') ||
                match == EOF // Stack pop() returns EOF if empty
            ) {
                return -1;
            }
        } else if (c == '\n') {
            if (stack->head != NULL) {
                return -1;
            } else {
                return 1;
            }
        }
    }

    return 0;
}

void push(Stack *stack, char c) {
    Node *node = malloc(sizeof(Node));
    node->data = c;

    node->next = stack->head;
    stack->head = node;
}

char pop(Stack *stack) {
    char c;
    if (stack->head == NULL) {
        return EOF;
    }

    Node *tmp = stack->head;
    stack->head = stack->head->next;
    c = tmp->data;
    free(tmp);
    return c;
}

void empty(Stack *stack) {
    while (stack->head != NULL) {
        pop(stack);
    }
}
