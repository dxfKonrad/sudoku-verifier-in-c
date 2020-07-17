# Sudoku verifier in C

The code is partially based on the code from:
https://github.com/sarmadhashmi/multithreaded-sudoku-validator

The verifier accepts 4x4 and 9x9 sudokus, verifies its input data and checks whether the provided sudoku is valid.
It has been written in C and is meant to be merely an illustration of some C language features for the blog article:

## *"The slightly bumpy road to Sudoku verifier in C".*


To compile the code:

```gcc ./src/sudokuValidator.c -o ./bin/sudokuValidator```

To execute the verifier:

```./bin/sudokuValidator ./data/<sudokuFileame>```

