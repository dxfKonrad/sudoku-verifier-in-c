#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#include "fileOperations.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* 
	The array which worker threads can update to 1 if the 
	corresponding region of the sudoku puzzle they were responsible 
 	for is valid.
*/
int *valid;

// Struct that stores the data to be passed to the threads checking subsections validity
typedef struct
{
    int row;
    int column;
} parameters;

int supportedSudokuSizes[2] = {4, 9};

// Sudoku puzzle to be solved
int **sudoku;
int sudokuSize = 0;
int subsectionSize = 0;

// Method that determines if numbers 1-[sudokuSize] only appear once in a column
void *isColumnValid(void *columnParam)
{
    int col = *((int *)columnParam);

    // Check if numbers 1-[sudokuSize] only appear once in the column
    int *isNumberPresentArray = (int *)malloc(sudokuSize * sizeof(int));
    memset(isNumberPresentArray, 0, sizeof(*isNumberPresentArray));
    int i;
    for (i = 0; i < sudokuSize; i++)
    {
        int num = sudoku[i][col];
        if (num < 1 || num > sudokuSize || isNumberPresentArray[num - 1] == 1)
        {
            pthread_exit(NULL);
        }
        else
        {
            isNumberPresentArray[num - 1] = 1;
        }
    }
    //column vectors are the [sudokuSize] last ones
    valid[sudokuSize + sudokuSize + col] = 1;
    pthread_exit(NULL);
}

// Method that determines if numbers 1-[sudokuSize] only appear once in a row
void *isRowValid(void *rowParam)
{
    // Confirm that parameters indicate a valid row subsection
    int row = *((int *)rowParam);
    // Check if numbers 1-[sudokuSize] only appear once in the row
    int *isNumberPresentArray = (int *)malloc(sudokuSize * sizeof(int));
    memset(isNumberPresentArray, 0, sizeof(*isNumberPresentArray));

    for (int i = 0; i < sudokuSize; i++)
    {
        int num = sudoku[row][i];
        if (num < 1 || num > sudokuSize || isNumberPresentArray[num - 1] == 1)
        {
            pthread_exit(NULL);
        }
        else
        {
            isNumberPresentArray[num - 1] = 1;
        }
    }
    //row vectors come after ther subsection vectors
    valid[sudokuSize + row] = 1;
    pthread_exit(NULL);
}

// Method that determines if numbers 1-[sudokuSize] only appear once in a subsection
void *isSubsectionValid(void *param)
{
    // Confirm that parameters indicate a valid subsection
    parameters *params = (parameters *)param;
    int row = params->row;
    int col = params->column;

    int *isNumberPresentArray = (int *)malloc(sudokuSize * sizeof(int));
    memset(isNumberPresentArray, 0, sizeof(*isNumberPresentArray));
    int i, j;
    for (i = row; i < row + subsectionSize; i++)
    {
        for (j = col; j < col + subsectionSize; j++)
        {
            int num = sudoku[i][j];
            if (num < 1 || num > sudokuSize || isNumberPresentArray[num - 1] == 1)
            {
                pthread_exit(NULL);
            }
            else
            {
                isNumberPresentArray[num - 1] = 1;
            }
        }
    }
    // If reached this point, subsection is valid.
    valid[row + col / subsectionSize] = 1; // Maps the subsection to an index in the first 8 indices of the valid array
    pthread_exit(NULL);
}

// Method that determines whether the sudoku has one of the supported sizes
int determineSudokuSize(node *headBase)
{
    int arraySize = getListSize(&headBase);

    int sudokuSizesArrayIndex = 0;
    while (sudokuSize == 0 && sudokuSizesArrayIndex < ARRAY_SIZE(supportedSudokuSizes))
    {
        if (supportedSudokuSizes[sudokuSizesArrayIndex] == (int)sqrt(arraySize))
        {
            sudokuSize = supportedSudokuSizes[sudokuSizesArrayIndex];
            printf("Determined sudoku size: %d\n", sudokuSize);
            return 1;
        }
        else
        {
            sudokuSizesArrayIndex++;
        }
    }
    return 0;
}

void displaySudoku()
{
    printf("Sudoku to be verified:\n");

    for (int i = 0; i < sudokuSize; i++)
    {
        for (int j = 0; j < sudokuSize; j++)
        {
            printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }
}

void checkSudokuValidity()
{
    //all rows, all columns and the subsections have to be verified
    int numberOfValidityVectors = sudokuSize + sudokuSize + sudokuSize;
    valid = (int *)malloc(numberOfValidityVectors * sizeof(int));

    //One thread per validity vector
    int numberOfThreads = numberOfValidityVectors;
    pthread_t threads[numberOfThreads];

    int threadIndex = 0;
    int i, j;
    // Create  [sudokuSize] threads for the  subsections,  [sudokuSize] threads for columns and  [sudokuSize] threads for rows.
    // This will end up with a total of 27 threads.
    subsectionSize = (int)sqrt(sudokuSize);
    for (i = 0; i < sudokuSize; i++)
    {
        for (j = 0; j < sudokuSize; j++)
        {
            if (i % subsectionSize == 0 && j % subsectionSize == 0)
            {
                parameters *rowData = (parameters *)malloc(sizeof(parameters));
                rowData->row = i;
                rowData->column = j;
                pthread_create(&threads[threadIndex++], NULL, isSubsectionValid, rowData); // subsection threads
            }
            if (i == 0)
            {
                int *arg = malloc(sizeof(*arg));
                *arg = j;

                pthread_create(&threads[threadIndex++], NULL, isColumnValid, arg); // column threads
            }
            if (j == 0)
            {
                int *arg = malloc(sizeof(*arg));
                *arg = i;

                pthread_create(&threads[threadIndex++], NULL, isRowValid, arg); // row threads
            }
        }
    }

    for (i = 0; i < numberOfThreads; i++)
    {
        pthread_join(threads[i], NULL); // Wait for all threads to finish
    }

    //If any of the entries in the valid array are 0, then the sudoku solution is invalid
    for (i = 0; i < numberOfThreads; i++)
    {
        if (valid[i] == 0)
        {
            printf("Sudoku solution is invalid!\n");
        }
    }

    printf("Sudoku solution is valid!\n");
}

int main(int argc, char **argv)
{
    node *head;
    if (argc < 2)
    {
        char *fileName = malloc(256 * sizeof(int));
        printf("Please specify the sudoku file name:");
        scanf("%s", fileName);
        head = readFromFile(fileName);
        free(fileName);
    }
    else
    {
        head = readFromFile(argv[1]);
    }

    if (!determineSudokuSize(head))
    {
        printf("The number of provided values does not match the supported sudoku sizes!\n");
        printf("The supported sudoku sizes are:\n");
        for(int i = 0; i < ARRAY_SIZE(supportedSudokuSizes); i++)
        {
            printf("%d x %d\n", supportedSudokuSizes[i], supportedSudokuSizes[i]);
        }
        return EXIT_FAILURE;
    }

    sudoku = (int **)malloc(sudokuSize * sizeof(int *));
    for (int i = 0; i < sudokuSize; i++)
    {
        sudoku[i] = (int *)malloc(sudokuSize * sizeof(int));
    }

    fillArrayFromList(&head, sudoku, sudokuSize);

    displaySudoku();

    checkSudokuValidity();
}
