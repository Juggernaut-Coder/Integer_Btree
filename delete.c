#define DELETE_C
#include "abtree.h"

static inline BTreeNode *find_predecessor(BTreeNode *keyNode, size_t childIdx, delStack *stack) {
    BTreeNode *current = keyNode->links[childIdx]; 
    stack->arr[stack->sz].parent = keyNode;
    stack->arr[stack->sz++].childIdx = childIdx;
    while(isInternalNode(current)) {
        stack->arr[stack->sz].parent = current;
        stack->arr[stack->sz++].childIdx = current->keyCount;
        current = current->links[current->keyCount];
    }
    return current;
}

static inline BTreeNode *find_successor(BTreeNode *keyNode, BTreeNode **sucParent, size_t childIdx) {
    *sucParent = keyNode;
    BTreeNode *current = keyNode->links[childIdx];
    while(isInternalNode(current)) {
        *sucParent = current;
        current = current->links[0];
    }
    return current;
}

static inline void merge(BTreeNode *parent, size_t parentKeyIdx, BTreeNode *right, BTreeNode *left) {
    left->keys[left->keyCount++] = parent->keys[parentKeyIdx];
#if MIN_CHILDREN > 2
    void *ptr = parent->keys + parentKeyIdx; 
    memmove(ptr, ptr + 1, sizeof(parent->keys[0]) * (MIN_KEYS - 1));
    void *ptrLinks = parent->links + parentKeyIdx + 1; 
    memmove(ptrLinks, ptrLinks + 1, sizeof(parent->links[0] * (MIN_KEYS - 1)));
#endif
    memcpy(left->keys + left->keyCount, right->keys, sizeof(left->keys[0]) * right->keyCount);
    if(isInternalNode(left)) memcpy(left->links + left->keyCount, right->links, sizeof(left->links[0]) * (right->keyCount + 1));
    left->keyCount += right->keyCount;
    parent->keyCount--;
    free(right);
}

static inline void setVarsForMerge(BTreeNode **right, BTreeNode **left, size_t *idx, delStack *stack, size_t stackIdx) {
    if(stack->arr[stackIdx].childIdx == 0) {
        *idx = stack->arr[stackIdx].childIdx;
        *left = stack->arr[stackIdx].parent->links[*idx];
        *right = stack->arr[stackIdx].parent->links[*idx + 1];
    } else {
        *idx = stack->arr[stackIdx].childIdx - 1;
        *left = stack->arr[stackIdx].parent->links[*idx];
        *right = stack->arr[stackIdx].parent->links[stack->arr[stackIdx].childIdx];
    }
}

static inline void handleUnderflow(delStack *stack, BTree  *tree) {
    size_t parentIdxInStack = stack->sz - 1, leftCnt, rightCnt;
    bool right_redist = checkLeftForRedist(stack->arr[parentIdxInStack].parent, &rightCnt, stack->arr[parentIdxInStack].childIdx),
    left_redist = checkRightForRedist(stack->arr[parentIdxInStack].parent, &leftCnt, stack->arr[parentIdxInStack].childIdx);
    if(left_redist && (!right_redist || leftCnt <= rightCnt)) {
        size_t rightIdx = stack->arr[parentIdxInStack].childIdx + leftCnt, leftIdx;
        for(size_t i = 0; i < leftCnt; i++) {
            leftIdx = rightIdx-1;
            rotateLeft(stack->arr[parentIdxInStack].parent->links[rightIdx--], stack->arr[parentIdxInStack].parent->links[leftIdx], stack->arr[parentIdxInStack].parent, leftIdx);
        }
    } else if(right_redist) {
        size_t leftIdx = stack->arr[parentIdxInStack].childIdx - rightCnt;
        for(size_t i =0; i < rightCnt; i++) {
            rotateRight(stack->arr[parentIdxInStack].parent->links[leftIdx + 1], stack->arr[parentIdxInStack].parent->links[leftIdx], stack->arr[parentIdxInStack].parent, leftIdx);
            leftIdx--;
        }
    } else {
        BTreeNode *left, *right;
        size_t parentKeyIdx;
        setVarsForMerge(&right, &left, &parentKeyIdx, stack, parentIdxInStack);
        merge(stack->arr[parentIdxInStack].parent, parentKeyIdx, right, left);
        if(stack->arr[parentIdxInStack].parent->keyCount >= MIN_KEYS) goto ADJUST_BY_SHIFTING;
        while(true) {
            parentIdxInStack--;
            right_redist = checkLeftForRedist(stack->arr[parentIdxInStack].parent, &rightCnt, stack->arr[parentIdxInStack].childIdx),
            left_redist = checkRightForRedist(stack->arr[parentIdxInStack].parent, &leftCnt, stack->arr[parentIdxInStack].childIdx);
            if(left_redist && (!right_redist || leftCnt <= rightCnt)) {
                size_t rightIdx = stack->arr[parentIdxInStack].childIdx + leftCnt, leftIdx;
                for(size_t i = 0; i < leftCnt; i++) {
                    leftIdx = rightIdx - 1;
                    rotateLeftNonLeaf(stack->arr[parentIdxInStack].parent->links[rightIdx--], stack->arr[parentIdxInStack].parent->links[leftIdx], stack->arr[parentIdxInStack].parent, leftIdx);
                }
                return;
            } else if(right_redist) {
                size_t leftIdx = stack->arr[parentIdxInStack].childIdx - rightCnt;
                for(size_t i =0; i < rightCnt; i++) {
                    rotateRightNonLeaf(stack->arr[parentIdxInStack].parent->links[leftIdx + 1], stack->arr[parentIdxInStack].parent->links[leftIdx], stack->arr[parentIdxInStack].parent, leftIdx);
                     leftIdx--;
                }
                return;
            } else {
                setVarsForMerge(&right, &left, &parentKeyIdx, stack, parentIdxInStack);
                merge(stack->arr[parentIdxInStack].parent, parentKeyIdx, right, left);
                if(stack->arr[parentIdxInStack].parent->keyCount >= MIN_KEYS) goto ADJUST_BY_SHIFTING;
            }
            if(parentIdxInStack == 0) break;
        }
        if(tree->root->keyCount == 0) {
            BTreeNode* temp = tree->root;
            tree->root = tree->root->links[0];
            tree->height--;
            free(temp);
        }
    }
    return;
ADJUST_BY_SHIFTING:
    size_t keyIdx = (stack->arr[parentIdxInStack].childIdx == 0) ? 0 : stack->arr[parentIdxInStack].childIdx - 1;
    void *ptr = tree->root->keys + keyIdx;
    void *ptrLinks = tree->root->links + stack->arr[parentIdxInStack].childIdx;
    memmove(ptr, ptr + 1, sizeof(tree->root->keys[0]) * (tree->root->keyCount - keyIdx));
    memmove(ptrLinks, ptrLinks + 1, sizeof(tree->root->keys[0]) * (tree->root->keyCount - keyIdx));
}

static inline void deleteFromNonLeaf(BTreeNode *keyNode, size_t keyIdx, delStack *stack, BTree *tree) {
    BTreeNode *predecessor = find_predecessor(keyNode, keyIdx, stack);
    keyNode->keys[keyIdx] = predecessor->keys[predecessor->keyCount - 1];
    predecessor->keyCount--;
    if(predecessor->keyCount >= MIN_KEYS) return;
    handleUnderflow(stack, tree);
}

void delete(int key, BTree *tree) {
    delStack stack;
    size_t idx;
    BTreeNode *keyNode = search(key, tree, &idx, &stack);
    if(!keyNode) goto FREE;
    if(isInternalNode(keyNode)) {
        deleteFromNonLeaf(keyNode, idx, &stack, tree);    
    } else {
        deleteKeyFromNode(keyNode, idx);
        if(isInternalNode(tree->root)) {
            if(keyNode->keyCount >= MIN_KEYS) goto DECREMENT_TREE_COUNT;
            handleUnderflow(&stack, tree);
        }
    }
DECREMENT_TREE_COUNT:
    tree->size--;
FREE:
    if(stack.arr) free(stack.arr);
}