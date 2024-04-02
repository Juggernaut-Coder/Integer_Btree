#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
//#include <pthreads.h>
#include "array_search.h"

enum TreeProperties {
#define MIN_CHILDREN 2 // except root if MIN_CHILDREN > 2
#define MAX_CHILDREN 4
    MAX_KEYS = MAX_CHILDREN - 1,
    MIN_KEYS = MIN_CHILDREN - 1, // except root
    SPLIT_KEYS = MAX_KEYS - MIN_CHILDREN,
    SPLIT_KEY_INDEX = MIN_CHILDREN,
    SPLIT_LINKS = SPLIT_KEYS + 1,
    SPLIT_LINK_INDEX = SPLIT_KEY_INDEX, 
    MAX_LINKS = MAX_CHILDREN
};

typedef struct BTreeNode { 
    struct BTreeNode *links[MAX_LINKS]; 
    int keys[MAX_KEYS];
    size_t keyCount;
}BTreeNode;

typedef struct BTree {
    BTreeNode *root;  
    size_t size;
    size_t height;
}BTree;

#define isInternalNode(x) ((x)->links[0])
#define isLeafNode(x) !((x)->links[0])

/* Function Prototype of BTreeNode */
size_t childrenNum(BTreeNode *node);
BTreeNode *createEmptyNode(void);
BTreeNode *createNodeWithFirstKey(int key);
BTreeNode *createRightSplitNode(BTreeNode *nodeToSplit);
void destroyNode(BTreeNode **pNode);
void insertKeyToNode(int key, BTreeNode *node, size_t *idx);
void deleteKeyFromNode(BTreeNode *node, size_t idx);
// Define the search type for isKeyInNode it can be IBS_SEARCH or SQ_SEARCH 
//#define IBS_SEARCH
#define SQ_SEARCH
bool isKeyInNode(int key, BTreeNode *current, size_t *idx);

/* Function Prototype of BTree */
BTree *initBTree(void);
void destroyBTree(BTree **pTree);


/* insert */
void insert(int key, BTree *tree);
void insert_postSplit(int key, BTree *tree);


/* delete */
void delete(int key, BTree *tree);

/* print for debugging / visualization */
void printBTree(BTreeNode* root);


#ifdef DELETE_C
typedef struct delStackElem {
    BTreeNode *parent;
    size_t childIdx;
}delStackElem;

typedef struct delStack {
    delStackElem *arr;
    size_t sz;
}delStack;
#endif 

/* BTree Search */

#ifndef DELETE_C
static inline BTreeNode *search (int key, BTree *tree, size_t *idx) 
#else
static inline BTreeNode *search (int key, BTree *tree, size_t *idx, delStack *stack) 
#endif
{
#ifdef DELETE_C
    stack->arr = NULL;
    stack->sz = 0;
#endif
    if(isKeyInNode(key, tree->root, idx)) return tree->root;
#ifdef DELETE_C
    stack->arr = malloc(sizeof(delStack) * (tree->height + 1));
    if(!stack->arr) {
        fprintf(stderr, "Not Enough memory to record search path");
        exit(EXIT_FAILURE);
    }
    stack->arr[stack->sz].parent = tree->root;
    stack->arr[stack->sz++].childIdx = *idx;
#endif
    BTreeNode *current = tree->root->links[*idx];
    while(current) {
        if(isKeyInNode(key, current, idx)) return current;
#ifdef DELETE_C
        stack->arr[stack->sz].parent = current;
        stack->arr[stack->sz++].childIdx = *idx;
#endif
        current = current->links[*idx];
    }
    return NULL;
}

/* Util Functions */

#if defined(INSERT_C) || defined(DELETE_C)

static inline void rotateRight(BTreeNode *right, BTreeNode *left, BTreeNode *parent, size_t parentKeyIdx) {
    memmove(right->keys + 1, right->keys, sizeof(right->keys[0]) * right->keyCount);
    right->keys[0] = parent->keys[parentKeyIdx];
    right->keyCount++;
    left->keyCount--;
    parent->keys[parentKeyIdx] = left->keys[left->keyCount];
}

static inline void rotateLeft(BTreeNode *right, BTreeNode *left, BTreeNode *parent, size_t parentKeyIdx) {
    left->keys[left->keyCount] = parent->keys[parentKeyIdx];
    left->keyCount++;
    parent->keys[parentKeyIdx] = right->keys[0];
    right->keyCount--;
    memmove(right->keys, right->keys + 1, sizeof(right->keys[0]) * (right->keyCount));
}

static inline void rotateLeftNonLeaf(BTreeNode *right, BTreeNode *left, BTreeNode *parent, size_t parentKeyIdx) {
    rotateLeft(right, left, parent, parentKeyIdx);
    left->links[left->keyCount] = right->links[0];
    const size_t linkCnt = right->keyCount + 1;
    memmove(right->links, right->links + 1, sizeof(right->links[0]) * (linkCnt));
    right->links[linkCnt] = NULL;
}

static inline void rotateRightNonLeaf(BTreeNode *right, BTreeNode *left, BTreeNode *parent, size_t parentKeyIdx) {
    rotateRight(right, left, parent, parentKeyIdx);
    memmove(right->links + 1, right->links, sizeof(right->links[0]) * right->keyCount);
    const size_t linkCnt = left->keyCount + 1;
    right->links[0] = left->links[linkCnt];
    left->links[linkCnt] = NULL;
}

static inline bool checkLeftForRedist(BTreeNode *parent, size_t *count, size_t nodeIdx) {
#ifdef DELETE_C
    *count = 1;
#elif defined(INSERT_C)
    *count = 0;
#endif
    if(nodeIdx > 0) {
        size_t i = nodeIdx - 1;
        while(true) {
            if(
#ifdef DELETE_C
            parent->links[i]->keyCount > MIN_KEYS
#elif defined (INSERT_C)
            parent->links[i]->keyCount < MAX_KEYS
#endif      
            ) {
                return true;
            }
            (*count)++;
            if(i == 0) break;
            i--;
        }
    }
    return false;
}

static inline bool checkRightForRedist(BTreeNode *parent, size_t *count, size_t nodeIdx) {
#ifdef DELETE_C
    *count = 1;
#elif defined(INSERT_C)
    *count = 0;
#endif
    if(nodeIdx < parent->keyCount) {
        size_t i = nodeIdx + 1;
        while(true) {
            if(
#ifdef DELETE_C
            parent->links[i]->keyCount > MIN_KEYS
#elif defined(INSERT_C)
            parent->links[i]->keyCount < MAX_KEYS
#endif
            ) {
                return true;
            }
            (*count)++;
            if(i == parent->keyCount) break;
            i++;
        }    
    }
    return false;
}

#endif
