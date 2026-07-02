#include <windows.h>
#include <Winuser.h>
#include <commdlg.h>
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#pragma once

#define REFRESH                             10
#define FINISH                              11
#define CALCULATE                           12
#define CHOOSEVALUE                         13
#define IMPASSE                             14
#define NORMALARRAYSIZE			    9
#define HEXARRAYSIZE			    16
#define NORMALSQUARESIZE		    3
#define HEXSQUARESIZE			    4

#define RWIDTH(rect) (rect.right - rect.left)	// IB 2009-04-14
#define RHEIGHT(rect) (rect.bottom - rect.top)

int	ShowSudokuDialog(HINSTANCE hInst, HWND hwnd, DLGPROC dlgProc);
void	InsertText(HDC hdc, char *szText, int x, int y, int size, DWORD BkColour, DWORD colour);
void	HideAllCandidates(HWND hDlg, int row, int col);
void	ShowAllCandidates(HWND hDlg, int row, int col);
void	HideCell(HWND hDlg, int row, int col);
void	ShowCell(HWND hDlg, int row, int col);

