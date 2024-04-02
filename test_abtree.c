#define INSERT_C
#include "abtree.h"
#include "array_search.h"
#include <assert.h>

#define TEST_4

#ifdef TEST_1 /* This test checks if the interpolation binary and sq search work as intended! */

#include <time.h>

int main(void) {
    
    int array1[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55};
    int array2[] = {1,10,15,30,400,401,402,600,620,640,650,700,701,702,705,2000,2005,3000,3200,3400,3500,3600,6000,6200,6500,6700,6800,6801,6803,8000,9001,9010,9100,9300,9500,9601,9602,9802,9900};
    int array3[] = {343, 1540, 1562, 2135, 2275, 2559, 3780, 4440, 4946, 6110, 6200, 7040, 7290, 7541, 7881, 9269};
    printf(
        "Arrays available:\n"
        "1)array1[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55}\n"
        "2)array2[] = {1,10,15,30,400,401,402,600,620,640,650,700,701,702,705,2000,2005,3000,3200,3400,3500,3600,6000,6200,6500,6700,6800,6801,6803,8000,9001,9010,9100,9300,9500,9601,9602,9802,9900}\n"
        "3)array3[] = {343, 1540, 1562, 2135, 2275, 2559, 3780, 4440, 4946, 6110, 6200, 7040, 7290, 7541, 7881, 9269};\n"
    );
    
    int number;
    clock_t start, end;
    double cpu_time_used;
    size_t array1_len = sizeof(array1) / sizeof(array1[0]);
    size_t array2_len = sizeof(array2) / sizeof(array2[0]);
    size_t array3_len = sizeof(array3) / sizeof(array3[0]);
    size_t idx;
    // ibsearch
    while(1) {
        printf("Enter array number:");
        scanf("%d", &number);
        int *p;
        size_t len;
        switch (number)
        {
        case 1:
            p = array1;
            len = array1_len;
            break;
        case 2:
            p = array2;
            len = array2_len;
            break;
        case 3:
            p = array3;
            len = array3_len;
            break;
        default:
            p = NULL;
            len = 0;
            exit(0);
            break;
        }
        printf("Enter a key to find in array %d: ", number);
        scanf("%d", &number);
        printf("You entered: %d\n", number);

        start = clock();
        if(ibs_isValInArray(number, p, 0, len, &idx)) {
            printf("Found at idx: ");
        } else {
            printf("Not found. idx is at :");
        }
        printf("%zu\n", idx);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("Time taken for IBS in array: %f seconds\n", cpu_time_used);
    }

    return 0;
}
#endif

#ifdef TEST_2 /* This test checks if the node creation and destruction functions work as intended! */
#include <assert.h>
int main(void) {
    BTreeNode *node = createNodeWithFirstKey(7);
    printf("First key: %d, key count: %zu\n", node->keys[0], node->keyCount);
    destroyNode(&node);
    
    // Create an empty node and populate it with keys to reach MAX_KEYS
    node = createEmptyNode();
    int _key = 50;
    for (size_t i = 0; i < MAX_KEYS; i++) {
        node->keys[i] = _key;
        _key = _key + 50;
        node->keyCount++;
        printf("%zu)key: %d\n", i, node->keys[i]);
    }
    printf("key count: %zu\n", node->keyCount);

    // Add child nodes, each filled with MIN_KEYS number of keys
    srand(50);
    for (size_t j = 0; j <= MAX_KEYS; j++) {  // <= MAX_KEYS for MAX_CHILDREN
        node->links[j] = createEmptyNode();  // Assuming createEmptyNode initializes keyCount to 0
        for (size_t k = 0; k < MIN_KEYS; k++) {
            int lower_bound = (j == 0 ?  0 : node->keys[j - 1] + 1);  // Lower bound is the previous key + 1
            int upper_bound = (j == node->keyCount ? node->keys[MAX_KEYS - 1] + 200 : node->keys[j]);  // Upper bound is the current key
            node->links[j]->keys[k] = lower_bound + (rand() % (upper_bound - lower_bound));  // Generate random key within range
            node->links[j]->keyCount++;
        }
    }

    printf("Child Node Keys Before Right Split:\n");
    for (size_t j = 0; j <= MAX_KEYS; j++) {
        printf("Child Node %zu Keys: ", j);
        for (size_t k = 0; k < MIN_KEYS; k++) {
            printf("%d ", node->links[j]->keys[k]);
        }
        printf("\n");
    }

    BTreeNode *RS_node = createRightSplitNode(node);
    for(size_t j = 0; j < node->keyCount; j++) {
        printf("%zu)LS_key: %d\n", j, node->keys[j]);
    }
    printf("LS_key count: %zu\n", node->keyCount);
    
    printf("LS_node Child Node Keys After Right Split:\n");
    for (size_t j = 0; j < node->keyCount + 1; j++) {
        printf("Child Node %zu Keys: ", j);
        for (size_t k = 0; k < node->links[j]->keyCount; k++) {
            printf("%d ", node->links[j]->keys[k]);
        }
        printf("\n");
    }

    for(size_t j = 0; j < RS_node->keyCount; j++) {
        printf("%zu)RS_key: %d\n", j, RS_node->keys[j]);
    }
    printf("RS_key count: %zu\n", node->keyCount);
    
    printf("RS_node Child Node Keys After Right Split:\n");
    for (size_t j = 0; j < RS_node->keyCount + 1; j++) {
        printf("Child Node %zu Keys: ", j);
        for (size_t k = 0; k < RS_node->links[j]->keyCount; k++) {
            printf("%d ", RS_node->links[j]->keys[k]);
        }
        printf("\n");
    }

    destroyNode(&node);
    return 0;
}

#endif

#ifdef TEST_3 /* This test checks if split root/child, search, insertoverflowleft, insertoverflowright  is working as intended */
#define KEY_SPACE 50
int main(void) {
    // Create an empty node and populate it with keys to reach MAX_KEYS
    BTreeNode *node = createEmptyNode();
    int _key = 50;
    for (size_t i = 0; i < MAX_KEYS; i++) {
        node->keys[i] = _key;
        _key = _key + KEY_SPACE;
        node->keyCount++;
        printf("%zu)key: %d\n", i, node->keys[i]);
    }
    printf("key count: %zu\n", node->keyCount);

    // Add child nodes, each filled with MIN_KEYS number of keys
    srand(50);
    for (size_t j = 0; j <= MAX_KEYS; j++) {  // <= MAX_KEYS for MAX_CHILDREN
        node->links[j] = createEmptyNode();  // Assuming createEmptyNode initializes keyCount to 0
        for (size_t k = 0; k < MIN_KEYS; k++) {
            int lower_bound = (j == 0 ?  0 : node->keys[j - 1] + 1);  // Lower bound is the previous key + 1
            int upper_bound = (j == node->keyCount ? node->keys[MAX_KEYS - 1] + 200 : node->keys[j]);  // Upper bound is the current key
            node->links[j]->keys[k] = lower_bound + (rand() % (upper_bound - lower_bound));  // Generate random key within range
            node->links[j]->keyCount++;
        }
    }

    printf("Child Node Keys Before Right Split:\n");
    for (size_t j = 0; j <= MAX_KEYS; j++) {
        printf("Child Node %zu Keys: ", j);
        for (size_t k = 0; k < MIN_KEYS; k++) {
            printf("%d ", node->links[j]->keys[k]);
        }
        printf("\n");
    }

    splitRoot(&node);

    BTreeNode *leafToFill = node->links[0]->links[0];
    assert(leafToFill);
    int low_bound = leafToFill->keys[leafToFill->keyCount - 1] + 1;
    int up_bound = node->links[0]->keys[0] - 1;
    for (size_t i = leafToFill->keyCount; i < MAX_KEYS; i++) {
        _key = low_bound + (rand() % (up_bound - low_bound)); // Ensure _key is within bounds
        leafToFill->keys[i] = _key;
        leafToFill->keyCount++;
        low_bound = _key + 1;
    }

    printf("\nAfter split n fill:\n");
    printf("New root key: %d\n", node->keys[0]);
    printf("Left child keys: ");
    for (size_t i = 0; i < node->links[0]->keyCount; i++) {  // Assuming MIN_KEYS are correctly defined
        printf("%d ", node->links[0]->keys[i]);
    }

    printf("\nRight child keys: ");
    for (size_t i = 0; i < node->links[1]->keyCount; i++) {  // Assuming this reflects the correct structure after split
        printf("%d ", node->links[1]->keys[i]);
    }
    printf("\n");

    printf("LS_node_child Child Node Keys After Right Split:\n");
    for (size_t j = 0; j < node->links[0]->keyCount + 1; j++) {
        printf("Child Node %zu Keys: ", j);
        for (size_t k = 0; k < node->links[0]->links[j]->keyCount; k++) {
            printf("%d ", node->links[0]->links[j]->keys[k]);
        }
        printf("\n");
    }

    printf("RS_node_child Child Node Keys After Right Split:\n");
    for (size_t j = 0; j < node->links[1]->keyCount + 1; j++) {
        printf("Child Node %zu Keys: ", j);
        for (size_t k = 0; k < node->links[1]->links[j]->keyCount; k++) {
            printf("%d ", node->links[1]->links[j]->keys[k]);
        }
        printf("\n");
    }

    splitChild(node->links[0], 0);

    node->links[0]->links[1]->keys[1] = 23;
    node->links[0]->links[1]->keys[2] = 30;
    node->links[0]->links[1]->keyCount += 2;
    // Printing children of the root
    printBTree(node);
    insertOverflowLeft(21, node->links[0]->links[1], node->links[0]->links[0], node->links[0], 0);
    printf("---------IOL------------------\n");
    printBTree(node);
    insertOverflowLeft(40, node->links[0]->links[1], node->links[0]->links[0], node->links[0], 0);
    printf("------------IOL2---------------\n");
    printBTree(node);
    insertOverflowRight(52, node->links[0]->links[1], node->links[0]->links[2], node->links[0], 1);
    printf("------------IOR---------------\n");
    printBTree(node);
    insertOverflowRight(35, node->links[0]->links[1], node->links[0]->links[2], node->links[0], 1);
    printf("------------IOR2---------------\n");
    printBTree(node);
    printf("test search function\n");
    size_t idx = 0;
    int keys_canbe_found[] = {100, 50, 150, 1, 16, 22, 77, 117, 235};
    int keys_cantbe_found[] = {200, 30, 300, 2, 15, 25, 66, 223, 249};
    BTreeNode *parent;
    if(search(12, node, &parent, &idx)) {
        printf("FOUND");
    } else {
        printf("NOT FOUND");
    }
    return 0;
}
#endif

#ifdef TEST_4 /* insert & delete function */
int main(void) {
    BTree *tree = initBTree();
    BTree *tree2 = initBTree();
    const int low_bound = 1;
    const int up_bound = 400;
    srand(10);
    int key;
#define SZ 20
    int keys[SZ];
    printf("keys: ");
    for(size_t i = 0; i < SZ; i++){
        keys[i] = low_bound + rand() % (up_bound - low_bound);
        printf("%d, ", keys[i]);
    }
    printf("\n");
    for(size_t i = 0; i < SZ; i++) {
        insert(keys[i], tree2);
        insert_postSplit(keys[i], tree);
    }
    printf("\n----------PRE---------\n");
    printBTree(tree2->root);
    printf("\n----------POST---------\n");
    printBTree(tree->root);

    delete(304,tree);
    delete(385,tree);
    delete(388,tree);
    delete(364,tree);
    delete(342, tree);
    delete(339, tree);
    delete(326, tree);
    delete(213, tree);
    delete(219, tree);
    delete(316, tree);
    delete(72, tree);
    delete(81,  tree);
    delete(156, tree);
    printf("\n----------delete delete post---------\n");
    printBTree(tree->root);
    delete(309, tree);
    printf("\n----------After delete post---------\n");
    printBTree(tree->root);
    // BTreeNode *parent;
    // size_t idx;
    // bool b = true;
    // for(size_t i = 0; i < SZ; i++) {
    //     if(!search(keys[i], tree->root, &parent, &idx)) {
    //         b = false;
    //         break;
    //     }
    // }
    // if(b) printf("FOUND ALL KEYS in tree\n");
    // b = true;
    // for(size_t i = 0; i < SZ; i++) {
    //     if(!search(keys[i], tree2->root, &parent, &idx)) {
    //         b = false;
    //         break;
    //     }
    // }
    // if(b) printf("FOUND ALL KEYS in tree 2\n");
    // delete(21, tree);
    // delete(24, tree);
    // printf("\n----------POST---------\n");
    // printBTree(tree->root);
    // delete(22, tree);
    // printf("\n----------POST---------\n");
    // printBTree(tree->root);
    printf("pre tree h: %zu, post tree h: %zu", tree2->height, tree->height);
    printf("pre tree cnt: %zu, post tree cnt: %zu", tree2->size, tree->size);
    destroyBTree(&tree2);
    destroyBTree(&tree);
    return 0;
}
#endif 