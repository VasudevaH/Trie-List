#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#define NODE_NUM 10000000
#define LEN 26
#define MAX 100
#define NODE_SIZE 9

typedef struct Node{
    char ch;
    struct Node* children, *sibling;
}Node;


int deleteStack[MAX],deleteTop;
unsigned int end, availableTop;

FILE *fp, *fpStack, *fpCnt;
int trie_fd,cnt_fd,stack_fd;
Node *trie;
int *cnt,*stack;


int main()
{
    trie_fd = open("TrieBinary",O_RDWR);
    if (trie_fd == -1) {
	perror("Error opening file for writing");
	exit(EXIT_FAILURE);
    }
    trie = mmap(0, NODE_NUM*sizeof(int)*LEN, PROT_READ | PROT_WRITE, MAP_SHARED, trie_fd, 0);
    if (trie == MAP_FAILED) {
	close(trie_fd);
	perror("Error mmapping the file");
	exit(EXIT_FAILURE);
    }

    //Node root = trie[0];
    // printf("initial c = %c\n",root.ch);
    // printf("initial children = %d\n",root.children);

    // root.ch='b';
    // root.children=10;
    // root.sibling=20;
    // trie[0]=root;
    // root.ch='c';
    // root.children=100;
    // root.sibling=200;
    // trie[1]=root;

    //root = trie[0];
    // printf("%c\n",trie[0].ch);
    // printf("%d\n",trie[0].children);
    // printf("%d\n",trie[0].sibling);
    // trie[0].children=1; 
    printf("%c\n",trie[1].ch);
    printf("%d\n",trie[1].children);
    printf("%d\n",trie[1].sibling);
    printf("ch=%c\n",trie[2].ch);
    printf("c=%d\n",trie[2].children);
    printf("s=%d\n",trie[2].sibling);
   



    if (munmap(trie, NODE_NUM*sizeof(int)*LEN) == -1) {
	perror("Error un-mmapping the file");
    }
    close(trie_fd);
}