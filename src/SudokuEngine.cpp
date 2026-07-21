#include <stdlib.h>
#include <stdio.h>
#include "sudoku.h"
#include "generate.h"

/*-----------------------------------------
    SudokuEngine.cpp

    Contains the core Sudoku solving engine.

    The engine maintains bit-mask
    representations of every row,
    column and square for efficient
    legality testing.

    Two solving algorithms are available:

	• Classic iterative backtracking
	• Recursive MRV (Most Restricted Variable)

    The MRV solver is also used by the
    Sudoku generator when checking
    puzzle uniqueness.
  -----------------------------------------*/

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

//#define MRV_TRACE

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
Find the empty cell with the fewest legal values
to minimise backtracking during the search.

TRUE  = empty cell found
FALSE = board is completely filled
-----------------------------------------*/

BOOL	FindBestEmptyCellForSolver(int &bestRow, int &bestCol)
    {
    int bestCount = ArraySize + 1;
    BOOL found = FALSE;

    for (int row = 0; row < ArraySize; row++)
	{
	for (int col = 0; col < ArraySize; col++)
	    {
	    if (known[row][col])
		continue;

	    int count = 0;

	    for (int value = 1; value <= ArraySize; value++)
		{
		if (is_available(row, col, value))
		    count++;
		}

	    if (count < bestCount)
		{
		bestCount = count;
		bestRow = row;
		bestCol = col;
		found = TRUE;

		if (count <= 1)
		    return TRUE;
		}
	    }
	}

    return found;
    }

  /*---------------------------------------------------------
    Recursive Sudoku solver using the most-constrained-cell
    heuristic.

    Returns:
	TRUE  - a solution has been found
	FALSE - this branch has no solution
  ---------------------------------------------------------*/

int SolvePuzzleMRV(void)
    {
    int row;
    int col;
    static __int64 Nodes = 0;
    int legal = 0;
    static __int64 Backtracks = 0;

#ifdef MRV_TRACE
    OutputDebugStringA("Entering solve_sudoku\n");
#endif

    // Find the empty cell having the fewest legal candidates.
    // If no empty cell remains, the puzzle is solved.
    if (!FindBestEmptyCellForSolver(row, col))
	return TRUE;

#ifdef MRV_TRACE
    char s[100];

    sprintf(s, "Cell = (%d,%d)\n", row, col);
    OutputDebugStringA(s);
#endif

    // Try every possible value in this cell.
    for (int value = 1; value <= ArraySize; value++)
	{
	if (!is_available(row, col, value))
	    {
	    continue;
	    }

	legal++;

#ifdef MRV_TRACE
	sprintf(s, "Trying %d at (%d,%d)\n", value, row, col);
	OutputDebugStringA(s);

	Nodes++;

	if ((Nodes % 100) == 0)
	    {
	    char s[100];

	    sprintf(s, "Nodes = %I64d\n", Nodes);
	    OutputDebugStringA(s);
	    }
#endif

	// Place the trial value.
	set_cell(row, col, value);
	known[row][col] = 1;

	// Search from this new position.
	if (SolvePuzzleMRV())
	    return TRUE;

	// This value led to a dead end.
	// Remove it and try the next value.
	clear_cell(row, col);
	known[row][col] = 0;
	}

#ifdef MRV_TRACE
    sprintf(s, "Backtracking from (%d,%d), %d candidates examined\n", row, col, legal);
    OutputDebugStringA(s);
    Backtracks++;

    if ((Backtracks % 1000) == 0)
	{
	sprintf(s, "Backtracks=%lld  Nodes=%lld\n", Backtracks, Nodes);
	OutputDebugStringA(s);
	}
#endif

    // No value worked in this cell.
    return FALSE;
    }

/*-----------------------------------------
    Classic iterative backtracking solver.

    Retained as an alternative to the newer
    MRV recursive solver for comparison and
    experimentation.
-----------------------------------------*/

#ifndef USE_MRV_SOLVER
BOOL	solve_sudoku(void)
    {
    int pos = 0;
    while (1)
	{
	while (pos < ArraySize * ArraySize && known[pos / ArraySize][pos%ArraySize])
	    ++pos;
	if (pos >= ArraySize * ArraySize)
	    return TRUE;		// solved
	if (advance_cell(pos / ArraySize, pos%ArraySize))
	    ++pos;
	else
	    {
	    do
		{
		--pos;
		} while (pos >= 0 && known[pos / ArraySize][pos%ArraySize]);
		if (pos < 0)
		    return FALSE;     // no solution
	    }
	}
    }
#endif

/*-----------------------------------------
    Solve the current Sudoku puzzle.

    This routine selects the configured
    solving algorithm, measures the elapsed
    execution time, and reports the timing
    information to the debug output.

    Returns TRUE if a solution is found,
    otherwise FALSE.
  -----------------------------------------*/

BOOL SolvePuzzle()
    {
    LARGE_INTEGER Frequency;
    LARGE_INTEGER Start;
    LARGE_INTEGER Finish;
    BOOL Result;
    char s[100];

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&Start);
    
#ifdef USE_MRV_SOLVER
    Result = SolvePuzzleMRV();
#else
    Result = solve_sudoku();
#endif

    QueryPerformanceCounter(&Finish);
    double ElapsedMS = (1000.0 * (Finish.QuadPart - Start.QuadPart)) / Frequency.QuadPart;    
    _snprintf_s(s, sizeof(s), _TRUNCATE, "Solver time = %.3f ms\n", ElapsedMS);
    OutputDebugStringA(s);
    return Result;
    }

/*-----------------------------------------
    Initialise the Sudoku engine.

    Clears the puzzle, resets the row,
    column and square bit masks, and
    prepares the lookup table used for
    fast constraint checking.
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

void SetSudokuMode(BOOL IsHex)
    {
    IsHEX = IsHex;

    ArraySize = IsHEX ? HEXARRAYSIZE : NORMALARRAYSIZE;
    SquareSize = IsHEX ? HEXSQUARESIZE : NORMALSQUARESIZE;

    init_arrays();
    }

