@echo off
gcc -std=c2x -gdwarf-2 -Wuninitialized -static-libgcc -fno-inline -fno-omit-frame-pointer -Wno-unused-variable -Wall -ggdb -o test1 ^
test_abtree.c array_search.c abtree.c insert.c delete.c print_btree.c