#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIGIT_LENGTH 9

typedef struct OperatorNode {
    char data;
    struct OperatorNode *next;
} OperatorNode;

typedef struct N {
    double value;
    struct N *next;
} N;

typedef struct OperatorStack {
    OperatorNode *head;
} OperatorStack;

typedef struct Numbers {
    N *head;
} Numbers;

void push(OperatorStack *, char);
char pop(OperatorStack *);
char peek(OperatorStack *);
void empty(OperatorStack *);

void push_n(Numbers *, double);
double pop_n(Numbers *);
void empty_n(Numbers *);

int icp(char);
int isp(char);

void evaluate(Numbers *, char);

int main(int argc, char *argv[]) {
    char c;
    OperatorStack *operators = malloc(sizeof(OperatorStack));
    operators->head = NULL;

    Numbers *numbers = malloc(sizeof(Numbers));
    numbers->head = NULL;

    char *digits = malloc(DIGIT_LENGTH + 1);
    char *d = digits;

    int num_cases = 0;
    scanf("%d\n", &num_cases);

    while ((c = getchar()) != EOF) {
        if (operators->head == NULL) {
            push(operators, '(');
        }

        if (isdigit(c) || c == '.') {
            *d = c;
            d++;
        } else {
            *d = '\0';
            double n = 0;

            // We parsed a digit!
            if (d != digits) {
                n = atof(digits);
                // Reset the digit pointer
                d = digits;
                #if POSTFIX
                    printf("{%f}", n);
                #endif
                push_n(numbers, n);
            }

            if (strchr("+-*/^()\n", c) != NULL) {
                char o;
                if (c == '(') {
                    push(operators, c);
                } else if (c == ')' || c == '\n') {
                    while ((o = pop(operators)) != '(') {
                        #if POSTFIX
                            printf("%c", o);
                        #endif
                        evaluate(numbers, o);
                    }
                } else {
                    while (icp(c) < isp(peek(operators))) {
                        o = pop(operators);
                        #if POSTFIX
                            printf("%c", o);
                        #endif
                        evaluate(numbers, o);
                    }
                    push(operators, c);
                }

                if (c == '\n') {
                    double result = pop_n(numbers);
                    if (result != EOF) {
                        printf("%.4f\n", result);
                    }
                    empty_n(numbers);
                    empty(operators);
                }
            }
        }
    }

    free(digits);

    empty_n(numbers);
    free(numbers);

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

void evaluate(Numbers *n, char op) {
    double b = pop_n(n);
    double a = pop_n(n);
    if (a == EOF || b == EOF) {
        return;
    }

    if (op == '+') {
        push_n(n, a + b);
        #if DEBUG
            printf("%f %c %f = %f\n", a, op, b, a + b);
        #endif
    } else if (op == '-') {
        push_n(n, a - b);
        #if DEBUG
            printf("%f %c %f = %f\n", a, op, b, a - b);
        #endif
    } else if (op == '*') {
        push_n(n, a * b);
        #if DEBUG
            printf("%f %c %f = %f\n", a, op, b, a * b);
        #endif
    } else if (op == '/') {
        push_n(n, a / b);
        #if DEBUG
            printf("%f %c %f = %f\n", a, op, b, a / b);
        #endif
    } else if (op == '^') {
        push_n(n, pow(a, b));
        #if DEBUG
            printf("%f %c %f = %f\n", a, op, b, pow(a, b));
        #endif
    }
}

void push(OperatorStack *stack, char c) {
    OperatorNode *node = malloc(sizeof(OperatorNode));

    node->data = c;
    node->next = stack->head;

    stack->head = node;
}

char pop(OperatorStack *stack) {
    char c;
    if (stack->head == NULL) {
        return EOF;
    }

    OperatorNode *tmp = stack->head;
    stack->head = stack->head->next;
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

void push_n(Numbers *stack, double n) {
    N *node = malloc(sizeof(N));

    node->value = n;
    node->next = stack->head;

    stack->head = node;
}

double pop_n(Numbers *stack) {
    double n;
    if (stack->head == NULL) {
        return EOF;
    }

    N *tmp = stack->head;
    stack->head = stack->head->next;
    n = tmp->value;
    free(tmp);
    return n;
}

void empty_n(Numbers *stack) {
    while (stack->head != NULL) {
        pop_n(stack);
    }
}
