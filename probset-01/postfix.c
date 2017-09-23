#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct OperatorNode {
    char data;
    struct OperatorNode *next;
} OperatorNode;

typedef struct OperatorStack {
    int n;
    OperatorNode *head;
} OperatorStack;

void push(OperatorStack *, char);
char pop(OperatorStack *);
char peek(OperatorStack *);
void empty(OperatorStack *);

int icp(char);
int isp(char);

int main(int argc, char *argv[]) {
    char c;
    OperatorStack *operators = malloc(sizeof(OperatorStack));
    operators->head = NULL;
    operators->n = 0;

    while ((c = getchar()) != EOF) {
        if (operators->head == NULL) {
            push(operators, '(');
        }
        if (strchr("+-*/^()\n", c) != NULL) {
            char o;
            if (c == '(') {
                push(operators, c);
            } else if (c == ')' || c == '\n') {
                while ((o = pop(operators)) != '(') {
                    printf("%c", o);
                }
            } else {
                while (icp(c) < isp(peek(operators))) {
                    o = pop(operators);
                    printf("%c", o);
                }
                push(operators, c);
            }
            if (c == '\n') {
                empty(operators);
                printf("%c=======\n", c);
            }
        } else if (isalnum(c)) {
            printf("%c", c);
        }
    }

    empty(operators);
    free(operators);

    // We cool? We cool.
    return 0;
}

int icp(char operator) {
    switch (operator) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 3;
        case '^':
            return 6;
        default:
            return -1;
    }
}

int isp(char operator) {
    switch (operator) {
        case '+':
        case '-':
            return 2;
        case '*':
        case '/':
            return 4;
        case '^':
            return 5;
        case '(':
            return 0;
        default:
            return -1;
    }
}

void push(OperatorStack *stack, char c) {
    OperatorNode *node = malloc(sizeof(OperatorNode));

    node->data = c;
    node->next = stack->head;
    stack->n++;
    printf("{%d}\n", stack->n);
    stack->head = node;
}

char pop(OperatorStack *stack) {
    char c;
    if (stack->head == NULL) {
        return EOF;
    }
    
    OperatorNode *tmp = stack->head;
    stack->head = stack->head->next;
    stack->n--;
    c = tmp->data;
    free(tmp);
    return c;
}

char peek(OperatorStack *stack) {
    if (stack->head == NULL) {
        return EOF;
    }

    return stack->head->data;
}

void empty(OperatorStack *stack) {
    while (stack->head != NULL) {
        pop(stack);
    }
}
