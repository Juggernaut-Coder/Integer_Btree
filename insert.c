#define INSERT_C
#include "abtree.h"

static inline BTreeNode *splitRoot(BTree *tree) {
#define oldRoot (tree->root)
    BTreeNode *newRoot = createNodeWithFirstKey(oldRoot->keys[SPLIT_KEY_INDEX - 1]);
    newRoot->links[1] = createRightSplitNode(oldRoot);
    newRoot->links[0] = oldRoot;
    tree->root = newRoot;
    tree->height++;
    return newRoot->links[1];
#undef oldRoot
}

static inline BTreeNode *splitChild(BTreeNode *parent, size_t splitLinkIdx) {
#define nodeToSplit parent->links[splitLinkIdx]
    BTreeNode *newRightChild = createRightSplitNode(nodeToSplit);
    /* Shift the children to right from splitIdx + 1 to make space for newRightChild */
    if(splitLinkIdx < parent->keyCount) {
        BTreeNode **ptrLink = parent->links + splitLinkIdx + 1;
        memmove(ptrLink + 1, ptrLink, sizeof(BTreeNode*) * (parent->keyCount - splitLinkIdx));
        /* Shift the keys to right from splitIdx to make space for child key */
        int *ptr = parent->keys + splitLinkIdx;
        memmove(ptr + 1, ptr, sizeof(parent->keys[0]) * (parent->keyCount - splitLinkIdx));
    }
    parent->links[splitLinkIdx + 1] = newRightChild;
    parent->keys[splitLinkIdx] = nodeToSplit->keys[SPLIT_KEY_INDEX - 1];
    parent->keyCount++;
    return newRightChild;
#undef nodeToSplit
}

static inline void insertOverflowLeft(int key, BTreeNode *node, BTreeNode *left, BTreeNode *parent, size_t parentKeyIdx, size_t idx) {
    left->keys[left->keyCount] = parent->keys[parentKeyIdx];
    left->keyCount++;
    if(idx == 0) {
        parent->keys[parentKeyIdx] = key;
    } else {
        parent->keys[parentKeyIdx] = node->keys[0];
        idx--;
        memmove(node->keys, node->keys + 1, sizeof(node->keys[0]) * idx);
        node->keys[idx] = key;
    }
}

static inline void insertOverflowRight(int key, BTreeNode *node, BTreeNode *right, BTreeNode *parent, size_t parentKeyIdx, size_t idx) {
    memmove(right->keys + 1, right->keys, sizeof(right->keys[0]) * right->keyCount);
    right->keys[0] = parent->keys[parentKeyIdx];
    right->keyCount++;
    if(idx == MAX_KEYS) {
        parent->keys[parentKeyIdx] = key;
    } else {
        parent->keys[parentKeyIdx] = node->keys[node->keyCount - 1];
        int *ptr = node->keys + idx;
        memmove(ptr + 1, ptr, sizeof(node->keys[0]) * (node->keyCount - idx - 1));
        node->keys[idx] = key;
    }
}

void insert(int key, BTree *tree) {
    size_t idx = 0;
    if(tree->root->keyCount == MAX_KEYS) { // root case
        splitRoot(tree);
        if(tree->root->keys[0] == key) return;
        if(key > tree->root->keys[0]) idx = 1;
    }else { 
        if(isKeyInNode(key,tree->root,&idx)) return;
        if(isLeafNode(tree->root)) { // else if root has no children insert in root
            insertKeyToNode(key, tree->root, &idx);
            goto INCREMENT_TREE_COUNT;
        }    
    }    
    BTreeNode *current = tree->root->links[idx], *parent = tree->root;
    while(isInternalNode(current)) {
        if(current->keyCount == MAX_KEYS) {
            splitChild(parent,idx);
            if(key == parent->keys[idx]) return;
            if(key > parent->keys[idx]) idx++;
            current = parent->links[idx];
        }else {
            if(isKeyInNode(key,current,&idx)) return;
            parent = current;
            current = current->links[idx];
        }
    }
    size_t insertIdx;
    if(isKeyInNode(key,current,&insertIdx)) return;
    if(current->keyCount == MAX_KEYS) {
        size_t left_count = SIZE_MAX, right_count = SIZE_MAX; 
        bool left_flag = checkLeftForRedist(parent, &left_count, idx), right_flag = checkRightForRedist(parent, &right_count, idx);
        if(left_flag && (!right_flag || left_count <= right_count)) {
            size_t leftIdx = idx - left_count - 1; 
            for(size_t iter = 0; iter < left_count; iter++, leftIdx++) {
                rotateLeft(parent->links[leftIdx+1], parent->links[leftIdx], parent, leftIdx);
            }
            insertOverflowLeft(key, current, parent->links[idx - 1], parent, idx -  1, insertIdx);
            goto INCREMENT_TREE_COUNT;
        } else if(right_flag) {
            size_t rightIdx = idx + right_count + 1, leftIdx;
            for(size_t iter = 0; iter < right_count; iter++, rightIdx--) {
                leftIdx = rightIdx - 1;
                rotateRight(parent->links[rightIdx],parent->links[leftIdx], parent, leftIdx);
            }
            insertOverflowRight(key, current, parent->links[idx + 1], parent, idx, insertIdx);
            goto INCREMENT_TREE_COUNT;
        } else {
            splitChild(parent,idx);
            if(key == parent->keys[idx]) return;
            if(key > parent->keys[idx]) idx++;
            current = parent->links[idx];
            if(insertIdx >= SPLIT_KEY_INDEX) insertIdx -= SPLIT_KEY_INDEX;
        }        
    } 
    insertKeyToNode(key, current, &insertIdx);
INCREMENT_TREE_COUNT:                                                                                          
    tree->size++;
}

static inline void handle_post_splits(size_t *indexes, size_t sz, BTreeNode *current,  BTreeNode **parent, BTree *tree) {
    BTreeNode *temp;
    if(!current) {
        BTreeNode *oldRoot = tree->root;
        temp = splitRoot(tree);
        if(indexes[0] < SPLIT_LINK_INDEX) {
            current =  oldRoot;
        } else {
            current = temp;
            indexes[0] -= SPLIT_LINK_INDEX; 
        }
    }
    
    temp = splitChild(current, indexes[i]);
    *parent = current;
    if(indexes[i + 1] >= SPLIT_LINK_INDEX) {
        current = temp;
        indexes[i + 1] -= SPLIT_LINK_INDEX;    
    } else {
        current = current->links[indexes[i]];
    }
      
}

void insert_postSplit(int key, BTree *tree) {
    size_t idx = 0;
    BTreeNode *current = tree->root;
    if(isLeafNode(tree->root)) {
        if(tree->root->keyCount == MAX_KEYS) {
            splitRoot(tree);
            if(tree->root->keys[0] == key) return;
            if(key > tree->root->keys[0]) idx = 1;
            current = tree->root->links[idx];
        }   
        if(isKeyInNode(key,current,&idx)) return;
        insertKeyToNode(key, current, &idx);
        tree->size++;
        return;
    }
    const size_t indexes_cap = tree->height + 1; // +1 to accomodate insert index
    size_t *indexes = malloc(sizeof(size_t) * indexes_cap);
    if(!indexes) fprintf(stderr, "Indexes memory allocation failed"); 
    size_t indexes_sz = 0;
    BTreeNode *parent, *lastNonfullNode = NULL;
    do {
        if(isKeyInNode(key,current,&idx)) goto FREE;
        if(current->keyCount < MAX_KEYS) {
            lastNonfullNode = current;
            indexes_sz = 0;    
        }
        indexes[indexes_sz++] = idx;
        parent = current;
        current = current->links[idx];
    } while(isInternalNode(current));
    if(isKeyInNode(key,current,&indexes[indexes_sz])) goto FREE;
    if(current->keyCount == MAX_KEYS) {
        size_t left_count = SIZE_MAX, right_count = SIZE_MAX; 
        bool left_flag = checkLeftForRedist(parent, &left_count, idx), right_flag = checkRightForRedist(parent, &right_count, idx);
        if(left_flag && (!right_flag || left_count <= right_count)) {
            size_t leftIdx = idx - left_count - 1; 
            for(size_t iter = 0; iter < left_count; iter++, leftIdx++) {
                rotateLeft(parent->links[leftIdx+1], parent->links[leftIdx], parent, leftIdx);
            }
            insertOverflowLeft(key, current, parent->links[idx - 1], parent, idx -  1, indexes[indexes_sz]);
            goto INCREMENT_TREE_COUNT;
        } else if(right_flag) {
            size_t rightIdx = idx + right_count + 1, leftIdx;
            for(size_t iter = 0; iter < right_count; iter++, rightIdx--) {
                leftIdx = rightIdx - 1;
                rotateRight(parent->links[rightIdx],parent->links[leftIdx], parent, leftIdx);
            }
            insertOverflowRight(key, current, parent->links[idx + 1], parent, idx, indexes[indexes_sz]);
            goto INCREMENT_TREE_COUNT;
        } else {
            handle_post_splits(indexes, indexes_sz, lastNonfullNode, &parent, tree);
            if(key == parent->keys[indexes[indexes_sz - 1]]) goto FREE;
            if(key > parent->keys[indexes[indexes_sz - 1]]) indexes[indexes_sz-1]++;       
            current = parent->links[indexes[indexes_sz - 1]];
        }        
    } 
    insertKeyToNode(key, current, &indexes[indexes_sz]);
INCREMENT_TREE_COUNT:
    tree->size++;
FREE:
    free(indexes);
}