#include <stdlib.h>
#include <stdio.h>
#include "sudoku.h"

extern	int	ArraySize, SquareSize;			// ArraySize is the size of the side of the puzzle, SquareSize is the size of the side of a square

int	matrix[HEXARRAYSIZE][HEXARRAYSIZE];		// The Sudoku matrix itself
int	known[HEXARRAYSIZE][HEXARRAYSIZE];		// Which numbers were given as known in the problem

// known[][] indicates cells fixed before solving. Solver-generated values are intentionally not marked
// as known so that the backtracking algorithm can erase and replace them during the search.

int	orig[HEXARRAYSIZE][HEXARRAYSIZE];		// Which numbers were given in the original problem?

// An array of nine integers, each of which representing a sub-square
// Each integer has its nth-bit on iff n belongs to the corresponding sub-square. 
int squares[HEXARRAYSIZE];

// An array of nine integers, each of which representing a row.
// Each integer has its nth-bit on iff n belongs to the corresponding row. 
int rows[HEXARRAYSIZE];

// An array of nine integers, each of which representing a column.
// Each integer has its nth-bit on iff n belongs to the corresponding column.
int cols[HEXARRAYSIZE];

int bits[HEXARRAYSIZE + 1];				// An array with some powers of 2 to avoid shifting all the time

/*-----------------------------------------
	Returns the index of the square the cell (i, j) belongs to
  -----------------------------------------*/

int square(int i, int j)
    {
    return (i/SquareSize)*SquareSize + j/SquareSize;
    }

/*-----------------------------------------
	Stores the number n in the cell (i, j), and turns on the corresponding bits in rows, cols, and squares
  -----------------------------------------*/

void set_cell(int i, int j, int n)
    {
    matrix[i][j] = n;
    rows[i] |= bits[n];
    cols[j] |= bits[n];
    squares[square(i, j)] |= bits[n];
    }

/*-----------------------------------------
	Clears the cell (i, j) and turns off the corresponding bits in rows, cols, and squares. Returns the number it contained
  -----------------------------------------*/

int clear_cell(int i, int j)
    {
    int n = matrix[i][j];
    matrix[i][j] = 0;
    rows[i] &= ~bits[n];
    cols[j] &= ~bits[n];
    squares[square(i, j)] &= ~bits[n];
    return n;
    }

/*-----------------------------------------
	Test stub to initialise a Sudoku
  -----------------------------------------*/

void init_known(void)
    {
    int	i, j;

    matrix[0][3] = 2;
    matrix[0][5] = 7;
    matrix[1][1] = 4;
    matrix[1][7] = 8;
    matrix[2][2] = 7;
    matrix[2][3] = 3;
    matrix[2][4] = 8;
    matrix[2][5] = 9;
    matrix[2][6] = 4;
    matrix[3][2] = 1;
    matrix[3][4] = 6;
    matrix[3][6] = 3;
    matrix[4][3] = 9;
    matrix[4][5] = 2;
    matrix[5][2] = 9;
    matrix[5][4] = 7;
    matrix[5][6] = 5;
    matrix[6][2] = 4;
    matrix[6][3] = 5;
    matrix[6][4] = 1;
    matrix[6][5] = 3;
    matrix[6][6] = 7;
    matrix[7][1] = 5;
    matrix[7][7] = 6;
    matrix[8][3] = 4;
    matrix[8][5] = 6;

    known[0][3] = 1;
    known[0][5] = 1;
    known[1][1] = 1;
    known[1][7] = 1;
    known[2][2] = 1;
    known[2][3] = 1;
    known[2][4] = 1;
    known[2][5] = 1;
    known[2][6] = 1;
    known[3][2] = 1;
    known[3][4] = 1;
    known[3][6] = 1;
    known[4][3] = 1;
    known[4][5] = 1;
    known[5][2] = 1;
    known[5][4] = 1;
    known[5][6] = 1;
    known[6][2] = 1;
    known[6][3] = 1;
    known[6][4] = 1;
    known[6][5] = 1;
    known[6][6] = 1;
    known[7][1] = 1;
    known[7][7] = 1;
    known[8][3] = 1;
    known[8][5] = 1;

    for (i = 0; i < 9; ++i) 
	{
	for (j = 0; j < 9; ++j) 
	    {
	    if (known[i][j])
		set_cell(i, j, matrix[i][j]);
	    }
	}
    }

/*-----------------------------------------
	Can we put n in the cell (i, j)?
  -----------------------------------------*/

bool is_available(int i, int j, int n)
    {
    return (rows[i] & bits[n]) == 0 && (cols[j] & bits[n]) == 0 && (squares[square(i, j)] & bits[n]) == 0;
    }

/*-----------------------------------------
	Tries to fill the cell (i, j) with the next available number.
	Returns a flag to indicate if it succeeded.
  -----------------------------------------*/

bool advance_cell(int i, int j)
    {
    int n = clear_cell(i, j);
    while (++n <= ArraySize) 
	{
        if (is_available(i, j, n)) 
	    {
            set_cell(i, j, n);
            return true;
	    }
	}
    return false;
    }

/*-----------------------------------------
	The main function, a fairly generic backtracking algorithms
  -----------------------------------------*/

int solve_sudoku(void)
    {
    int pos = 0;
    while (1) 
	{
        while (pos < ArraySize * ArraySize && known[pos/ArraySize][pos%ArraySize]) 
            ++pos;
        if (pos >= ArraySize * ArraySize)
            return 0;
        if (advance_cell(pos/ArraySize, pos%ArraySize))
	    ++pos;
        else 
	    {
            do 
		{
                --pos;
		} while (pos >= 0 && known[pos/ArraySize][pos%ArraySize]);
            if (pos < 0) 
                return -1;
	    }
	}
    }

/*-----------------------------------------
	Initialise the arrays
  -----------------------------------------*/

void init_arrays(void)
    {
    int i, j, n;

    for (i = 0; i < ArraySize; i++)
	{
	rows[i] = 0;
	cols[i] = 0;
	squares[i] = 0;
	for (j = 0; j < ArraySize; j++)
	    {
	    known[i][j] = 0;			// 0 means no value set or found
	    orig[i][j] = 0;			// 0 means no value set or found
	    matrix[i][j] = 0;			// reset everything
	    }
	}
    for (n = 1; n < HEXARRAYSIZE + 1; n++) 
	{
	bits[n] = 1 << n;			// Initialise the arrays with powers of 2
	}
    }

