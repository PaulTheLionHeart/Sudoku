#include "sudoku.h"
#include "resource.h"

/*
SudokuDisplay.cpp
-----------------

Address helpers
---------------
CellButton()
CandidateButton()
GetBigCellRect()

Visibility helpers
------------------
HideCell()
ShowCell()
HideCandidate()
ShowCandidate()
HideAllCandidates()
ShowAllCandidates()

Rendering
---------
PaintSudokuBoard()

Initialisation
--------------
InitialiseBoard()

Interaction
-----------
PlaceValue()
UndoCell()
*/

/*-----------------------------------------
	Set the state of the window or button
  -----------------------------------------*/

void	SetWindowState(HWND hDlg, int address, int state)
    {
    HWND hButton;

    hButton = GetDlgItem(hDlg, IDC_BUTTON1 + address);
    ShowWindow(hButton, state);
    }

/*-----------------------------------------
	Get address of the cell button
  -----------------------------------------*/

int CellButton(int row, int col)
    {
    if (IsHEX)
	return row * ArraySize + col;
    return (row * ArraySize + col) * 10;
    }

/*-----------------------------------------
	Get address of the candidate button
  -----------------------------------------*/

int CandidateButton(int row, int col, int value)
    {
    return CellButton(row, col) + value;
    }

/*-----------------------------------------
	Get rectangle of the big cell button
  -----------------------------------------*/

BOOL GetBigCellRect(HWND hDlg, int row, int col, RECT *rc)
    {
    HWND hCtrl = GetDlgItem(hDlg, CellButton(row, col) + IDC_BUTTON1);
    if (!hCtrl)
	{
	OutputDebugStringA("GetBigCellRect not found\n");
	return FALSE;
	}

    GetWindowRect(hCtrl, rc);
    MapWindowPoints(HWND_DESKTOP, hDlg, (POINT *)rc, 2);

    return TRUE;
    }

/*-----------------------------------------
	A few useful helper functions
  -----------------------------------------*/

void ShowCell(HWND hDlg, int row, int col)
    {
    SetWindowState(hDlg, CellButton(row, col), SW_SHOWNORMAL);
    }

void HideCell(HWND hDlg, int row, int col)
    {
    SetWindowState(hDlg, CellButton(row, col), SW_HIDE);
    }

void ShowCandidate(HWND hDlg, int row, int col, int value)
    {
    if (IsHEX)
	{
	RefreshCell(hDlg, row, col);
	return;
	}

    SetWindowState(hDlg, CandidateButton(row, col, value), SW_SHOWNORMAL);
    }

void HideCandidate(HWND hDlg, int row, int col, int value)
    {
    if (IsHEX)
	{
	RefreshCell(hDlg, row, col);
	return;
	}

    SetWindowState(hDlg, CandidateButton(row, col, value), SW_HIDE);
    }

void HideAllCandidates(HWND hDlg, int row, int col)
    {
    if (IsHEX)
	{
	RefreshCell(hDlg, row, col);
	return;
	}

    for (int k = 1; k <= SquareSize * SquareSize; k++)
	SetWindowState(hDlg, CellButton(row, col) + k, SW_HIDE);
    }

void ShowAllCandidates(HWND hDlg, int row, int col)
    {
    if (IsHEX)
	{
	RefreshCell(hDlg, row, col);
	return;
	}
    for (int k = 1; k <= SquareSize * SquareSize; k++)
	SetWindowState(hDlg, CellButton(row, col) + k, SW_SHOWNORMAL);
    }

/*-----------------------------------------
	A small helper for opening the board dialog
  -----------------------------------------*/

int	ShowSudokuDialog(HINSTANCE hInst, HWND hwnd, DLGPROC dlg)
    {
    return IsHEX ? (int)DialogBox(hInst, "DisplayHexDlg", hwnd, dlg) : (int)DialogBox(hInst, "DisplayDlg", hwnd, dlg);
    }

/*-----------------------------------------
    Oops, we hit the wrong number. Better undo
-----------------------------------------*/

void UndoCell(HWND hDlg, int row, int col)
    {
    clear_cell(row, col);
    known[row][col] = 0;
    orig[row][col] = 0;

    if (IsHEX)
	{
	InvalidateRect(hDlg, NULL, TRUE);
	UpdateWindow(hDlg);
	return;
	}

    HideCell(hDlg, row, col);

    RECT rc;
    if (GetBigCellRect(hDlg, row, col, &rc))
	{
	InvalidateRect(hDlg, &rc, TRUE);
	UpdateWindow(hDlg);
	}

    for (int r = 0; r < ArraySize; r++)
	{
	for (int c = 0; c < ArraySize; c++)
	    {
	    if (matrix[r][c] != 0)
		{
		HideAllCandidates(hDlg, r, c);
		}
	    else
		{
		for (int n = 1; n <= ArraySize; n++)
		    {
		    if (is_available(r, c, n))
			ShowCandidate(hDlg, r, c, n);
		    else
			HideCandidate(hDlg, r, c, n);
		    }
		}
	    }
	}
    }

/*-----------------------------------------
    Paint the large Sudoku values
  -----------------------------------------*/

void PaintSudokuBoard(HWND hDlg, HDC hdc)
    {
    char lpText[14];
    RECT rc;

    for (int row = 0; row < ArraySize; row++)
	{
	for (int col = 0; col < ArraySize; col++)
	    {
	    int value = matrix[row][col];

	    if (value == 0)
		continue;

	    if (!GetBigCellRect(hDlg, row, col, &rc))
		continue;

	    if (IsHEX)
		sprintf_s(lpText, sizeof(lpText), "%X", value - 1);
	    else
		sprintf_s(lpText, sizeof(lpText), "%d", value);

	    COLORREF TextColour;

	    if (orig[row][col])
		TextColour = RGB(240, 0, 0);  // Original clue
	    else
		TextColour = RGB(0, 0, 0);    // User or solver

	    InsertText(hdc, lpText, rc.left + (IsHEX ? 8 : 16), rc.top, IsHEX ? 40 : 48, TextColour, RGB(240, 240, 240));
	    }
	}
    }

/*-----------------------------------------
    Initialise the Sudoku board display
  -----------------------------------------*/

void InitialiseBoard(HWND hDlg)
    {
    HWND hCtrl = GetDlgItem(hDlg, IDC_ORIG);
    SendMessage(hCtrl, BM_SETCHECK, IsOrig, 0L);
    for (int row = 0; row < ArraySize; row++)
	{
	for (int col = 0; col < ArraySize; col++)
	    {
	    // Big buttons are now geometry markers only.
	    if (IsHEX)
		ShowCell(hDlg, row, col);
	    else
		HideCell(hDlg, row, col);
	    if (matrix[row][col] != 0)
		{
		// Filled cells display a painted big number.
		HideAllCandidates(hDlg, row, col);
		}
	    else if (IsHEX)
		{
		HideAllCandidates(hDlg, row, col);
		}
	    else
		{
		// Show only candidates currently allowed by
		// the Sudoku engine.
		for (int value = 1; value <= ArraySize; value++)
		    {
		    if (is_available(row, col, value))
			ShowCandidate(hDlg, row, col, value);
		    else
			HideCandidate(hDlg, row, col, value);
		    }
		}
	    }
	}

    SetFocus(GetDlgItem(hDlg, IDC_FINISH));
    InvalidateRect(hDlg, NULL, TRUE);
    }

/*-----------------------------------------
    Place a value into a Sudoku cell
  -----------------------------------------*/

void PlaceValue(HWND hDlg, int row, int col, int value)
    {
    int i, j;

    // Store the value.
    set_cell(row, col, value);
    known[row][col] = 1;

    // Was this entered as part of the original puzzle?
    orig[row][col] = (IsDlgButtonChecked(hDlg, IDC_ORIG) == BST_CHECKED);

    // Big numbers are painted, not buttons.
    if (IsHEX)
	ShowCell(hDlg, row, col);
    else
	HideCell(hDlg, row, col);
    // Remove the candidates from this cell.
    HideAllCandidates(hDlg, row, col);

    // Remove this value from the rest of the row and column.
    for (i = 0; i < ArraySize; i++)
	{
	HideCandidate(hDlg, i, col, value);
	HideCandidate(hDlg, row, i, value);
	}

    // Remove this value from the remainder of the square.
    for (i = 0; i < SquareSize; i++)
	{
	int ButtonLocation = (row / SquareSize) * SquareSize + i;

	for (j = 0; j < SquareSize; j++)
	    {
	    HideCandidate(hDlg,	ButtonLocation,	j + (col / SquareSize) * SquareSize, value);
	    }
	}

    // Repaint the board.
    InvalidateRect(hDlg, NULL, FALSE);
    UpdateWindow(hDlg);
    }

/*-----------------------------------------
    Place a hex value into a Sudoku cell
  -----------------------------------------*/

void PaintHexCandidates(HWND hDlg, HDC hdc, int row, int col)
    {
    RECT rc;

    if (!GetBigCellRect(hDlg, row, col, &rc))
	return;

    int CellWidth = (rc.right - rc.left) / 4;
    int CellHeight = (rc.bottom - rc.top) / 4;

    char text[2];

    for (int value = 1; value <= 16; value++)
	{
	if (!is_available(row, col, value))
	    continue;

	int r = (value - 1) / 4;
	int c = (value - 1) % 4;

	sprintf_s(text, sizeof(text), "%X", value - 1);
	InsertText(hdc, text, rc.left + c * CellWidth + 2, rc.top + r * CellHeight, CellHeight, RGB(0, 0, 0), RGB(240, 240, 240));
	}
    }

/*-----------------------------------------
    Repaint one Sudoku cell
  -----------------------------------------*/

void RefreshCell(HWND hDlg, int row, int col)
    {
    RECT rc;

    if (GetBigCellRect(hDlg, row, col, &rc))
	{
	InvalidateRect(hDlg, &rc, TRUE);
	}
    }

/*-----------------------------------------
    Paint the HEX Sudoku board

    Each button paints its own contents.

    Filled cells display one large value.

    Empty cells will later display the
    available candidates.
  -----------------------------------------*/

void PaintHexSudokuBoard(HWND hDlg)
    {
    PAINTSTRUCT ps;
    HWND hCtrl;
    char lpText[14];

    for (int row = 0; row < ArraySize; row++)
	{
	for (int col = 0; col < ArraySize; col++)
	    {
	    hCtrl = GetDlgItem(hDlg, IDC_BUTTON1 + CellButton(row, col));
	    if (!hCtrl)
		continue;

	    BeginPaint(hCtrl, &ps);

	    if (matrix[row][col] > 0)
		{
		sprintf_s(lpText, sizeof(lpText), "%X", matrix[row][col] - 1);

		if (orig[row][col])
		    {
		    InsertText(ps.hdc, lpText, 8, 0, 40, RGB(240, 0, 0), RGB(240, 240, 240));
		    }
		else
		    {
		    InsertText(ps.hdc, lpText, 8, 0, 40, RGB(0, 0, 0), RGB(240, 240, 240));
		    }
		}
	    else
		{
		for (int j = 0; j < SquareSize; j++)
		    {
		    for (int k = 0; k < SquareSize; k++)
			{
			sprintf_s(lpText, sizeof(lpText), "%X", j * SquareSize + k);

			if (is_available(row, col, j * SquareSize + k + 1))
			    {
			    InsertText(ps.hdc, lpText, k * 9 + 2,j * 10, 10, RGB(0, 0, 0), RGB(240, 240, 240));
			    }
			}
		    }
		}
	    EndPaint(hCtrl, &ps);
	    }
	}
    }

/*-----------------------------------------
Select a Sudoku cell

9x9:
    Candidate buttons call PlaceValue()
    directly, so this routine is not used.

16x16:
    Clicking a cell either removes an
    existing value or displays the
    hexadecimal value picker.
-----------------------------------------*/

void SelectCell(HWND hDlg, int row, int col)
    {
    int value;

    // Existing value? Remove it.
    if (known[row][col])
	{
	UndoCell(hDlg, row, col);
	return;
	}

    // Ask the user for a hexadecimal value.
    value = (int)DialogBox((HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE), "ChooseHexValueDlg", hDlg, ChooseValueDlg);
    if (value <= 0)
	return;

    // Is the value legal?
    if (is_available(row, col, value))
	{
	PlaceValue(hDlg, row, col, value);
	}
    }

/*-----------------------------------------
    Toggle between original cell or user cell
  -----------------------------------------*/

void ToggleOriginal(HWND hDlg, int row, int col)
    {
    if (!known[row][col])
	return;

    orig[row][col] = !orig[row][col];

    if (IsHEX)
	{
	InvalidateRect(hDlg, NULL, TRUE);
	UpdateWindow(hDlg);
	return;
	}

    RECT rc;
    if (GetBigCellRect(hDlg, row, col, &rc))
	{
	InvalidateRect(hDlg, &rc, TRUE);
	UpdateWindow(hDlg);
	}
    }
