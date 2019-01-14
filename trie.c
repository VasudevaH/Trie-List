//                      gcc binary.c -o binary;./binary
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
    char ch;
    int sibling;
    int children;
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
int deleteStack[MAX], deleteTop=-1;
unsigned int end, availableTop=-1;

FILE *fp, *fpStack, *fpava;
int trie_fd, ava_fd, stack_fd;
Node *trie;
DeleteNode *deleteTire;
Avaliable *avaliable;





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
    //return ++end;
    if (availableTop == 0)
     return ++end;
    else
    {
        int val = avaliable[availableTop].a_offset;
        availableTop--;
        return val;
    }
    
}
void insertTrie(char *key)
{
    int i,prev_off=0,curr_off=1;
    Node curr = trie[curr_off];

    for(i = 0; key[i];i++)
    {
        if(absolute(curr.children))
        {
            if(trie[absolute(trie[absolute(curr_off)].children)].ch==key[i])
            {
                curr=trie[absolute(trie[absolute(curr_off)].children)];
                prev_off=absolute(curr_off);
                curr_off=absolute(trie[absolute(curr_off)].children);
            }
            else
            {   // Sibling traversal 
                curr=trie[absolute(trie[absolute(curr_off)].children)];
                prev_off=absolute(curr_off);
                curr_off=absolute(trie[absolute(curr_off)].children);
                int found_sibling=0;
                // Traverse the sibling list
                while(absolute(curr.sibling) && !found_sibling)
                {
                    if(trie[absolute(trie[absolute(curr_off)].sibling)].ch==key[i])
                    {
                        found_sibling=1;
                        curr=trie[absolute(trie[absolute(curr_off)].sibling)];
                        prev_off=absolute(curr_off);
                        curr_off=absolute(trie[absolute(curr_off)].sibling);
                        

                    }
                    else
                    {
                        curr=trie[absolute(trie[absolute(curr_off)].sibling)];
                        prev_off=absolute(curr_off);
                        curr_off=absolute(trie[absolute(curr_off)].sibling);
                    }



                }
                 // Key not found in sibling list
                if(!found_sibling)
                {
                   // Creating new sibling node and linking it to its previous sibling
                    Node temp;
                    int new_node_number=newNode();
                    temp.ch=key[i];
                    temp.children=0;
                    temp.sibling=0;
                    trie[new_node_number]=temp;
                    curr=trie[absolute(new_node_number)]; 
                    prev_off=absolute(curr_off);
                    curr_off=new_node_number;
                    trie[absolute(prev_off)].sibling=new_node_number;
                   
                 }
                }
            }
        else
        {
            // Creating new child node and linking it to its parent
            Node temp;
            int new_node_number=newNode();
            //printf("new node=%d",new_node_number);
            temp.ch=key[i];
            temp.children=0;
            temp.sibling=0;
            trie[new_node_number]=temp;
            prev_off=absolute(curr_off);
            curr_off=new_node_number;
            curr=trie[absolute(new_node_number)];
            trie[absolute(prev_off)].children=new_node_number;
            
         }

    }




    if(absolute(trie[absolute(prev_off)].children)==curr_off)
    {
        if(trie[absolute(prev_off)].children<0)
              ;//       printf("String alredy inserted\n");
        else
        {
            trie[absolute(prev_off)].children=-1*trie[absolute(prev_off)].children;
            //printf("String inserted\n");
        }  
    }
    else if(absolute(trie[absolute(prev_off)].sibling)==curr_off)
    {
         if(trie[absolute(prev_off)].sibling<0)
                  ;//   printf("String alredy inserted\n");
        else
        {
            trie[absolute(prev_off)].sibling=-1*trie[absolute(prev_off)].sibling;
           // printf("String inserted\n");
        }
    }

}


void searchTrie(char *key)
{
    int i,prev_off=0,curr_off=1,break_flag=0;
    Node curr = trie[curr_off];

    for(i = 0; key[i];i++)
    {
        if(absolute(curr.children))
        {
            if(trie[absolute(trie[absolute(curr_off)].children)].ch==key[i])
            {
                curr=trie[absolute(trie[absolute(curr_off)].children)];
                prev_off=absolute(curr_off);
                curr_off=absolute(trie[absolute(curr_off)].children);
            }
            else
            {   // Sibling traversal 
                curr=trie[absolute(trie[absolute(curr_off)].children)];
                prev_off=absolute(curr_off);
                curr_off=absolute(trie[absolute(curr_off)].children);
                int found_sibling=0;
                // Traverse the sibling list
                while(absolute(curr.sibling) && !found_sibling)
                {
                    if(trie[absolute(trie[absolute(curr_off)].sibling)].ch==key[i])
                    {
                        found_sibling=1;
                        curr=trie[absolute(trie[absolute(curr_off)].sibling)];
                        prev_off=absolute(curr_off);
                        curr_off=absolute(trie[absolute(curr_off)].sibling);
                        

                    }
                    else
                    {
                        curr=trie[absolute(trie[absolute(curr_off)].sibling)];
                        prev_off=absolute(curr_off);
                        curr_off=absolute(trie[absolute(curr_off)].sibling);
                    }



                }
                 // Key not found in sibling list
                if(!found_sibling)
                {
                    break_flag=1;
                 }
                }
            }
        else
        {
            break_flag=1;
            break;
         }

    }
    if(break_flag==1)
         printf("1==String not present\n");
    else
    {
    if(absolute(trie[absolute(prev_off)].children)==curr_off)
    {
        if(trie[absolute(prev_off)].children<0)
              printf("present\n");
        else
            printf("2==String not present\n");
        
       
    }
    else if(absolute(trie[absolute(prev_off)].sibling)==curr_off)
    {
         if(trie[absolute(prev_off)].sibling<0)
                 printf("String present\n");
        else
                printf("3==String not present\n");
     
       
    }
    }
}


//  deleteTop   deleteTire[]

void deleteTriea(char *key)
{
    int i,prev_off=0,curr_off=1,break_flag=0;
    Node curr = trie[curr_off];

    for(i=0;key[i];i++)
    {


        if(absolute(curr.children))
        {
            if(trie[absolute(trie[absolute(curr_off)].children)].ch==key[i])
            {
                curr=trie[absolute(trie[absolute(curr_off)].children)];
                prev_off=absolute(curr_off);
                curr_off=absolute(trie[absolute(curr_off)].children);
            }
            else
            {   // Sibling traversal 
                curr=trie[absolute(trie[absolute(curr_off)].children)];
                prev_off=absolute(curr_off);
                curr_off=absolute(trie[absolute(curr_off)].children);
                int found_sibling=0;
                // Traverse the sibling list
                while(absolute(curr.sibling) && !found_sibling)
                {
                    if(trie[absolute(trie[absolute(curr_off)].sibling)].ch==key[i])
                    {
                        found_sibling=1;
                        curr=trie[absolute(trie[absolute(curr_off)].sibling)];
                        prev_off=absolute(curr_off);
                        curr_off=absolute(trie[absolute(curr_off)].sibling);
                        

                    }
                    else
                    {
                        curr=trie[absolute(trie[absolute(curr_off)].sibling)];
                        prev_off=absolute(curr_off);
                        curr_off=absolute(trie[absolute(curr_off)].sibling);
                    }
                }
                 // Key not found in sibling list
                if(!found_sibling)
                        break_flag=1;
                 
                }
            }
        else
        {
            break_flag=1;
            break;
        }
        if(break_flag==1)
                {
                    //printf("String is not present to delete it\n");
                    return ;
                }

        DeleteNode DeleteTemp;
        DeleteTemp.ch=key[i];
        DeleteTemp.current_off=curr_off;
        DeleteTemp.previous_off=prev_off;
        deleteTop++;
        deleteTire[deleteTop]=DeleteTemp;
     

    }

    // for(int i=deleteTop;i>=0;i--)
    // {
    //     printf("ch=%c current=%d prev=%d\n",deleteTire[i].ch,deleteTire[i].current_off,deleteTire[i].previous_off);
    // }

    for(int i=deleteTop;i>=0;i--)
    {
        int substringflag=0;
        if(i==(strlen(key)-1) && trie[deleteTire[i].current_off].children!=0)
        {
             if(trie[deleteTire[i].current_off].children!=0)
                {
                    if(absolute(trie[deleteTire[i].previous_off].children)==deleteTire[i].current_off && trie[deleteTire[i].previous_off].children<0)
                    {
                            trie[deleteTire[i].previous_off].children=trie[deleteTire[i].previous_off].children*-1;
                            substringflag=1;
                    }
                    else if(absolute(trie[deleteTire[i].previous_off].sibling)==deleteTire[i].current_off && trie[deleteTire[i].previous_off].sibling<0)
                    {
                            trie[deleteTire[i].previous_off].sibling=trie[deleteTire[i].previous_off].sibling*-1;
                            substringflag=1;
                    }
                }
                if(substringflag==1)
                {
                   // searchTrie(key);
                   // printf("String deleted\n");
                    return;
                }
                else
                {
                   // searchTrie(key);
                  //  printf("Substring not deleted\n");
                    return;
                }
            
            }
            else
            {
                //availableTop avaliable

                if(trie[deleteTire[i].current_off].children==0  )
                {
                    if(trie[deleteTire[i].current_off].sibling==0 )
                    {
                        if(absolute(trie[deleteTire[i].previous_off].sibling)==deleteTire[i].current_off)
                        {
                            if(trie[deleteTire[i].previous_off].sibling<0 && (i==strlen(key)-1)) 
                                    trie[deleteTire[i].previous_off].sibling=0;
                            else if(trie[deleteTire[i].previous_off].sibling>0 && (i!=strlen(key)-1)) 
                                     trie[deleteTire[i].previous_off].sibling=0;
                            else if(trie[deleteTire[i].previous_off].sibling<0 && (i!=strlen(key)-1))
                                    {
                                       // printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                                        break;
                                    }
                             
                        }
                        else if(absolute(trie[deleteTire[i].previous_off].children)==deleteTire[i].current_off)
                        {
                            if(trie[deleteTire[i].previous_off].children<0 && (i==strlen(key)-1))
                                trie[deleteTire[i].previous_off].children=0;
                            else if(trie[deleteTire[i].previous_off].children>0 && (i!=strlen(key)-1)) 
                                     trie[deleteTire[i].previous_off].children=0;
                            else if(trie[deleteTire[i].previous_off].children<0 && (i!=strlen(key)-1))
                                   {
                                       // printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                                        break;
                                    }
                        }
                       // printf("%c is deleted\n",trie[deleteTire[i].current_off].ch);
                        availableTop++;
                        avaliable[availableTop].a_offset=deleteTire[i].current_off;
                        trie[deleteTire[i].current_off].ch='~';
                        trie[deleteTire[i].current_off].children=0;
                        trie[deleteTire[i].current_off].sibling=0;
                        
                    }
                    else if(trie[deleteTire[i].current_off].sibling!=0)
                    {
                        if(absolute(trie[deleteTire[i].previous_off].sibling)==deleteTire[i].current_off)
                        {
                           if(trie[deleteTire[i].previous_off].sibling<0 && (i==strlen(key)-1)) 
                                 trie[deleteTire[i].previous_off].sibling=trie[deleteTire[i].current_off].sibling;
                           else if(trie[deleteTire[i].previous_off].sibling>0 && (i!=strlen(key)-1)) 
                                trie[deleteTire[i].previous_off].sibling=trie[deleteTire[i].current_off].sibling;
                           else if(trie[deleteTire[i].previous_off].sibling<0 && (i!=strlen(key)-1))
                                    {
                                      //  printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                                        break;
                                    }
                        }
                        else if(absolute(trie[deleteTire[i].previous_off].children)==deleteTire[i].current_off)
                        {
                            if(trie[deleteTire[i].previous_off].children<0 && (i==strlen(key)-1))
                                  trie[deleteTire[i].previous_off].children=trie[deleteTire[i].current_off].sibling;
                             else if(trie[deleteTire[i].previous_off].children>0 && (i!=strlen(key)-1)) 
                                  trie[deleteTire[i].previous_off].children=trie[deleteTire[i].current_off].sibling;
                            else if(trie[deleteTire[i].previous_off].children<0 && (i!=strlen(key)-1))
                                   {
                                      //  printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                                        break;
                                    }
                        }
                       // printf("%c is deleted\n",trie[deleteTire[i].current_off].ch);
                        availableTop++;
                        avaliable[availableTop].a_offset=deleteTire[i].current_off;
                        trie[deleteTire[i].current_off].ch='~';
                        trie[deleteTire[i].current_off].children=0;
                        trie[deleteTire[i].current_off].sibling=0;
                        

                    }


                }


            }

    }



}


int main()
{
    trie_fd = open("TrieBinary", O_RDWR);
    if (trie_fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    trie = mmap(0, NODE_NUM * sizeof(Node), PROT_READ | PROT_WRITE, MAP_SHARED, trie_fd, 0);
    if (trie == MAP_FAILED)
    {
        close(trie_fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }




    stack_fd = open("deleteStack", O_RDWR);
    if (stack_fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    deleteTire= mmap(0, NODE_NUM * sizeof(DeleteNode), PROT_READ | PROT_WRITE, MAP_SHARED, stack_fd, 0);
    if (deleteTire == MAP_FAILED)
    {
        close(stack_fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }



    ava_fd = open("AvaliableTrie", O_RDWR);
    if (ava_fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    avaliable= mmap(0, NODE_NUM * sizeof(Avaliable), PROT_READ | PROT_WRITE, MAP_SHARED, ava_fd, 0);
    if (avaliable == MAP_FAILED)
    {
        close(ava_fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    availableTop=avaliable[0].a_offset;
    int e=1;
        end = trie[0].children;
        char filename[20]="dictionary1";
     //printf("%d initial new node\n",trie[0].children);
         char key[MAX];
          FILE *fpin = fopen(filename, "r");
           FILE *fpin2 = fopen(filename, "r");
            FILE *fpin3 = fopen(filename, "r");
    switch(e)
{
    case 1:
availableTop= avaliable[0].a_offset;
   printf("avaliable %d\n",availableTop);
    while (fscanf(fpin, "%s", key) != -1)
        {
            insertTrie(key);
        }
     printf("end = %d\n",end);
    trie[0].children = end;
    avaliable[0].a_offset=availableTop;

    fclose(fpin);
  break;

case 2:
   
    printf("search-----------------\n");
    int i=1;
    while (fscanf(fpin2, "%s", key) != -1)
        {
          printf("%s\n ",key);  
          searchTrie(key);
         
        }
    
    fclose(fpin2);
     printf("end = %d\n",end);
    trie[0].children = end;
    
    break;


case 3:
   
    printf("delete-----------------\n");
    while (fscanf(fpin3, "%s", key) != -1)
        {
            deleteTop=-1;
         // printf("%s \n",key);  
          deleteTriea(key);
        }
    
    fclose(fpin3);
   

    avaliable[0].a_offset=availableTop;
   printf("avaliable top=%d\n",availableTop);
   
   break;

}


 avaliable[0].a_offset=availableTop;
 printf("%d\n",avaliable[0].a_offset);

   
    if (munmap(trie, NODE_NUM * sizeof(Node)) == -1)
    {
        perror("Error un-mmapping the file");
    }
    close(trie_fd);


       if (munmap(deleteTire, NODE_NUM * sizeof(DeleteNode)) == -1)
    {
        perror("Error un-mmapping the file");
    }
    close(stack_fd);

       if (munmap(avaliable, NODE_NUM * sizeof(Avaliable)) == -1)
    {
        perror("Error un-mmapping the file");
    }
    close(ava_fd);



  













    return 0;
}