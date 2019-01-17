#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include<math.h>
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
static inline int  display()
{
    return 2;
}
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



 
    for(int i=2;i<=end;i++)
    {
   printf("ch=%c ",trie[i].ch);
    printf("c=%d ",trie[i].children);
    printf("s=%d\n",trie[i].sibling);

    }
 
    printf("%d\n",display());
   



    if (munmap(trie, NODE_NUM*sizeof(int)*LEN) == -1) {
	perror("Error un-mmapping the file");
    }
    close(trie_fd);
}