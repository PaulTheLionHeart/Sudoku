#include <windows.h>
#include <Winuser.h>
#include <commdlg.h>
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#pragma once

// Return values :
// IMPASSE - Puzzle cannot be solved.
// REFRESH - Redisplay the current puzzle.
// FINISH - User finished normally.
// CANCEL - User cancelled the dialog.
// CALCULATE - solve the puzzle

#define REFRESH                             10
#define FINISH                              11
#define CALCULATE                           12
#define IMPASSE                             13
#define CANCEL				    14

#define NORMALARRAYSIZE			    9
#define HEXARRAYSIZE			    16
#define NORMALSQUARESIZE		    3
#define HEXSQUARESIZE			    4
#define MAXARRAYSIZE			    HEXARRAYSIZE
#define MAXCELLS			    (MAXARRAYSIZE * MAXARRAYSIZE)

#define RWIDTH(rect) (rect.right - rect.left)	// IB 2009-04-14
#define RHEIGHT(rect) (rect.bottom - rect.top)

int	ShowSudokuDialog(HINSTANCE hInst, HWND hwnd, DLGPROC dlgProc);
void	InsertText(HDC hdc, char *szText, int x, int y, int size, DWORD BkColour, DWORD colour);
void	HideAllCandidates(HWND hDlg, int row, int col);
void	ShowAllCandidates(HWND hDlg, int row, int col);
void	HideCell(HWND hDlg, int row, int col);
void	ShowCell(HWND hDlg, int row, int col);

extern	INT_PTR CALLBACK    ChooseValueDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

extern	void	ShowCandidate(HWND hDlg, int row, int col, int value);
extern	void	HideCandidate(HWND hDlg, int row, int col, int value);
extern	BOOL	GetBigCellRect(HWND hDlg, int row, int col, RECT *rc);
extern	int	CellButton(int row, int col);
extern	void	UndoCell(HWND hDlg, int row, int col);
extern	void	PaintSudokuBoard(HWND hDlg, HDC hdc);
extern	void	PaintHexSudokuBoard(HWND hDlg);
extern	void	InitialiseBoard(HWND hDlg);
extern	void	InsertText(HDC hdc, char *szText, int x, int y, int size, DWORD colour, DWORD BkColour);
extern	void	PlaceValue(HWND hDlg, int row, int col, int value);
extern	void	PaintHexCandidates(HWND hDlg, HDC hdc, int row, int col);
extern	void	RefreshCell(HWND hDlg, int row, int col);
extern	void	SelectCell(HWND hDlg, int row, int col);
extern	void	ToggleOriginal(HWND hDlg, int row, int col);
extern	void	SetSudokuMode(BOOL IsHex);

extern	int	ArraySize, SquareSize;				// ArraySize is the size of the side of the puzzle, SquareSize is the size of the side of a square
extern	BOOL	IsHEX;						// default is 9*9 Sudoku
extern	int	matrix[HEXARRAYSIZE][HEXARRAYSIZE];
extern	int	known[HEXARRAYSIZE][HEXARRAYSIZE];		// dimension for larger array if required
extern	int	orig[HEXARRAYSIZE][HEXARRAYSIZE];		// Which numbers were given in the original problem?

// An array of nine integers, each of which representing a sub-square.
//Each integer has its nth-bit on iff n belongs to the corresponding sub-square.
extern	int squares[HEXARRAYSIZE];

// An array of nine integers, each of which representing a row.
//Each integer has its nth-bit on iff n belongs to the corresponding row.
extern	int rows[HEXARRAYSIZE];

// An array of nine integers, each of which representing a column.
//Each integer has its nth-bit on iff n belongs to the corresponding column. 
extern	int cols[HEXARRAYSIZE];

extern	int bits[HEXARRAYSIZE + 1];				// // An array with some powers of 2 to avoid shifting all the time.

extern	BOOL	IsOrig;						// Save the original puzzle

#define USE_MRV_SOLVER		1

// stuff from SudokuEngine.cpp
extern	void	init_known(void);
extern	void	init_arrays(void);
extern	BOOL	SolvePuzzle(void);
extern	bool	is_available(int i, int j, int n);
extern	void	set_cell(int i, int j, int n);
extern	int	clear_cell(int i, int j);
extern	int	SaveData(HWND hwnd, char *szFileName, char *szAppName, BOOL csv);
extern	int	GetData(HWND hwnd, char *szFileName, char *szAppName);
extern	int	square(int i, int j);


