//                      gcc binary.c -o binary;./binary
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
#define MAX 100
/*
Maximum number of nodes that a
trie file can have
*/
#define NODE_NUM 1027804


/*
Structure of the Trie node.
*/
typedef struct Node
{

    int sibling;
    int children;
    char ch;
} Node;

/*
Structure to keep track of deleted nodes.
and add it delete_node stack.
*/
typedef struct DeleteNode
{
    char ch;
    int current_off;
    int previous_off;
} DeleteNode;

/*
Struture to add deleted nodes into 
available list Stack.
*/
typedef struct Avaliable
{
    int a_offset;
} Avaliable;

//Initialsing the Stacks.
int deleteTop = -1;
unsigned int availableTop = -1;

//End pointer and file pointer.
unsigned int end;
int trie_fd, ava_fd, stack_fd;

//Pointers to Array of mmapped file.
Node *trie;
DeleteNode *deleteTire;
Avaliable *avaliable;


/*
    FUNCTION NAME:InsertTrie
    INPUT:String
    OUTPUT:Return '1' on successful insertion.
           Return '2' if string already exists.
           Return '-1' on unsucessful operation.
    DESCRIPTION:The functions inserts string into 
    the Trie DataStructure.
*/
int insertTrie(char *key)
{
    int i;
    int prev_off = 0;
    int curr_off = 1;
    //Initialise  Root Node as my the current node.
    Node curr = trie[curr_off];
    //Iterate untill end of string.
    for (i = 0; key[i]; i++)
    {
        //Check if current node has childern
        if (abs(curr.children))
        {
            //check if the current node child character is same as scanned character.
            if (trie[abs(trie[abs(curr_off)].children)].ch == key[i])
            {
                curr = trie[abs(trie[abs(curr_off)].children)];
                prev_off = abs(curr_off);
                curr_off = abs(trie[abs(curr_off)].children);
            }
            //check if the scanned character is present in sibliing list.
            else
            { 
                //update current,previous offset and current node.
                curr = trie[abs(trie[abs(curr_off)].children)];
                prev_off = abs(curr_off);
                curr_off = abs(trie[abs(curr_off)].children);

                //flag is set if sibling is found
                int found_sibling = 0;

                // Traverse the sibling list to find the scanned character in the sibiling.
                while (abs(curr.sibling) && !found_sibling)
                {
                    //check if the current node sibiling character is same as scanned character.
                    if (trie[abs(trie[abs(curr_off)].sibling)].ch == key[i])
                    {
                        found_sibling = 1;
                        curr = trie[abs(trie[abs(curr_off)].sibling)];
                        prev_off = abs(curr_off);
                        curr_off = abs(trie[abs(curr_off)].sibling);
                    }
                    //continue traversing the sibling list
                    else
                    {
                        ///update current,previous offset and current node.
                        curr = trie[abs(trie[abs(curr_off)].sibling)];
                        prev_off = abs(curr_off);
                        curr_off = abs(trie[abs(curr_off)].sibling);
                    }
                }
                /*
                if scanned charcter is not found in sibling list 
                then Create new sibling node and linking it to its previous sibling
                 */
                if (!found_sibling)
                {
                    Node temp;
                    /*
                    Ternary operation to check if there are some available nodes which were already deleted 
                    from the Trie file and added to Available stack.else return the latest node.which will be 
                    new_node _number.
                    */
                    int new_node_number = (availableTop == 0)? ++end : avaliable[availableTop--].a_offset;
                    //Fill the new node with current scanned character
                    temp.ch = key[i];
                    temp.children = 0;
                    temp.sibling = 0;
                    //write the new node to Trie file.
                    trie[new_node_number] = temp;
                    //update current ,previous offset and current node
                    curr = trie[abs(new_node_number)];
                    prev_off = abs(curr_off);
                    curr_off = new_node_number;
                    //Add new node to sibling list
                    trie[abs(prev_off)].sibling = new_node_number;
                }
            }
        }
        //Create newe node if current node as no childern.
        else
        {
            // Creating new child node and linking it to its parent
            Node temp;
            /*
                Ternary operation to check if there are some available nodes which were already deleted 
                from the Trie file and added to Available stack.else return the latest node.which will be 
                new_node _number.
            */
            int new_node_number =  (availableTop == 0)? ++end : avaliable[availableTop--].a_offset;
            //Fill the new node with current scanned character
            temp.ch = key[i];
            temp.children = 0;
            temp.sibling = 0;
            //write the new node to Trie file.
            trie[new_node_number] = temp;
             //update current ,previous offset and current node
            prev_off = abs(curr_off);
            curr_off = new_node_number;
            curr = trie[abs(new_node_number)];
            //Add new node has child to parent node i'e previous node
            trie[abs(prev_off)].children = new_node_number;
        }
    }
    //check if current node is child of previous node
    if (abs(trie[abs(prev_off)].children) == curr_off)
    {
        /*
        Check if current node was inserted previously
        by looking into previous child offset.Negative 
        value indicates the string was previously inserted.
        */
        if (trie[abs(prev_off)].children < 0)
            return 2; 
        /*
        Make pervious child offset negative to indicate end of string
        */
        else
        {
            trie[abs(prev_off)].children = -1 * trie[abs(prev_off)].children;
            return 1;
        }
    }
    //check if current node is sibling of previous  node
    else if (abs(trie[abs(prev_off)].sibling) == curr_off)
    {
        /*
        Check if current node was inserted previously
        by looking into previous sibling offset.Negative 
        value indicates the string was previously inserted.
        */
        if (trie[abs(prev_off)].sibling < 0)
           return 2 ; 
         /*
        Make pervious sibling offset negative to indicate end of string
        */
        else
        {
            trie[abs(prev_off)].sibling = -1 * trie[abs(prev_off)].sibling;
            return 1;
        }
    }
    else
        return -1;
}

/*
    FUNCTION NAME:SearchTrie
    INPUT:Array of characters
    OUTPUT:Return 1 if string is found
           Return -1 if String not found
    DESCRIPTION:The functions will search string in
    the Trie DataStructure.
*/
int searchTrie(char *key)
{
    int i;
    int prev_off = 0;
    int curr_off = 1;
    int break_flag = 0;
    //Initialise  Root Node as my the current node.
    Node curr = trie[curr_off];
    
    //Iterate untill end of string.
    for (i = 0; key[i]; i++)
    {
         //Check if current node has childern
        if (abs(curr.children))
        {
            //check if the current node child character is same as scanned character.
            if (trie[abs(trie[abs(curr_off)].children)].ch == key[i] )
            {
                curr = trie[abs(trie[abs(curr_off)].children)];
                prev_off = abs(curr_off);
                curr_off = abs(trie[abs(curr_off)].children);
            }
            //check if the scanned character is present in sibliing list.
            else
            { 
                //update current,previous offset and current node.
                curr = trie[abs(trie[abs(curr_off)].children)];
                prev_off = abs(curr_off);
                curr_off = abs(trie[abs(curr_off)].children);
                //flag is set if sibiling is found.
                int found_sibling = 0;
                // Traverse the sibling list
                while (abs(curr.sibling) && !found_sibling)
                {
                    //check if the current node sibiling character is same as scanned character.
                    if (trie[abs(trie[abs(curr_off)].sibling)].ch == key[i])
                    {
                        found_sibling = 1;
                        curr = trie[abs(trie[abs(curr_off)].sibling)];
                        prev_off = abs(curr_off);
                        curr_off = abs(trie[abs(curr_off)].sibling);
                    }
                    //continue searching the sibling list
                    else
                    {   //update current,previous offset and current node.
                        curr = trie[abs(trie[abs(curr_off)].sibling)];
                        prev_off = abs(curr_off);
                        curr_off = abs(trie[abs(curr_off)].sibling);
                    }
                }
                // Key not found in sibling list
                if (!found_sibling)
                {
                    break_flag = 1;
                    return -1;
                }
            }
        }
        else
        {
            break_flag = 1;
            return -1;
        }
    }
    
    
    //check if current node is child of previous node
    if (abs(trie[abs(prev_off)].children) == curr_off)
    {
        /*
        Check if previous node child offset is negative
        indicting end of string
        */
        if (trie[abs(prev_off)].children < 0)
            return 1;
        else
            return -1;
    }
    //check if current node is sibling of previous node
    else if (abs(trie[abs(prev_off)].sibling) == curr_off)
    {
        /*
        Check if previous node sibling offset is negative
        indicting end of string
        */
        if (trie[abs(prev_off)].sibling < 0)
            return 1;
        else
            return -1;
    }

}

/*
    FUNCTION NAME:deleteTrie
    INPUT:String
    OUTPUT:Return 1 if string is successfully deleted
           Return -1 if deletion operation fails
    DESCRIPTION:The functions will search string in
                the Trie DataStructure and then delete 
                the string.
*/
void deleteTriea(char *key)
{
    int i, prev_off = 0, curr_off = 1, break_flag = 0;
    Node curr = trie[curr_off];

    for (i = 0; key[i]; i++)
    {

        if (abs(curr.children))
        {
            if (trie[abs(trie[abs(curr_off)].children)].ch == key[i])
            {
                curr = trie[abs(trie[abs(curr_off)].children)];
                prev_off = abs(curr_off);
                curr_off = abs(trie[abs(curr_off)].children);
            }
            else
            { // Sibling traversal
                curr = trie[abs(trie[abs(curr_off)].children)];
                prev_off = abs(curr_off);
                curr_off = abs(trie[abs(curr_off)].children);
                int found_sibling = 0;
                // Traverse the sibling list
                while (abs(curr.sibling) && !found_sibling)
                {
                    if (trie[abs(trie[abs(curr_off)].sibling)].ch == key[i])
                    {
                        found_sibling = 1;
                        curr = trie[abs(trie[abs(curr_off)].sibling)];
                        prev_off = abs(curr_off);
                        curr_off = abs(trie[abs(curr_off)].sibling);
                    }
                    else
                    {
                        curr = trie[abs(trie[abs(curr_off)].sibling)];
                        prev_off = abs(curr_off);
                        curr_off = abs(trie[abs(curr_off)].sibling);
                    }
                }
                // Key not found in sibling list
                if (!found_sibling)
                    break_flag = 1;
            }
        }
        else
        {
            break_flag = 1;
            break;
        }
        if (break_flag == 1)
        {
            //printf("String is not present to delete it\n");
            return;
        }

        DeleteNode DeleteTemp;
        DeleteTemp.ch = key[i];
        DeleteTemp.current_off = curr_off;
        DeleteTemp.previous_off = prev_off;
        deleteTop++;
        deleteTire[deleteTop] = DeleteTemp;
    }


    for (int i = deleteTop; i >= 0; i--)
    {
        int substringflag = 0;
        int sl=strlen(key) - 1;
        if (i == (sl) && trie[deleteTire[i].current_off].children != 0)
        {
            if (trie[deleteTire[i].current_off].children != 0)
            {
                if (abs(trie[deleteTire[i].previous_off].children) == deleteTire[i].current_off && trie[deleteTire[i].previous_off].children < 0)
                {
                    trie[deleteTire[i].previous_off].children = trie[deleteTire[i].previous_off].children * -1;
                    substringflag = 1;
                }
                else if (abs(trie[deleteTire[i].previous_off].sibling) == deleteTire[i].current_off && trie[deleteTire[i].previous_off].sibling < 0)
                {
                    trie[deleteTire[i].previous_off].sibling = trie[deleteTire[i].previous_off].sibling * -1;
                    substringflag = 1;
                }
            }
            if (substringflag == 1)
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

            if (trie[deleteTire[i].current_off].children == 0)
            {
                if (trie[deleteTire[i].current_off].sibling == 0)
                {
                    if (abs(trie[deleteTire[i].previous_off].sibling) == deleteTire[i].current_off)
                    {
                        if (trie[deleteTire[i].previous_off].sibling < 0 && (i == sl))
                            trie[deleteTire[i].previous_off].sibling = 0;
                        else if (trie[deleteTire[i].previous_off].sibling > 0 && (i != sl))
                            trie[deleteTire[i].previous_off].sibling = 0;
                        else if (trie[deleteTire[i].previous_off].sibling < 0 && (i != sl))
                        {
                            // printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                            break;
                        }
                    }
                    else if (abs(trie[deleteTire[i].previous_off].children) == deleteTire[i].current_off)
                    {
                        if (trie[deleteTire[i].previous_off].children < 0 && (i == sl))
                            trie[deleteTire[i].previous_off].children = 0;
                        else if (trie[deleteTire[i].previous_off].children > 0 && (i != sl))
                            trie[deleteTire[i].previous_off].children = 0;
                        else if (trie[deleteTire[i].previous_off].children < 0 && (i != sl))
                        {
                            // printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                            break;
                        }
                    }
                    // printf("%c is deleted\n",trie[deleteTire[i].current_off].ch);
                    availableTop++;
                    avaliable[availableTop].a_offset = deleteTire[i].current_off;
                    trie[deleteTire[i].current_off].ch = '~';
                    trie[deleteTire[i].current_off].children = 0;
                    trie[deleteTire[i].current_off].sibling = 0;
                }
                else if (trie[deleteTire[i].current_off].sibling != 0)
                {
                    if (abs(trie[deleteTire[i].previous_off].sibling) == deleteTire[i].current_off)
                    {
                        if (trie[deleteTire[i].previous_off].sibling < 0 && (i == sl))
                            trie[deleteTire[i].previous_off].sibling = trie[deleteTire[i].current_off].sibling;
                        else if (trie[deleteTire[i].previous_off].sibling > 0 && (i != sl))
                            trie[deleteTire[i].previous_off].sibling = trie[deleteTire[i].current_off].sibling;
                        else if (trie[deleteTire[i].previous_off].sibling < 0 && (i != sl))
                            break;
                    }
                    else if (abs(trie[deleteTire[i].previous_off].children) == deleteTire[i].current_off)
                    {
                        if (trie[deleteTire[i].previous_off].children < 0 && (i == sl))
                            trie[deleteTire[i].previous_off].children = trie[deleteTire[i].current_off].sibling;
                        else if (trie[deleteTire[i].previous_off].children > 0 && (i != sl))
                            trie[deleteTire[i].previous_off].children = trie[deleteTire[i].current_off].sibling;
                        else if (trie[deleteTire[i].previous_off].children < 0 && (i != sl))
                        {
                            //  printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                            break;
                        }
                    }
                    // printf("%c is deleted\n",trie[deleteTire[i].current_off].ch);
                    availableTop++;
                    avaliable[availableTop].a_offset = deleteTire[i].current_off;
                    trie[deleteTire[i].current_off].ch = '~';
                    trie[deleteTire[i].current_off].children = 0;
                    trie[deleteTire[i].current_off].sibling = 0;
                }
            }
        }
    }
}
/*
    FUNCTION NAME:
    INPUT
    OUTPUT
    DESCRIPTION
*/
int main(int argc, char** argv)
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
    deleteTire = mmap(0, NODE_NUM * sizeof(DeleteNode), PROT_READ | PROT_WRITE, MAP_SHARED, stack_fd, 0);
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
    avaliable = mmap(0, NODE_NUM * sizeof(Avaliable), PROT_READ | PROT_WRITE, MAP_SHARED, ava_fd, 0);
    if (avaliable == MAP_FAILED)
    {
        close(ava_fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    availableTop = avaliable[0].a_offset;
    end = trie[0].children;
    printf("%d initial end\n", end);
    char key[MAX];
    FILE *fpin = fopen(argv[2], "r");

    switch (argv[1][0])
    {
    case 'i':
        availableTop = avaliable[0].a_offset;
        printf("avaliable %d\n", availableTop);
        while (fscanf(fpin, "%s", key) != -1)
            insertTrie(key);
        printf("end = %d\n", end);
        trie[0].children = end;
        avaliable[0].a_offset = availableTop;

        break;

    case 's':
        while (fscanf(fpin, "%s", key) != -1)
            searchTrie(key);
        printf("end = %d\n", end);
        trie[0].children = end;

        break;

    case 'd':
        while (fscanf(fpin, "%s", key) != -1)
        {
            deleteTop = -1;
            deleteTriea(key);
        }
        
        avaliable[0].a_offset = availableTop;
        printf("avaliable top=%d\n", availableTop);

        break;
    }
    fclose(fpin);

    avaliable[0].a_offset = availableTop;
    printf("%d\n", avaliable[0].a_offset);

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