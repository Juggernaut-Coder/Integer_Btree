#include "abtree.h"

static inline void __printBTree(BTreeNode* node, int level, int parentIndex, int childIndex) {
    // Print indentation based on the level
    for (int i = 0; i < level; i++) printf("     ");
    // Print the node's keys
    if (level > 0) { // For non-root nodes
        printf("Child %d.%d keys: ", parentIndex, childIndex);
    } else { // For the root node
        printf("Root keys: ");
    }
    for (int i = 0; i < node->keyCount; i++) {
        printf("%d ", node->keys[i]);
    }
    printf("\n");
    // Recursively print child nodes
    if(isInternalNode(node)) {
        for (int i = 0; i <= node->keyCount; i++) {
            __printBTree(node->links[i], level + 1, childIndex, i);
        }
    }
}

void printBTree(BTreeNode* root) {
    __printBTree(root, 0, -1, 0);
}