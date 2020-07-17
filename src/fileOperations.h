#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queueOperations.h"

node * readFromFile(char *fileName)
{
    FILE *myFile;
    myFile = fopen(fileName, "r");
    if (myFile == NULL)
    {
        printf("Error reading file: %s\n", fileName);
        exit(0);
    }

    node* head = NULL;

    createList(myFile, &head);
    
    return head;
}