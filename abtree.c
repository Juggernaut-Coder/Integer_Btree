#include "abtree.h"
_Static_assert(((MAX_KEYS - MIN_KEYS) > MIN_KEYS), "Violates BTree Properties of MIN_KEY");

inline size_t childrenNum(BTreeNode *node) {
    if(isInternalNode(node)) return node->keyCount + 1;
    return 0;
}

/* START: NODE FUNCTIONS */
static inline BTreeNode *_createNode(void) {
    BTreeNode *newNode = (BTreeNode *)malloc(sizeof(BTreeNode));
    if (!newNode) {
        fprintf(stderr, "New node init failed: out of memory\n");
        exit(EXIT_FAILURE);
    }
    return newNode;
}

BTreeNode *createEmptyNode(void) {
    BTreeNode *newNode = _createNode();
    memset(newNode->links, 0, sizeof(BTreeNode*) * MAX_LINKS);
    newNode->keyCount = 0;
    return newNode;
}

BTreeNode *createNodeWithFirstKey(int key) {
    BTreeNode *newNode = _createNode();
    memset(newNode->links, 0, sizeof(BTreeNode*) * MAX_LINKS);
    newNode->keys[0] = key;
    newNode->keyCount = 1;
    return newNode;
}

BTreeNode *createRightSplitNode(BTreeNode *nodeToSplit) {
    BTreeNode *newNode = _createNode();
    memcpy(newNode->keys, nodeToSplit->keys + SPLIT_KEY_INDEX, sizeof(newNode->keys[0]) * SPLIT_KEYS);
    memcpy(newNode->links, nodeToSplit->links + SPLIT_LINK_INDEX, sizeof(BTreeNode*) * SPLIT_LINKS);
    memset(newNode->links + SPLIT_LINKS, 0, sizeof(BTreeNode*) * (MAX_CHILDREN - SPLIT_LINKS));
    memset(nodeToSplit->links + SPLIT_LINK_INDEX, 0, sizeof(BTreeNode*) * SPLIT_LINKS);
    /* Delete copied keys from Node to split by adjusting key count*/
    nodeToSplit->keyCount = MIN_KEYS;
    /* Set the key count */
    newNode->keyCount = SPLIT_KEYS;
    return newNode;
}

void destroyNode(BTreeNode **pNode) {
    if(*pNode) {
        if(isInternalNode(*pNode)) {
            for(size_t i = 0; i <= (*pNode)->keyCount; i++) {
                destroyNode(&(*pNode)->links[i]);
            }
        }
        free(*pNode);
        *pNode = NULL;
    }
}

void insertKeyToNode(int key, BTreeNode *node, size_t *idx) { 
    int *ptr = node->keys + *idx;
    memmove(ptr + 1, ptr, sizeof(node->keys[0]) * (node->keyCount - *idx));
    node->keys[*idx] = key;
    node->keyCount++;
}

void deleteKeyFromNode(BTreeNode *node, size_t idx) {
    int *ptr = node->keys + idx; 
    memmove(ptr, ptr + 1, sizeof(node->keys[0]) * (node->keyCount - idx - 1));
    node->keyCount--;
}

bool inline isKeyInNode(int key, BTreeNode *current, size_t *idx) {
#ifdef SQ_SEARCH
    return sq_isValInArray(key, current->keys, 0, current->keyCount, idx);
#elif defined(IBS_SEARCH)
    return ibs_isValInArray(key, current->keys, 0, current->keyCount, idx);
#else
  #error "No search method defined"
#endif
}
/* END: NODE FUNCTIONS */

BTree *initBTree(void) {
    BTree *tree = (BTree*)malloc(sizeof(BTree));
    if(!tree) {
        fprintf(stderr,"Failed to allocate memory for tree: out of memory\n");
        exit(EXIT_FAILURE);
    }
    tree->root = createEmptyNode();
    tree->size = 0;
    tree->height = 0;
    return tree;
}

void destroyBTree(BTree **pTree) {
    destroyNode(&(*pTree)->root);
    free(*pTree);
    (*pTree) = NULL;
}
