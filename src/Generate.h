#include <windows.h>
#include <Winuser.h>
#include <commdlg.h>
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

// Stuff for Sudoku generator

enum DifficultyType
    {
    EASY,
    MEDIUM,
    HARD,
    EXPERT
    };

enum SymmetryType
    {
    ROTATIONAL,
    MIRROR,
    RANDOM
    };

struct GenerateOptions
    {
    BOOL IsHex;

    DifficultyType Difficulty;
    SymmetryType Symmetry;

    GenerateOptions()
	{
	IsHex = FALSE;
	Difficulty = EASY;
	Symmetry = ROTATIONAL;
	}
    };

extern	INT_PTR CALLBACK GenerateDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
extern	BOOL	GeneratePuzzle(HWND, GenerateOptions &GenOptions);
extern	BOOL	FindBestEmptyCell(int &bestRow, int &bestCol);
extern	void	ShuffleValues(int values[]);
extern	int	CountSolutionsRecursive(int limit);
extern	BOOL	FillBoardRandom(void);
extern	int	CountSolutions(void);
extern	BOOL	RemoveClues(HWND hwnd, GenerateOptions &GenOptions);
extern	void	MarkAllOriginals(void);
extern	void	Stats(GenerateOptions &GenOptions, int RemovedClues, double GenerationTime, long long RecursiveNodes, char *Message);
extern	void	SetProgress(HWND hDlg, int Position);
extern	void	SetProgressRange(HWND hDlg, int Maximum);

