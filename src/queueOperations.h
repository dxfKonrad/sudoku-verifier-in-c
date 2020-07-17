#include <stdio.h>
#include <stdlib.h>

typedef struct node {
int value;
struct node  *next;
} node;

void createList(FILE    *fInp, node **headBase)
{
    node    *currNode;
    node    *head = *headBase;
    node    *tail;

    while(!feof(fInp))
    {
        currNode = malloc(sizeof(struct node));
        fscanf(fInp, "%d ", &currNode->value);
        currNode->next = NULL;

        if(NULL == head)
        {
            head = currNode;
            tail = currNode;
        }
        else
        {
            tail->next = currNode;
            tail = currNode;
        }
    }

    //Store back the updated head pointer
    *headBase = head;
}

int getListSize(node **headBase) {
    node    *tmpNode = *headBase;
    if (tmpNode == NULL) {
        return 0;
    }
    int counter = 1;
    while(tmpNode->next != NULL)
    {
        counter++;
        tmpNode = tmpNode->next;
    }
    return counter;
}

void fillArrayFromList(node **headBase, int ** array, int columns) {
    int counter = 0;
   node    *tmpNode = *headBase;

    while(tmpNode->next != NULL)
    {
        array[counter / columns][counter % columns] = tmpNode->value;
        tmpNode = tmpNode->next;
        counter++;
    }
    array[counter / columns][counter % columns] = tmpNode->value;
}

void deleteList(node **headBase)
{
    node    *head = *headBase;
    node    *tmp;

    while(NULL != head)
    {
        tmp = head; // Get a temp pointer
        head = head->next; // Move head pointer
        tmp->next = NULL; // break the link
        printf("<< Deleted Node: %d\n", tmp->value);
        free(tmp);
    }

    // Store the head pointer back which should be NULL
    *headBase = head;
}
