//                  gcc initialize.c -o initialize;./initialize
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
#define NODE_SIZE 9

typedef struct Node{
    char ch;
    struct Node* children, *sibling;
}Node;

FILE *fp, *fpStack, *fpCnt;
int trie_fd,cnt_fd,stack_fd;
Node *trie;

int main()
{

    unsigned int end=1;
    FILE *fp = fopen("TrieBinary", "w+");
    FILE *fpStack = fopen("TrieStack","w+");
    FILE *fpCnt = fopen("TrieCnt","w+");
    //fseek(fp, 5, SEEK_SET);
    //fwrite(&end, sizeof(unsigned int), 1, fp);
    fseek(fp, NODE_NUM*sizeof(int)*NODE_SIZE, SEEK_SET);
    end=0;
    fwrite(&end, sizeof(unsigned int), 1, fp);
    fseek(fpCnt, NODE_NUM*sizeof(int), SEEK_SET);
    fwrite(&end, sizeof(unsigned int), 1, fpCnt);
    fseek(fpStack, NODE_NUM*sizeof(int), SEEK_SET);
    fwrite(&end, sizeof(unsigned int), 1, fpStack);
    fclose(fpCnt);
    fclose(fpStack);
    fclose(fp);

    
    
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

    trie[0].children=1;


    if (munmap(trie, NODE_NUM*sizeof(int)*LEN) == -1) {
	perror("Error un-mmapping the file");
    }
    close(trie_fd);
    return 0;
}
