/*-----------------------------------------
	GENERATE.CPP
	Sudoku creating Program Written by Paul de Leeuw
  -----------------------------------------*/

#include "resource.h"
#include "sudoku.h"
#include "generate.h"

/*-----------------------------------------
    Puzzle generation proceeds in four stages:

        1. Create a complete valid Sudoku grid
           using recursive backtracking.

        2. Verify that the completed grid has
           exactly one solution.

        3. Remove clues in random order while
           repeatedly checking uniqueness.

        4. Produce the finished puzzle and
           generation statistics.

    The generator always favours correctness
    over speed. Every accepted clue removal is
    verified by the recursive solver.
  -----------------------------------------*/

static	DWORD	StartTime;
static	DWORD	GenerationDeadline;
static	__int64 TotalNodes = 0;
static	__int64 NodesThisClue = 0;

#define SEARCH_TIMEOUT    (-1)
//#define	GENERATOR_DIAGNOSTICS	1

/*-----------------------------------------
    Return the maximum time (milliseconds)
    allowed for a uniqueness search.

    Larger 16x16 hexadecimal puzzles require
    a longer timeout than standard 9x9 puzzles.
  -----------------------------------------*/

DWORD GetTimeBudget(void)
    {
    return (ArraySize == 16) ? 10000 : 3000;
    }

/*-----------------------------------------
    Update the window title and process any
    pending Windows messages.

    Called during lengthy operations so that
    status changes are displayed immediately
    and the application remains responsive.
  -----------------------------------------*/

void	SetStatus(HWND hwnd, LPCSTR text)
    {
    SetWindowText(hwnd, text);
    UpdateWindow(hwnd);

    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	}
    }
     
/*-----------------------------------------
    Generate a new Sudoku puzzle using the selected
    options (size, difficulty and symmetry).
  -----------------------------------------*/

BOOL	GeneratePuzzle(HWND hwnd, GenerateOptions &GenOptions)
    {
    HCURSOR OldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    OutputDebugStringA("GeneratePuzzle(): begin\n");
    SetStatus(hwnd, "Sudoku - Generating complete board...");
    SetSudokuMode(GenOptions.IsHex);
    StartTime = GetTickCount();
    GenerationDeadline = GetTickCount() + GetTimeBudget();
    TotalNodes = 0;
    NodesThisClue = 0;
    SetProgressRange(hwnd, ArraySize * ArraySize);
    SetProgress(hwnd, 0);

    if (!FillBoardRandom())
	{
	OutputDebugStringA("GeneratePuzzle(): FillBoardRandom failed\n");
	MessageBox(hwnd, "Unable to generate a complete Sudoku grid.", "Generate Sudoku", MB_OK | MB_ICONEXCLAMATION);
	SetWindowText(hwnd, "Paul's Sudoku Generator");
	SetCursor(OldCursor);
	return FALSE;
	}

    MarkAllOriginals();

    OutputDebugStringA("GeneratePuzzle(): complete grid generated\n");

    SetStatus(hwnd, "Sudoku - Verifying solution...");
    int count = CountSolutions();

    if (count != 1)
	{
	OutputDebugStringA("GeneratePuzzle(): generated board is not unique!\n");
	}
    else
	{
	OutputDebugStringA("GeneratePuzzle(): uniqueness verified!\n");
	}

    SetStatus(hwnd, "Sudoku - Removing clues...");
    RemoveClues(hwnd, GenOptions);

    SetWindowText(hwnd, "Paul's Sudoku Generator");     // restore original title

    // Return to normal editing mode.
    SetCursor(OldCursor);
    IsOrig = FALSE;
    SendMessage(hwnd, WM_COMMAND, IDM_SELECTSUDOKU, 0L);
    return TRUE;
    }

/*-----------------------------------------
    Find the empty cell with the fewest legal values
    to minimise backtracking during the search.

    TRUE  = empty cell found
    FALSE = board is completely filled
  -----------------------------------------*/

BOOL	FindBestEmptyCell(int &bestRow, int &bestCol)
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

/*-----------------------------------------
    Count the number of solutions, stopping as soon
    as two solutions have been found.

    Returns:
	0 = no solution
	1 = unique solution
	2 = multiple solutions
  -----------------------------------------*/

int	CountSolutions()
    {
    return CountSolutionsRecursive(2);
    }

/*-----------------------------------------
    Recursive backtracking solver used to count
    solutions while limiting the search to 'limit'
    solutions for efficiency.
  -----------------------------------------*/

int	CountSolutionsRecursive(int limit)
    {
    if (limit <= 0)
	return 0;

    if ((LONG)(GetTickCount() - GenerationDeadline) >= 0)
	{
#ifdef GENERATOR_DIAGNOSTICS
	OutputDebugStringA("Uniqueness search timed out.\n");
#endif
	return SEARCH_TIMEOUT;
	}

    int row;
    int col;

    if (!FindBestEmptyCell(row, col))
	return 1;               // complete valid solution found

    int count = 0;

    for (int value = 1; value <= ArraySize; value++)
	{
	if (!is_available(row, col, value))
	    continue;

	TotalNodes++;
	NodesThisClue++;

#ifdef GENERATOR_DIAGNOSTICS
	if ((TotalNodes % 1000000) == 0)
	    {
	    char s[100];

	    sprintf(s, "Visited %I64d million nodes\n",	TotalNodes / 1000000);

	    OutputDebugStringA(s);
	    }
#endif

	set_cell(row, col, value);
	known[row][col] = 1;

	// Reduce the remaining search limit once solutions have been found.
	int result = CountSolutionsRecursive(limit - count);

	clear_cell(row, col);
	known[row][col] = 0;

	// Did the search time out?
	if (result == SEARCH_TIMEOUT)
	    return SEARCH_TIMEOUT;

	// Count the solutions found.
	count += result;

	// Have we found enough solutions?
	if (count >= limit)
	    return limit;
	}
    return count;
    }

/*-----------------------------------------
    Randomly shuffle the candidate values so each
    generated puzzle starts from a different solution.
  -----------------------------------------*/

void	ShuffleValues(int values[])
    {
    for (int i = 0; i < ArraySize; i++)
	values[i] = i + 1;

    for (int i = ArraySize - 1; i > 0; i--)
	{
	int j = rand() % (i + 1);

	int temp = values[i];
	values[i] = values[j];
	values[j] = temp;
	}
    }

/*-----------------------------------------
    Recursively fill an empty board with a complete,
    valid Sudoku solution using random backtracking.
-----------------------------------------*/

BOOL	FillBoardRandom(void)
    {
    static int MaxFilled = 0;

    int row;
    int col;

    if (!FindBestEmptyCell(row, col))
	return TRUE;			// Board complete.

    int Filled = 0;

    for (int r = 0; r < ArraySize; r++)
	{
	for (int c = 0; c < ArraySize; c++)
	    {
	    if (known[r][c])
		Filled++;
	    }
	}

    if (Filled > MaxFilled)
	{
	MaxFilled = Filled;
#ifdef GENERATOR_DIAGNOSTICS
	char s[100];
	sprintf(s, "Maximum filled = %d\n", Filled);
	OutputDebugStringA(s);
#endif 
	}

    int values[16];
    ShuffleValues(values);

    for (int i = 0; i < ArraySize; i++)
	{
	int value = values[i];

	if (!is_available(row, col, value))
	    continue;

	set_cell(row, col, value);
	known[row][col] = 1;

	if (FillBoardRandom())
	    return TRUE;

	clear_cell(row, col);
	known[row][col] = 0;
	}

    return FALSE;
    }

/*-----------------------------------------
    Mark every occupied cell as an original clue.
  -----------------------------------------*/

void	MarkAllOriginals(void)
    {
    for (int row = 0; row < ArraySize; row++)
	{
	for (int col = 0; col < ArraySize; col++)
	    {
	    orig[row][col] = known[row][col];
	    }
	}
    }

/*-----------------------------------------
    Build a randomly ordered list of every
    cell in the Sudoku grid. This allows
    clues to be removed in a different order
    each time a puzzle is generated.
  -----------------------------------------*/

void	ShuffleCells(int cells[])
    {
    // Build the list of cell numbers.

    for (int i = 0; i < ArraySize * ArraySize; i++)
	cells[i] = i;

    // Shuffle the list using the Fisher-Yates algorithm.

    for (int i = ArraySize * ArraySize - 1; i > 0; i--)
	{
	int j = rand() % (i + 1);

	int temp = cells[i];
	cells[i] = cells[j];
	cells[j] = temp;
	}
    }

/*-----------------------------------------
    Attempt to remove a single clue from the
    puzzle. The clue is only removed if the
    resulting puzzle still has exactly one
    solution. Returns TRUE if the clue was
    successfully removed.
    -----------------------------------------*/

BOOL TryRemoveOneClue(int row, int col)
    {
    // Is there a clue here?
    if (!known[row][col])
	return FALSE;

    // Remember the clue.
    int value = matrix[row][col];

    // Remove it.
    clear_cell(row, col);
    known[row][col] = 0;
    orig[row][col] = 0;

    // Is the solution still unique?
    OutputDebugStringA("    Checking uniqueness...\n");
    NodesThisClue = 0;

    if (CountSolutions() == 1)
	return TRUE;

    set_cell(row, col, value);
    known[row][col] = 1;
    orig[row][col] = 1;

    return FALSE;
    }

/*--------------------------------------------------------------------
    Attempt to remove a pair of clues related by 180 degree rotational
    symmetry. Both clues are removed together and the puzzle is tested
    for uniqueness. If the puzzle no longer has a unique solution, both
    clues are restored.

    Returns TRUE if the pair can be removed while preserving a unique
    solution, otherwise FALSE.
    ---------------------------------------------------------------*/


BOOL TryRemoveSymmetricPair(int row1, int col1,
    int row2, int col2)
    {
    int value1 = matrix[row1][col1];
    int value2 = matrix[row2][col2];

    // Remove the first clue.
    clear_cell(row1, col1);
    known[row1][col1] = 0;
    orig[row1][col1] = 0;

    // Remove the second clue if it is different.
    if (row1 != row2 || col1 != col2)
	{
	clear_cell(row2, col2);
	known[row2][col2] = 0;
	orig[row2][col2] = 0;
	}

    NodesThisClue = 0;

    // Check uniqueness.
    if (CountSolutions() == 1)
	return TRUE;

    // Restore first clue.
    set_cell(row1, col1, value1);
    known[row1][col1] = 1;
    orig[row1][col1] = 1;

    // Restore second clue.
    if (row1 != row2 || col1 != col2)
	{
	set_cell(row2, col2, value2);
	known[row2][col2] = 1;
	orig[row2][col2] = 1;
	}

    return FALSE;
    }

/*-----------------------------------------
    Starting with a completed Sudoku,
    repeatedly attempt to remove clues while
    preserving a unique solution.

    Clues are processed in random order.
    Each proposed removal (or symmetric pair)
    is accepted only if the recursive solver
    confirms that the resulting puzzle still
    has exactly one solution.
  -----------------------------------------*/

BOOL RemoveClues(HWND hwnd, GenerateOptions &GenOptions)
    {
    char    s[100];
    int	    Removed = 0;
    DWORD   StartTime;
    DWORD   LastImprovementTime;
    DWORD   CurrentTime;
    int	    Attempts = 0;

    // Stage 1: Find one removable clue.
    int Cells[MAXCELLS];

    // Shuffle the list using the Fisher-Yates algorithm.
    ShuffleCells(Cells);

    StartTime = GetTickCount();
    LastImprovementTime = StartTime;
    SetStatus(hwnd, "Sudoku - Verifying uniqueness...");
    for (int i = 0; i < ArraySize * ArraySize; i++)
	{
	int row = Cells[i] / ArraySize;
	int col = Cells[i] % ArraySize;
	int row2, col2;
	SetProgress(hwnd, i + 1);

	switch (GenOptions.Symmetry)
	    {
	    case ROTATIONAL:
		row2 = ArraySize - 1 - row;
		col2 = ArraySize - 1 - col;
		break;

	    case MIRROR:
		row2 = row;
		col2 = ArraySize - 1 - col;
		break;

	    default:
		row2 = row;
		col2 = col;
		break;
	    }

	// For symmetric puzzles, only process one member of each pair.
	if (GenOptions.Symmetry != RANDOM)
	    {
	    if (row > row2)
		continue;

	    if (row == row2 && col > col2)
		continue;
	    }

	BOOL Accepted;

	Attempts++;

	// Only check the clock occasionally.
	if ((Attempts % 50) == 0)
	    {
	    CurrentTime = GetTickCount();
	    sprintf_s(s, sizeof(s), "Sudoku - Removed %d clues (%d attempts)", Removed,	Attempts);
//	    sprintf_s(s, sizeof(s), "Sudoku - Removing clues... %d/%d", i + 1, ArraySize * ArraySize);
	    SetStatus(hwnd, s);
	    if (CurrentTime - StartTime >= GetTimeBudget())
		break;
	    }

	if (GenOptions.Symmetry != RANDOM)
	    Accepted = TryRemoveSymmetricPair(row, col, row2, col2);
	else
	    Accepted = TryRemoveOneClue(row, col);

	if (Accepted)
	    {
	    LastImprovementTime = GetTickCount();
	    if (GenOptions.Symmetry != RANDOM)
		{
		if (row == row2 && col == col2)
		    Removed++;
		else
		    Removed += 2;
		}
	    else
		{
		Removed++;
		if ((Removed % 10) == 0)
		    {
		    sprintf(s, "Removed=%d  Attempts=%d\n", Removed, Attempts);
		    OutputDebugStringA(s);
		    }
		}

//	    if (Removed >= TargetRemovals)
//		break;
	    }
	}

    char Message[1024];

    CurrentTime = GetTickCount();

    Stats(GenOptions, Removed, (CurrentTime - StartTime) / 1000.0, TotalNodes, Message);
    MessageBox(hwnd, Message, "Puzzle Generation Complete", MB_OK | MB_ICONINFORMATION);
/*
    sprintf(s, "Successfully removed %d clues\n", Removed);
    OutputDebugStringA(s);
    CurrentTime = GetTickCount();
    sprintf(s, "Generation time = %.3f seconds\n", (CurrentTime - StartTime) / 1000.0);
    OutputDebugStringA(s);
    sprintf(s, "Total recursive nodes visited = %I64d\n", TotalNodes);
    OutputDebugStringA(s);
    sprintf(s, "Nodes per removed clue = %.0f\n", (double)TotalNodes / Removed);
    OutputDebugStringA(s);
*/
    return TRUE;
    }

/*-----------------------------------------
    Generate a final Sudoku Generation report.
  -----------------------------------------*/

void	Stats(GenerateOptions &GenOptions, int RemovedClues, double GenerationTime, long long RecursiveNodes, char *Message)
    {
    int CluesRemaining = ArraySize * ArraySize - RemovedClues;
    const char *BoardType = GenOptions.IsHex ? "Hexadecimal" : "Standard";
    const char *Symmetry;
    switch (GenOptions.Symmetry)
	{
	case ROTATIONAL:
	    Symmetry = "Rotational";
	    break;

	case MIRROR:
	    Symmetry = "Mirror";
	    break;

	case RANDOM:
	default:
	    Symmetry = "Random";
	    break;
	}

    sprintf_s(Message, 1024,
	"Puzzle Generation Complete\n\n"
	"A new Sudoku puzzle has been created successfully.\n\n"
	"Puzzle\n"
	"------\n"
	"Board              %d x %d %s\n"
	"Symmetry           %s\n"
	"Clues              %d\n\n"
	"Generation\n"
	"----------\n"
	"Time               %.3f seconds\n"
	"Recursive Nodes    %I64d",
	ArraySize,
	ArraySize,
	BoardType,
	Symmetry,
	CluesRemaining,
	GenerationTime,
	RecursiveNodes);
    }

