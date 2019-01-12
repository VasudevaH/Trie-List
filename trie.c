//                      gcc binary.c -o binary;./binary
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define LEN 26
#define MAX 100
#define NODE_NUM 10000000

typedef struct Node
{
    char ch;
    int sibling;
    int children;
} Node;

int deleteStack[MAX], deleteTop;
unsigned int end, availableTop;

FILE *fp, *fpStack, *fpCnt;
int trie_fd, cnt_fd, stack_fd;
Node *trie;
int *cnt, *stack;

Node root;

int absolute(int x)
{
    return x - 2 * (x < 0) * x;
}

int newNode()
{
    //if stack is empty
    if (end >= NODE_NUM)
    {
        ; //printf("Trie memory full\n");
        exit(0);
    }
    if (availableTop == 0)
    // return ++end;
    {
        return ++end;
    }
    int val;
    val = stack[availableTop - 1 + 1];

    if (val == 1)
    {
        val = stack[availableTop - 2 + 1];
        availableTop -= 2;
        return val;
    }
    availableTop--;
    return val; 
}
void insertTrie(char *key)
{
    int i,prev=1,prevprev=0,found,found_sibling,end_sibling=0,cond;
    Node curr = trie[1];
    cond=0;

    //printf("ch = %c\nsibling = %d\nchildren = %d\n",curr.ch,curr.sibling,curr.children);
    for(i = 0; key[i];i++)
    {
        //printf("curr.children = %d\n",curr.children);
        if(curr.children)
        {
            //printf("If curr.children = %d\n",curr.children);
            found=0;
            while(curr.children && !found)
            {
                if(trie[absolute(curr.children)].ch == key[i])
                    {
                        curr = trie[absolute(curr.children)];
                        prevprev=prev;
                        if(trie[absolute(trie[absolute(trie[absolute(prevprev)].sibling)].children)].children == curr.children)
                            {
                                cond=1;
                                prev = trie[absolute(prev)].sibling;
                            }
                        else
                            {
                                prev = trie[absolute(prev)].children;
                            }
                        found=1;
                    }
                else
                    {
                        prev=trie[absolute(prev)].children;
                        curr = trie[absolute(curr.children)];
                        found_sibling = 0;
                        printf("-------------prev %d curr=.ch %c\n",prev,curr.ch);

                        while(curr.sibling && !found_sibling)
                        {
                            if(trie[absolute(curr.sibling)].ch == key[i])
                                {
                                    curr = trie[absolute(curr.sibling)];
                                    found_sibling = 1;
                                    found=1;
                                }
                             /* else if(trie[absolute(curr.sibling)].ch > key[i])
                                {
                                    Node temp;
                                    int new_node = newNode();
                                    if(!key[i+1])
                                        end_sibling=1;
                                    temp.ch = key[i];
                                    temp.children = 0;
                                    temp.sibling = trie[absolute(prev)].children;
                                    trie[absolute(prev)].children = new_node;
                                    found_sibling = 1;
                                    found=1;
                                    prevprev=prev;
                                    prev = new_node;
                                    curr = temp;
                                }*/
                            else
                                {
                                    
                                    prevprev=prev;
                                    prev = trie[absolute(prev)].sibling;
                                    curr = trie[absolute(curr.sibling)];
                                   
                                }
                        }
                        if(!found_sibling)
                        {
                              if(!key[i+1])
                                        end_sibling=1;
                            Node temp;
                            found=1;
                            // prevprev = prev;
                            // prev=trie[absolute(prev)].children;
                            int new_node = newNode();
                            temp.ch = key[i];
                            temp.children = 0;
                            temp.sibling = curr.sibling;
                            // trie[absolute(trie[absolute(prev)].sibling)].sibling = new_node;
                            trie[absolute(prev)].sibling = new_node;
                            trie[new_node] = temp;
                            prevprev=prev;
                            prev=new_node;
                            curr = temp;
                        }

                    }
            }
        }
        else
        {
            //printf("prev = %d\n",prev);
            trie[absolute(prev)].children = newNode();
            curr = trie[absolute(trie[absolute(prev)].children)];      
            curr.ch = key[i];
            curr.children = 0;
            curr.sibling = 0;

            trie[absolute(trie[absolute(prev)].children)].ch = key[i];
            trie[absolute(trie[absolute(prev)].children)].sibling = 0;
            trie[absolute(trie[absolute(prev)].children)].children = 0;
            prevprev=prev;
            prev = trie[absolute(prev)].children;             
        }
    }
    if(cond)
        {
            prevprev=prev;
        }
    printf("prev.ch=%c\n",trie[absolute(prev)].ch);
    printf("prev.children=%d\n",trie[absolute(prev)].children);
    if(end_sibling)
    {
        if(trie[absolute(prevprev)].sibling<0)
            printf("String already present\n");
        else
            {
                trie[absolute(prevprev)].sibling=-1*trie[absolute(prevprev)].sibling;
                printf("Inserted children = %d\n",trie[absolute(prevprev)].sibling);
            }
    }
    else
    {
        if(trie[absolute(prevprev)].children<0)
            printf("String already present\n");
        else
            {
                trie[absolute(prevprev)].children=-1*trie[absolute(prevprev)].children;
                printf("Inserted children = %d\n",trie[absolute(prevprev)].children);
            }
    }
    // trie[prev].children = -newNode();
    // curr = trie[trie[prev].children];      
    // curr.ch = key[i];
    // curr.children = 0;
    // curr.sibling = 0;
    // prev = trie[prev].children;



}
int main()
{
    trie_fd = open("TrieBinary", O_RDWR);
    if (trie_fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    trie = mmap(0, NODE_NUM * sizeof(int) * LEN, PROT_READ | PROT_WRITE, MAP_SHARED, trie_fd, 0);
    if (trie == MAP_FAILED)
    {
        close(trie_fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    end = trie[0].children;

    printf("Initial end = %d\n",end);

    char key[MAX];
    // Node node;
    // node.children = newNode();
    // trie[1] = node;
    //printf("root ch = %c\nroot.children = %d\nroot.sibling = %d\n",root.ch,root.children,root.sibling);

    //printf("tri[2] ch = %c\ntrie[2].children = %d\ntrie[2].sibling = %d\n",trie[2].ch,trie[2].children,trie[2].sibling);
    //insertTrie("apple");
    // insertTrie("apple");
    // insertTrie("apple");
    //printf("wolrd %d\n", end);
    FILE *fpin = fopen("words_10", "r");
    while (fscanf(fpin, "%s", key) != -1)
        {
            //printf("len = %d\n",strlen(key));
            printf("%s\n",key);
            insertTrie(key);
        }
    fclose(fpin);
    // for(int i=0;i<10;i++)
    // {
    //     insertTrie("apple");
    // }

    printf("end = %d\n",end);



    trie[0].children = end;
    if (munmap(trie, NODE_NUM * sizeof(int) * LEN) == -1)
    {
        perror("Error un-mmapping the file");
    }
    close(trie_fd);
    return 0;
}
