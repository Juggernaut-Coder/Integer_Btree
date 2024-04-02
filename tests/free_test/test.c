#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINKS 10
#define MAX_KEYS (MAX_LINKS - 1)

typedef struct BTreeNode {
    struct BTreeNode *links[MAX_LINKS];
    int keys[MAX_KEYS];
    size_t keyCount;
} BTreeNode;

static inline BTreeNode *_createNode() {
    BTreeNode *newNode = (BTreeNode *)malloc(sizeof(BTreeNode));
    if (!newNode) {
        fprintf(stderr, "New node init failed: out of memory\n");
        exit(EXIT_FAILURE);
    }
    return newNode;
}

BTreeNode *createEmptyNode() {
    BTreeNode *newNode = _createNode();
    memset(newNode->links, 0, sizeof(BTreeNode *) * MAX_LINKS);
    newNode->keyCount = 0;
    return newNode;
}

int main(void) {
    BTreeNode *node = createEmptyNode();
    printf("Empty key count: %zu\n", node->keyCount);
    free(node);
    return 0;
}