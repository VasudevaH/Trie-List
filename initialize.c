#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define LEN 26
#define MAX 100
#define NODE_NUM 1027804


typedef struct Node
{
    
    int sibling;
    int children;
    char ch;
} Node;
typedef struct DeleteNode
{
    char ch;
    int current_off;
    int previous_off;
}DeleteNode;

typedef struct Avaliable
{
  int a_offset;
}Avaliable;

int trie_fd, ava_fd, stack_fd;
Node *trie;
DeleteNode *deleteTire;
Avaliable *avaliable;


void main()
{
    unsigned int end=1,avaliableTop=0;
    FILE *fp = fopen("TrieBinary", "w+");
    FILE *fpStack = fopen("deleteStack","w+");
    FILE *fpavl = fopen("AvaliableTrie","w+");
    if(fp==NULL)
        printf("unable to open file\n");
    fseek(fp, NODE_NUM*sizeof(Node), SEEK_SET);
    end=0;
    fwrite(&end, sizeof(unsigned int), 1, fp);
    fseek(fpavl, NODE_NUM*sizeof(int), SEEK_SET);
    fwrite(&end, sizeof(unsigned int), 1, fpavl);
    fseek(fpStack, NODE_NUM*sizeof(int), SEEK_SET);
    fwrite(&end, sizeof(unsigned int), 1, fpStack);
    fclose(fpavl);
    fclose(fpStack);
    fclose(fp);

    
    
    trie_fd = open("TrieBinary",O_RDWR);
    if (trie_fd == -1) {
	perror("Error opening file for writing");
	exit(EXIT_FAILURE);
    }
    trie = mmap(0, NODE_NUM*sizeof(Node), PROT_READ | PROT_WRITE, MAP_SHARED, trie_fd, 0);
    if (trie == MAP_FAILED) {
	close(trie_fd);
	perror("Error mmapping the file");
	exit(EXIT_FAILURE);
    }

    trie[0].children=1;
        printf("end %d\n",trie[0].children);
    trie[1].children=0;
    
    if (munmap(trie, NODE_NUM*sizeof(Node)) == -1) {
	perror("Error un-mmapping the file");
    }
    close(trie_fd);
//-----------------------------------------------------------------------

    ava_fd = open("AvaliableTrie",O_RDWR);
    if (ava_fd == -1) {
	perror("Error opening file for writing");
	exit(EXIT_FAILURE);
    }
    avaliable = mmap(0, NODE_NUM*sizeof(Avaliable), PROT_READ | PROT_WRITE, MAP_SHARED, ava_fd, 0);
    if (avaliable == MAP_FAILED) {
	close(ava_fd);
	perror("Error mmapping the file");
	exit(EXIT_FAILURE);
    }
    avaliable[avaliableTop].a_offset=0;
    printf("avaliable Top=%d\n", avaliable[avaliableTop].a_offset);

    if (munmap(avaliable, NODE_NUM * sizeof(Avaliable)) == -1)
    {
        perror("Error un-mmapping the file");
    }
    close(ava_fd);


}