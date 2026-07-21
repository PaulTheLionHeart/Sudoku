/*-----------------------------------------
	GENERATEDLG.CPP 
	Sudoku creating Program Written by Paul de Leeuw
  -----------------------------------------*/

#include "resource.h"
#include "generate.h"
#include "sudoku.h"

/*-----------------------------------------
      Enabling helper
  -----------------------------------------*/

static void EnableGenerateControls(HWND hDlg, BOOL Enable)
    {
    EnableWindow(GetDlgItem(hDlg, IDC_NORMAL), Enable);
    EnableWindow(GetDlgItem(hDlg, IDC_HEX), Enable);

    EnableWindow(GetDlgItem(hDlg, IDC_EASY), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_MEDIUM), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_HARD), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_EXPERT), FALSE);

    EnableWindow(GetDlgItem(hDlg, IDC_ROTATION_SYM), Enable);
    EnableWindow(GetDlgItem(hDlg, IDC_MIRROR), Enable);
    EnableWindow(GetDlgItem(hDlg, IDC_RANDOM), Enable);

    EnableWindow(GetDlgItem(hDlg, IDOK), Enable);
    EnableWindow(GetDlgItem(hDlg, IDCANCEL), Enable);
    }

/*-----------------------------------------
      Show generation progress
  -----------------------------------------*/

void SetProgress(HWND hDlg, int Position)
    {
    SendDlgItemMessage(hDlg, IDC_PROGRESS_BAR, PBM_SETPOS, Position, 0);
    }

void SetProgressRange(HWND hDlg, int Maximum)
    {
    SendDlgItemMessage(hDlg, IDC_PROGRESS_BAR, PBM_SETRANGE, 0, MAKELPARAM(0, Maximum));
    }

/*-----------------------------------------
    Create New Puzzle dialog
-----------------------------------------*/

INT_PTR CALLBACK GenerateDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
    GenerateOptions *GenOptions;

    switch (message)
	{
	case WM_INITDIALOG:				// Initialise the default options.
	    GenOptions = (GenerateOptions *)lParam;
	    if (GenOptions == NULL)
		{
		EndDialog(hDlg, IDCANCEL);
		return FALSE;
		}

	    SetDlgItemText(hDlg, IDC_GENERATE_STATUS, "");
	    ShowWindow(GetDlgItem(hDlg, IDC_PROGRESS_BAR), SW_SHOW);
	    EnableGenerateControls(hDlg, TRUE);

	    SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)GenOptions);
	    CheckRadioButton(hDlg, IDC_NORMAL, IDC_HEX, GenOptions->IsHex ? IDC_HEX : IDC_NORMAL);
	    switch (GenOptions->Difficulty)
		{
		case EASY:
		    CheckRadioButton(hDlg, IDC_EASY, IDC_EXPERT, IDC_EASY);
		    break;

		case MEDIUM:
		    CheckRadioButton(hDlg, IDC_EASY, IDC_EXPERT, IDC_MEDIUM);
		    break;

		case HARD:
		    CheckRadioButton(hDlg, IDC_EASY, IDC_EXPERT, IDC_HARD);
		    break;

		case EXPERT:
		    CheckRadioButton(hDlg, IDC_EASY, IDC_EXPERT, IDC_EXPERT);
		    break;
		}

	    switch (GenOptions->Symmetry)
		{
		case ROTATIONAL: CheckRadioButton(hDlg, IDC_ROTATION_SYM, IDC_RANDOM, IDC_ROTATION_SYM);
		    break;

		case MIRROR:
		    CheckRadioButton(hDlg, IDC_ROTATION_SYM, IDC_RANDOM, IDC_MIRROR);
		    break;

		default:
		    CheckRadioButton(hDlg, IDC_ROTATION_SYM, IDC_RANDOM, IDC_RANDOM);
		    break;
		}
/*
	    {
	    char Buffer[128];
	    LONG Style;

	    Style = GetWindowLong(GetDlgItem(hDlg, IDC_NORMAL), GWL_STYLE);
	    _snprintf_s(Buffer, sizeof(Buffer), _TRUNCATE,
		"IDC_NORMAL WS_GROUP=%s Style=%08lX\n",
		(Style & WS_GROUP) ? "YES" : "NO",
		Style);
	    OutputDebugStringA(Buffer);

	    Style = GetWindowLong(GetDlgItem(hDlg, IDC_EASY), GWL_STYLE);
	    _snprintf_s(Buffer, sizeof(Buffer), _TRUNCATE,
		"IDC_EASY WS_GROUP=%s Style=%08lX\n",
		(Style & WS_GROUP) ? "YES" : "NO",
		Style);
	    OutputDebugStringA(Buffer);

	    Style = GetWindowLong(GetDlgItem(hDlg, IDC_ROTATION_SYM), GWL_STYLE);
	    _snprintf_s(Buffer, sizeof(Buffer), _TRUNCATE,
		"IDC_ROTATION_SYM WS_GROUP=%s Style=%08lX\n",
		(Style & WS_GROUP) ? "YES" : "NO",
		Style);
	    OutputDebugStringA(Buffer);
	    }
*/

	    return TRUE;

	case WM_COMMAND:
	    {
	    int CommandNumber = LOWORD(wParam);

	    switch (CommandNumber)
	    {
	    case IDOK:
		{
		GenOptions = (GenerateOptions *)GetWindowLongPtr(hDlg, GWLP_USERDATA);
		GenOptions->IsHex = (IsDlgButtonChecked(hDlg, IDC_HEX) == BST_CHECKED);
		if (IsDlgButtonChecked(hDlg, IDC_EASY))
		    GenOptions->Difficulty = EASY;
		else if (IsDlgButtonChecked(hDlg, IDC_MEDIUM))
		    GenOptions->Difficulty = MEDIUM;
		else if (IsDlgButtonChecked(hDlg, IDC_HARD))
		    GenOptions->Difficulty = HARD;
		else
		    GenOptions->Difficulty = EXPERT;
		if (IsDlgButtonChecked(hDlg, IDC_ROTATION_SYM))
		    GenOptions->Symmetry = ROTATIONAL;
		else if (IsDlgButtonChecked(hDlg, IDC_MIRROR))
		    GenOptions->Symmetry = MIRROR;
		else
		    GenOptions->Symmetry = RANDOM;

		ShowWindow(GetDlgItem(hDlg, IDC_PROGRESS_BAR), SW_SHOW);
		SendDlgItemMessage(hDlg, IDC_PROGRESS_BAR, PBM_SETRANGE, 0, MAKELPARAM(0, ArraySize * ArraySize));

		SetProgress(hDlg, 0);
		SetDlgItemText(hDlg, IDC_GENERATE_STATUS, "Generating Sudoku...");
		EnableGenerateControls(hDlg, FALSE);
		GeneratePuzzle(hDlg, *GenOptions);
		EndDialog(hDlg, IDOK);
		return TRUE;
		}
	    case IDCANCEL:

		EndDialog(hDlg, IDCANCEL);
		return TRUE;
	    }

	    break;
	    }
	}

	return FALSE;
    }


