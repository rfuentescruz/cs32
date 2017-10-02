#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char c;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct Stack {
    char data[53];
    int head;
} Stack;

char pop(Stack *);
char peek(Stack *);
Node *process(Stack *, char *, int, int);

void destroy(Node *);
void postorder(Node *);

int main(int argc, char *argv[]) {
    int num_cases = 0;
    scanf("%d\n", &num_cases);
    Stack *preorder = malloc(sizeof(Stack));
    char inorder[53];
    Node *root = NULL;
    for (int i = 0; i < num_cases; i++) {
        scanf("%s %s", preorder->data, inorder);
        preorder->head = 0;
        root = process(preorder, inorder, 0, strlen(inorder));
        postorder(root);
        printf("\n");
        destroy(root);
    }

    free(preorder);
}

Node *process(Stack *preorder, char *inorder, int l, int r) {
    char root = peek(preorder);
    Node *n = NULL;
    for (int i = l; i < r; i++) {
        if (inorder[i] == root) {
            n = malloc(sizeof(Node));
            n->c = pop(preorder);
            n->left = process(preorder, inorder, l, i);
            n->right = process(preorder, inorder, i + 1, r);
            # if DEBUG
                if (n->left) {
                    printf("%c <- ", n->left->c);
                }
                printf("%c", n->c);
                if (n->right) {
                    printf(" -> %c", n->right->c);
                }
                printf("\n");
            # endif
        }
    }
    return n;
}

void postorder(Node *root) {
    if (root->left != NULL) {
        postorder(root->left);
    }

    if (root->right != NULL) {
        postorder(root->right);
    }
    printf("%c", root->c);
}

char pop(Stack *stack) {
    char c = stack->data[stack->head];
    stack->head++;
    return c;
}

char peek(Stack *stack) {
    return stack->data[stack->head];
}

void destroy(Node *root) {
    if (root->left != NULL) {
        destroy(root->left);
    }
    if (root->right != NULL) {
        destroy(root->right);
    }
    free(root);
}
