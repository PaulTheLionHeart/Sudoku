/*-----------------------------------------
	SUDOKU.CPP 
	Sudoku solving Program Written by Paul de Leeuw
  -----------------------------------------*/

#include "resource.h"
#include "sudoku.h"
#include "generate.h"

/*-----------------------------------------
      Sudoku.cpp

      Main application module.

      Responsible for:

	  • application startup
	  • message loop
	  • menu commands
	  • dialog creation
	  • dispatching commands to the
	    Sudoku engine and display modules.

	WinMain()
            Create the application window.

        WndProc()
            Handles menus and keyboard commands.

        DisplayDlg()
            Interactive Sudoku editor and solver.

        AboutBoxDlg()
            Version information.

        ChooseValueDlg()
            Numeric/hexadecimal value picker.
  -----------------------------------------*/

HWND	GlobalHwnd;

WNDPROC CALLBACK    WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    AboutBoxDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    DisplayDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    DisplayResultsDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    ChooseValueDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

char 	szAppName [100] = "Sudoku";
static	char 	szFileName[MAX_PATH];
static	HANDLE	hBitmap = NULL;					// opening bitmap
static	HDC	hdcMem;
static	BITMAP	bm;
static	LOGFONT	lf;

// specific Sudoku stuff
static	int	value;						// global to transcend dialogue boxes
static  int	button;						// which button has the user selected?
BOOL		IsHEX = FALSE;					// default is 9*9 Sudoku
BOOL		IsOrig = TRUE;					// Save the original puzzle
int		ArraySize, SquareSize;				// dimension for normal or hexadecimal Sudoku

PAINTSTRUCT 	ps;
RECT 		r;
										// Styles of app. window 
DWORD		dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
int		AppIndex;
static	char    *lpText ;
static	short	nNumLines;
GenerateOptions GenOptions;

/*-----------------------------------------
	Main Windows Entry Point
  -----------------------------------------*/

int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
     {
     HWND     hwnd;
     MSG      msg;
     WNDCLASS wndclass;

     if (!hPrevInstance)
	  {
	  wndclass.style	 = CS_HREDRAW | CS_VREDRAW;
	  wndclass.lpfnWndProc	 = (WNDPROC)WndProc;
	  wndclass.cbClsExtra	 = 0;
	  wndclass.cbWndExtra	 = 0;
	  wndclass.hInstance	 = hInstance;
	  wndclass.hIcon	 = LoadIcon (hInstance, szAppName);
	  wndclass.hCursor	 = LoadCursor (NULL, IDC_ARROW);
	  wndclass.hbrBackground = (HBRUSH)GetStockObject (WHITE_BRUSH);
	  wndclass.lpszMenuName  = szAppName;
	  wndclass.lpszClassName = szAppName;

	  RegisterClass (&wndclass);
	  }
     
     hwnd = CreateWindow (szAppName, "Paul's Sudoku Solver",
     			  dwStyle, 
			  160, 120, 				// nice initial location
			  500, 320, 				// nice initial size
			  NULL, NULL, hInstance, NULL);

     GlobalHwnd = hwnd;
     ShowWindow (hwnd, nCmdShow);
     UpdateWindow (hwnd);

     while (GetMessage (&msg, NULL, 0, 0))
	  {
	  TranslateMessage (&msg);
	  DispatchMessage (&msg);
	  }

     return (int)msg.wParam;
     }

/*-----------------------------------------
	Draw Opening Bitmap
  -----------------------------------------*/

void	DrawBitmap(HWND hwnd, HDC hdc, HANDLE hBitmap)
    {
    DWORD	ErrorCode;
    char	s[180];

    hdcMem = CreateCompatibleDC(hdc);
    if (SelectObject(hdcMem, hBitmap) == NULL)
	{
	ErrorCode = GetLastError();
	sprintf_s(s, sizeof(s), "SelectObject Error type = %ld", ErrorCode);
	MessageBox (hwnd, s, szAppName, MB_ICONEXCLAMATION | MB_OK);
	}
    SetMapMode(hdcMem, GetMapMode(hdc));
    if (GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm) == 0)
	{
	ErrorCode = GetLastError();
	sprintf_s(s, sizeof(s), "GetObject Error type = %ld", ErrorCode);
	MessageBox (hwnd, s, szAppName, MB_ICONEXCLAMATION | MB_OK);
	}
    if (hBitmap)
	{
	DeleteObject(hBitmap);
	hBitmap = NULL;
	}
    RECT rc = { 0, 0, bm.bmWidth, bm.bmHeight };
    AdjustWindowRect(&rc, GetWindowLong(hwnd, GWL_STYLE), TRUE);      // TRUE = account for menu
    MoveWindow(hwnd, 160, 120, rc.right - rc.left, rc.bottom - rc.top, TRUE);
    ShowWindow (hwnd, SW_SHOWNORMAL);
    InvalidateRect(hwnd, NULL, FALSE);      
    }

/*-----------------------------------------
	Let's Process a few messages
  -----------------------------------------*/

WNDPROC CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
    static HINSTANCE	hInst;
    static HCURSOR	hCursor;
    static HBRUSH	hBrush;
    static BOOL		complete, csv;
    HDC			hdc;
    DWORD		ErrorCode;
    int			result, result1;
    static HWND		hScroll ;
    PAINTSTRUCT		ps ;
    char		s[180];

    switch (message)
	{
	case WM_CREATE:
	    hInst = ((LPCREATESTRUCT)(_int64)lParam)->hInstance;
	    hdc = GetDC(hwnd);
	    hBitmap = LoadImage(hInst, MAKEINTRESOURCE(SUDOKU_BMP), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR/*LR_LOADFROMFILE*/);
	    DrawBitmap(hwnd, hdc, hBitmap);		// cute opening bitmap
	    ArraySize = (IsHEX) ? HEXARRAYSIZE : NORMALARRAYSIZE;		// dimension array indexing
	    SquareSize = (IsHEX) ? HEXSQUARESIZE : NORMALSQUARESIZE;
	    init_arrays();
	    srand((unsigned)time(NULL));
	    return 0;

	case WM_INITMENUPOPUP:

	    switch (lParam)
		{
		case 0:	   // File menu
		    EnableMenuItem ((HMENU)(_int64)wParam, IDM_OPEN, MF_ENABLED);
		    EnableMenuItem ((HMENU)(_int64)wParam, IDM_EXIT, MF_ENABLED);
		    break;

		case 1:	   // Options menu
		    EnableMenuItem ((HMENU)(_int64)wParam, IDM_SELECTSUDOKU, MF_ENABLED);
		    EnableMenuItem ((HMENU)(_int64)wParam, IDM_SELECT_MODE, MF_ENABLED);
		    break;
		}
	    return 0;

	  case WM_COMMAND:
	       switch (wParam)
		    {
		    case IDM_SELECT_MODE:    
			SetSudokuMode(MessageBox(hwnd, "Run in HEX mode (16 by 16 Sudoku)?", szAppName,	MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDYES);
			return 0;

		    case IDM_GENERATESUDOKU:
			if (DialogBoxParam(hInst, "GenerateDlg", hwnd, GenerateDlg, (LPARAM)&GenOptions) == IDOK)
			    {
			    SendMessage(hwnd, WM_COMMAND, IDM_SELECTSUDOKU, 0L);
			    }
			return 0;

		    case IDM_EXIT:
			SendMessage (hwnd, WM_CLOSE, 0, 0L);
			return 0;

//		    case IDM_PRINT:
//			MessageBox (hwnd, "Printing hasn't been implemented yet!", szAppName, MB_ICONEXCLAMATION | MB_OK);
//			return 0;

		    case IDM_SELECTSUDOKU:
			result = ShowSudokuDialog(hInst, hwnd, DisplayDlg);
			switch (result)
			    {
			    // Return values :
			    // IMPASSE - Puzzle cannot be solved.
			    // REFRESH - Redisplay the current puzzle.
			    // FINISH - User finished normally.
			    // CANCEL - User cancelled the dialog.
			    // CALCULATE - solve the puzzle

			    case IMPASSE:
				MessageBox(hwnd, "Impasse reached. Sudoku is not solvable", "Sudoku", MB_ICONEXCLAMATION | MB_OK);
				return 0;
			    case REFRESH:
				SendMessage (hwnd, WM_COMMAND, IDM_SELECTSUDOKU, 0L);
				return 0;
			    case FINISH:
			    case CANCEL:
				return 0;
			    case CALCULATE:
				result1 = ShowSudokuDialog(hInst, hwnd, DisplayDlg);
				if (result1 == FINISH)
				    {
				    return 0;
				    }
				else if (result1 == REFRESH)
				    SendMessage (hwnd, WM_COMMAND, IDM_SELECTSUDOKU, 0L);
				return 0;
			    default:
				MessageBox (hwnd, "How did we get here?", szAppName, MB_ICONEXCLAMATION | MB_OK);
			    }
			DialogBox (hInst, "DisplayDlg", hwnd, (DLGPROC)DisplayDlg);
			return 0;

			      // Messages from Help menu
		    case IDM_ABOUT:
			 DialogBox (hInst, "AboutBoxDlg", hwnd, (DLGPROC)AboutBoxDlg);
			 return 0;

			 // Messages from File menu
		    case IDM_OPEN:
			 GetData(hwnd, szFileName, szAppName);
			 SendMessage (hwnd, WM_COMMAND, IDM_SELECTSUDOKU, 0L);
			 return 0;
		    }
	       break;

	  case WM_SIZE:
		InvalidateRect(hwnd, NULL, FALSE);      
		return 0;

	  case WM_PAINT:
		{
		BeginPaint(hwnd, &ps);
		if (BitBlt(ps.hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY) == 0)
		    {
		    ErrorCode = GetLastError();
		    sprintf_s(s, sizeof(s), "Error type = %ld", ErrorCode);
		    MessageBox (hwnd, s, szAppName, MB_ICONEXCLAMATION | MB_OK);
		    }
		RECT rc = { 0, 0, bm.bmWidth, bm.bmHeight };
		AdjustWindowRect(&rc, GetWindowLong(hwnd, GWL_STYLE), TRUE);      // TRUE = account for menu
		MoveWindow(hwnd, 160, 120, rc.right - rc.left, rc.bottom - rc.top, TRUE);
		ShowWindow (hwnd, SW_SHOWNORMAL);
		EndPaint(hwnd, &ps);
		return 0;
		}

        case WM_KEYDOWN:					// Handle any keyboard messages
            switch (wParam) 
                {
                case VK_F1:						// Help    
		    PostMessage (hwnd, WM_COMMAND, IDM_ABOUT,   0L);
                    break;
                case 's':						// Select Constant to display
                case 'S': 
		     SendMessage (hwnd, WM_COMMAND, IDM_SELECT_MODE, 0L);
                     break;
		case 'c':						// Select Constant to display
		case 'C':
		    SendMessage(hwnd, WM_COMMAND, IDM_SELECTSUDOKU, 0L);
		    break;
		case 'g':						// Generate new sudoku
		case 'G':
		    SendMessage(hwnd, WM_COMMAND, IDM_GENERATESUDOKU, 0L);
		    break;
		case 'o':						// open file  
                case 'O': 
		     SendMessage (hwnd, WM_COMMAND, IDM_OPEN, 0L);
                     break;
                case VK_RETURN:						// Let's get out of here
                case VK_ESCAPE: 
		    SendMessage (hwnd, WM_CLOSE, 0, 0L);
                    break;
                }
            break;

	  case WM_DESTROY :
		PostQuitMessage (0);
	       	return 0;
	  }
     return (WNDPROC)DefWindowProc (hwnd, message, wParam, lParam);
     }

/**************************************************************************
	Select Value Dialog Box
**************************************************************************/

INT_PTR CALLBACK ChooseValueDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
     {
     static     int	temp;
     static     UINT	tempParam;
     int	i;
     HWND	hCtrl;
     char	Text[4];

     switch (message)
	  {
	  case WM_INITDIALOG:
		for (i = 0; i < ArraySize; i++)
		    {
		    hCtrl = GetDlgItem (hDlg, IDC_BUTTON1 + i);
		    if (is_available(button / ArraySize, button % ArraySize , i+1))
			ShowWindow(hCtrl, SW_SHOWNORMAL);
		    else
			ShowWindow(hCtrl, SW_HIDE);
		    }
	        return FALSE ;
	  case WM_PAINT:
		for (i = 0; i < ArraySize; i++)
		    {
		    hCtrl = GetDlgItem (hDlg, IDC_BUTTON1 + i);
		    BeginPaint(hCtrl, &ps);
		    sprintf(Text, "%X", ((IsHEX) ? i : i + 1));
		    InsertText(ps.hdc, Text, 16,  0, 48, RGB(0,0,0), RGB(240,240,240));
		    EndPaint(hCtrl, &ps);
		    }
	        return FALSE ;

	  case WM_COMMAND:
	        switch ((int) LOWORD(wParam))
		    {
		    case IDC_BUTTON1:
		    case IDC_BUTTON2:
		    case IDC_BUTTON3:
		    case IDC_BUTTON4:
		    case IDC_BUTTON5:
		    case IDC_BUTTON6:
		    case IDC_BUTTON7:
		    case IDC_BUTTON8:
		    case IDC_BUTTON9:
		    case IDC_BUTTON10:
		    case IDC_BUTTON11:
		    case IDC_BUTTON12:
		    case IDC_BUTTON13:
		    case IDC_BUTTON14:
		    case IDC_BUTTON15:
		    case IDC_BUTTON16:
//			if (IsHEX)
//			    EndDialog (hDlg, LOWORD(wParam) - IDC_BUTTON1);
//			else
			    EndDialog (hDlg, LOWORD(wParam) - IDC_BUTTON1 + 1);
		        return TRUE;

		    case IDCANCEL:
			EndDialog (hDlg, FALSE);
			return FALSE;
		   }
		   break;
	    }
      return FALSE ;
      }

/**************************************************************************
	About Constant Dialog Box
**************************************************************************/

INT_PTR CALLBACK    AboutBoxDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
     {
     switch (message)
	  {
	  case WM_COMMAND:
	        switch ((int) LOWORD(wParam))
		    {
		    case IDOK:
			EndDialog (hDlg, TRUE);
			return TRUE;
		   }
		   break;
	    }
      return FALSE ;
      }

/*---------------------------------------------------------------------
	Insert Text into constant dialogue box
  -------------------------------------------------------------------*/

void	InitText(void)
    {
    lf.lfEscapement	= 0;
    lf.lfOrientation	= 0;
    lf.lfQuality	= DEFAULT_QUALITY;
    lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
    lf.lfWidth		= 0;
    lf.lfItalic		= 0;
    lf.lfUnderline	= 0;
    lf.lfStrikeOut	= 0;
    lf.lfOrientation	= 0; 
    lf.lfWeight		= FW_NORMAL; 
    }

void	InsertText(HDC hdc, char *szText, int x, int y, int size, DWORD colour, DWORD BkColour)
    {
    HFONT   font;

    SetTextColor(hdc, colour);
    SetBkColor(hdc, BkColour);
    lf.lfHeight	= size;
    font = CreateFontIndirect (&lf);
    SelectObject (hdc, font) ;
    TextOut (hdc, x, y, szText, (int)strlen(szText)) ;
    DeleteObject(font);
    }

/**************************************************************************
	Display the known numbers Dialog Box
**************************************************************************/

INT_PTR CALLBACK    DisplayDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
     {
     switch (message)
	 {
	 case WM_INITDIALOG:
	     InitialiseBoard(hDlg);
	     return FALSE;

	 case WM_PAINT:
	     {
	     PAINTSTRUCT ps;

	     BeginPaint(hDlg, &ps);
	     if (IsHEX)
		 PaintHexSudokuBoard(hDlg);
	     else
		 PaintSudokuBoard(hDlg, ps.hdc);
	     EndPaint(hDlg, &ps);

	     return TRUE;
	     }

/*-----------------------------------------
    Right-click on a HEX board cell.

    Standard button controls consume
    WM_RBUTTONDOWN, so use WM_CONTEXTMENU
    to determine which cell was clicked.
-----------------------------------------*/	 
	 case WM_RBUTTONDOWN:
	     {
	     POINT pt;
	     pt.x = (short)LOWORD(lParam);
	     pt.y = (short)HIWORD(lParam);

	     for (int row = 0; row < ArraySize; row++)
		 {
		 for (int col = 0; col < ArraySize; col++)
		     {
		     RECT rc;

		     if (GetBigCellRect(hDlg, row, col, &rc) && PtInRect(&rc, pt))
			 {
			 if (known[row][col])
			     ToggleOriginal(hDlg, row, col);

			 return TRUE;
			 }
		     }
		 }

	     return FALSE;
	     }

	 case WM_CONTEXTMENU:
	     {
	     if (!IsHEX)
		 return FALSE;

	     HWND hCtrl = (HWND)wParam;

	     for (int row = 0; row < ArraySize; row++)
		 {
		 for (int col = 0; col < ArraySize; col++)
		     {
		     if (hCtrl == GetDlgItem(hDlg, IDC_BUTTON1 + CellButton(row, col)))
			 {
			 ToggleOriginal(hDlg, row, col);
			 return TRUE;
			 }
		     }
		 }

	     return FALSE;
	     }

	 case WM_LBUTTONDOWN:
	     {
	     POINT pt;
	     pt.x = (short)LOWORD(lParam);
	     pt.y = (short)HIWORD(lParam);
	     for (int row = 0; row < ArraySize; row++)
		 {
		 for (int col = 0; col < ArraySize; col++)
		     {
		     if (matrix[row][col] > 0)
			 {
			 RECT rc;
			 if (GetBigCellRect(hDlg, row, col, &rc) && PtInRect(&rc, pt))
			     {
			     UndoCell(hDlg, row, col);
			     return TRUE;
			     }
			 }
		     }
		 }

	     return FALSE;
	     }

	 case WM_COMMAND:
	     {
	     int CommandNumber = LOWORD(wParam);

	     if (IsHEX)
		 {
		 if (CommandNumber >= IDC_BUTTON1 && CommandNumber < IDC_BUTTON1 + HEXARRAYSIZE * HEXARRAYSIZE)
		     {
		     button = CommandNumber - IDC_BUTTON1;
		     int row = button / ArraySize;
		     int col = button % ArraySize;

		     SelectCell(hDlg, row, col);
		     return TRUE;
		     }
		 }
	     else
		 {
		 if (CommandNumber >= IDC_BUTTON1 && CommandNumber <= IDC_BUTTON810) 
		     {
		     button = (CommandNumber - IDC_BUTTON1) / 10;
		     int row = button / ArraySize;
		     int col = button % ArraySize;
		     int value = (CommandNumber - IDC_BUTTON1) % 10;

		     PlaceValue(hDlg, row, col, value);
		     return TRUE;
		     }
		 }

	     switch (CommandNumber)
		 {
		 case IDCANCEL:
		     EndDialog(hDlg, CANCEL);
		     return TRUE;

		 case IDC_FINISH:
		     EndDialog(hDlg, FINISH);
		     return TRUE;

		 case IDC_REFRESH:
		     init_arrays();
		     EndDialog(hDlg, REFRESH);
		     return TRUE;

		 case IDC_SAVE_CSV:
		     SaveData(hDlg, szFileName, szAppName, TRUE);
		     return TRUE;

		 case IDC_SAVESUDOKU:
		     SaveData(hDlg, szFileName, szAppName, FALSE);
		     return TRUE;

		 case IDOK:
		 case IDC_CALCULATE:      // or IDOK if that's what the button uses
		     if (!SolvePuzzle())
			 {
			 EndDialog(hDlg, IMPASSE);
			 }
		     else
			 {
			 IsOrig = FALSE;
			 InitialiseBoard(hDlg);
			 InvalidateRect(hDlg, NULL, FALSE);
			 UpdateWindow(hDlg);
			 EndDialog(hDlg, CALCULATE);
			 }

		     return TRUE;
		 }

	     break;
	     }
	 }

     return FALSE;
    }



