/*-----------------------------------------
	SUDOKU.CPP 
	Sudoku solving Program Written by Paul de Leeuw
  -----------------------------------------*/

#include "resource.h"
#include "sudoku.h"

//extern	void InitializeScrollBars(HWND, RECT *, RECT *);

//extern	void InitText(void);
//extern	int  SaveData(HWND hwnd, char *szFileName, char *szAppName, char *lpText, char *Time, char *Name);

// stuff from SimpleSudoku.cpp
extern	void	init_known(void);
extern	void	init_arrays(void);
extern	int	solve_sudoku(void);
extern	bool	is_available(int i, int j, int n);
extern	void	set_cell(int i, int j, int n);
extern	int	clear_cell(int i, int j);
extern	int	SaveData(HWND hwnd, char *szFileName, char *szAppName, BOOL csv);
extern	int	GetData(HWND hwnd, char *szFileName, char *szAppName);
extern	int	square(int i, int j);

HWND	GlobalHwnd;

//extern	void	print_matrix(void);
extern	int	known[HEXARRAYSIZE][HEXARRAYSIZE];		// dimension for larger array if required
extern	int	matrix[HEXARRAYSIZE][HEXARRAYSIZE];

/* An array of nine integers, each of which representing a sub-square.
Each integer has its nth-bit on iff n belongs to the corresponding sub-square. */
extern	int squares[HEXARRAYSIZE];

/* An array of nine integers, each of which representing a row.
Each integer has its nth-bit on iff n belongs to the corresponding row. */
extern	int rows[HEXARRAYSIZE];

/* An array of nine integers, each of which representing a column.
Each integer has its nth-bit on iff n belongs to the corresponding column. */
extern	int cols[HEXARRAYSIZE];

/* An array with some powers of 2 to avoid shifting all the time. */
extern	int bits[HEXARRAYSIZE + 1];


// Which numbers were given in the original problem?
extern	int	orig[HEXARRAYSIZE][HEXARRAYSIZE];

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
//DWORD		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
int		AppIndex;
static	char    *lpText ;
static	short	nNumLines;

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
//	  wndclass.style	 = CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS;
	  wndclass.lpfnWndProc	 = (WNDPROC)WndProc;
	  wndclass.cbClsExtra	 = 0;
	  wndclass.cbWndExtra	 = 0;
	  wndclass.hInstance	 = hInstance;
//	  wndclass.hIcon	 = LoadIcon (NULL, IDI_APPLICATION);
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
			  500, 300, 				// nice initial size
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

// Okay, here is the basic Sudoku code

/*-----------------------------------------
	Just a quicky
  -----------------------------------------*/

void DoCaption (HWND hwnd, char *szTitleName)
     {
     char szCaption [255];

     sprintf_s(szCaption, sizeof(szCaption), "%s", szTitleName);
     SetWindowText (hwnd, szCaption);
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
    MoveWindow (hwnd, 160, 120, bm.bmWidth, bm.bmHeight, TRUE);
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
    int			row, col;
//    int			row, col, i, j, count, val;
//     char	    *p;

    char		s[180];
//    static char    szSaveFileName  [480];

    switch (message)
	{
	case WM_CREATE:
//	    hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
	    hInst = ((LPCREATESTRUCT)(_int64)lParam)->hInstance;
	    hdc = GetDC(hwnd);
	    hBitmap = LoadImage(hInst, MAKEINTRESOURCE(SUDOKU_BMP), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR/*LR_LOADFROMFILE*/);
	    DrawBitmap(hwnd, hdc, hBitmap);		// cute opening bitmap

//	    IsHEX = TRUE;



	    ArraySize = (IsHEX) ? HEXARRAYSIZE : NORMALARRAYSIZE;		// dimension array indexing
	    SquareSize = (IsHEX) ? HEXSQUARESIZE : NORMALSQUARESIZE;
	    init_arrays();

//	    Setup();					// initialise all Sudoku variables
//	    InvalidateRect(hwnd, NULL, FALSE);      
//	    SendMessage (hwnd, WM_COMMAND, IDM_SELECTSUDOKU, 0L);
	    return 0;

	case WM_INITMENUPOPUP:

	    switch (lParam)
		{
		case 0:	   // File menu
		    EnableMenuItem ((HMENU)(_int64)wParam, IDM_OPEN, MF_ENABLED);
//		    EnableMenuItem ((HMENU)(_int64)wParam, IDM_SAVEAS, MF_ENABLED);
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
			IsHEX = (MessageBox (hwnd, "Run in HEX mode (16 by 16 Sudoku)?", szAppName, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDYES);
			ArraySize = (IsHEX) ? HEXARRAYSIZE : NORMALARRAYSIZE;		// dimension array indexing
			SquareSize = (IsHEX) ? HEXSQUARESIZE : NORMALSQUARESIZE;
			init_arrays();
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
			    case IMPASSE:
				MessageBox(hwnd, "Impasse reached. Sudoku is not solvable", "Sudoku", MB_ICONEXCLAMATION | MB_OK);
				return 0;
			    case REFRESH:
				SendMessage (hwnd, WM_COMMAND, IDM_SELECTSUDOKU, 0L);
				return 0;
			    case FINISH:
				return 0;
			    case CALCULATE:
//				result1 = DialogBox (hInst, "DisplayDlg", hwnd, DisplayResultsDlg);
				result1 = ShowSudokuDialog(hInst, hwnd, DisplayResultsDlg);
				if (result1 == FINISH)
				    {
				    return 0;
				    }
				else if (result1 == REFRESH)
				    SendMessage (hwnd, WM_COMMAND, IDM_SELECTSUDOKU, 0L);
				return 0;
			    case CHOOSEVALUE:
				row = button / ArraySize;
				col = button % ArraySize;
				if (known[row][col] == 1)		    // undo
				    {
				    clear_cell(row, col);
				    known[row][col] = 0;
				    }
				else
				    {
				    value = (int)((IsHEX) ? DialogBox (hInst, "ChooseHexValueDlg", hwnd, (DLGPROC)ChooseValueDlg) : DialogBox (hInst, "ChooseValueDlg", hwnd, (DLGPROC)ChooseValueDlg));
				    if (is_available(row, col , value))
    //				    if (is_available(row, col , value - 1))
					{
					set_cell(row, col, value);
					known[row][col] = 1;
					}
				    }
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

		BeginPaint(hwnd, &ps);
		if (BitBlt(ps.hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY) == 0)
		    {
		    ErrorCode = GetLastError();
		    sprintf_s(s, sizeof(s), "Error type = %ld", ErrorCode);
		    MessageBox (hwnd, s, szAppName, MB_ICONEXCLAMATION | MB_OK);
		    }
		MoveWindow (hwnd, 160, 120, bm.bmWidth, bm.bmHeight, TRUE);
		ShowWindow (hwnd, SW_SHOWNORMAL);
		EndPaint(hwnd, &ps);
		return 0;

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
		     SendMessage (hwnd, WM_COMMAND, IDM_SELECTSUDOKU, 0L);
                     break;
/*
                case 'e':						// Edit Appointment
                case 'E': 
		     SendMessage (hwnd, WM_COMMAND, IDM_ADD_APPOINTMENT, 0L);
                     break;
*/
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
//		    if (PossibleValues[button / 9][button % 9][i])
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

void	SetWindowState(HWND hDlg, int address, int state)
    {
    HWND hButton;

    hButton = GetDlgItem(hDlg, IDC_BUTTON1 + address);
    ShowWindow(hButton, state);
    }

INT_PTR CALLBACK    DisplayDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
     {
     RECT	    rect, DLGRect;
     int	    VertPos, HorPos, CommandNumber;
     static short   nPosition, cxChar, cyChar, cyClient, xScroll, maxrows;
     HWND	    hCtrl;
     int	    row, col;
     int	    i, j, k;
     char	    s[240];

     switch (message)
	  {
	  case WM_INITDIALOG:
		hCtrl = GetDlgItem (hDlg, IDC_SAVESUDOKU);
		GetWindowRect(hCtrl, &rect);
		GetWindowRect(hDlg, &DLGRect);
		VertPos = rect.top - DLGRect.top - RHEIGHT(rect) - 1;
		MoveWindow (hCtrl, ((IsHEX) ? 400 : 330), VertPos, RWIDTH(rect), RHEIGHT(rect), TRUE);
		ShowWindow (hCtrl, SW_SHOWNORMAL);

		hCtrl = GetDlgItem (hDlg, IDOK);
		MoveWindow (hCtrl, ((IsHEX) ? 250 : 180), VertPos, RWIDTH(rect), RHEIGHT(rect), TRUE);
		ShowWindow (hCtrl, SW_SHOWNORMAL);
		hCtrl = GetDlgItem (hDlg, IDC_ORIG);
		SendMessage(hCtrl, BM_SETCHECK, IsOrig, 0L);
		hCtrl = GetDlgItem (hDlg, IDC_SAVE_CSV);
		ShowWindow(hCtrl, SW_HIDE);

		// set the remaining buttons to the same vertical position
		hCtrl = GetDlgItem(hDlg, IDC_REFRESH);
		GetWindowRect(hCtrl, &rect);
		HorPos = rect.left - DLGRect.left;
		MoveWindow(hCtrl, HorPos, VertPos, RWIDTH(rect), RHEIGHT(rect), TRUE);
		ShowWindow(hCtrl, SW_SHOWNORMAL);

		hCtrl = GetDlgItem(hDlg, IDC_FINISH);
		GetWindowRect(hCtrl, &rect);
		HorPos = rect.left - DLGRect.left;
		MoveWindow(hCtrl, HorPos, VertPos, RWIDTH(rect), RHEIGHT(rect), TRUE);
		ShowWindow(hCtrl, SW_SHOWNORMAL);
		for (i = 0; i < ArraySize * ArraySize; i++)
		    {
		    row = i / ArraySize;
		    col = i % ArraySize;
		    value = matrix[i / ArraySize][i % ArraySize];
		    if (value)
			{
			HDC	hdc;
			char	lpText[14];

			for (j = 0; j < ArraySize; j++)
			    {
//			    if ((cols[i] & bits[value]) == 0)
				SetWindowState(hDlg, (col + j * ArraySize) * 10 + value, SW_HIDE);
//			    if ((rows[i] & bits[value]) == 0)
				SetWindowState(hDlg, (row * ArraySize + j) * 10 + value, SW_HIDE);
			    }
			for (j = 0; j < SquareSize; j++)
			    {
			    int ButtonLocation = (row / SquareSize) * SquareSize + j;
			    for (k = 0; k < SquareSize; k++)
				{
//				if ((squares[square(i, j)] & bits[value]) == 0)
				    SetWindowState(hDlg, (ButtonLocation * ArraySize + k + (col / SquareSize) * SquareSize) * 10 + value, SW_HIDE);
				}
			    }
			SetWindowState(hDlg, i * 10, SW_SHOWNORMAL);
			hCtrl = GetDlgItem (hDlg, i * 10 + IDC_BUTTON1);
			hdc = GetDC(hCtrl);
			if (IsHEX)
			    sprintf(lpText, "%X", matrix[i / ArraySize][i % ArraySize] - 1);
			else
			    sprintf(lpText, "%d", matrix[i / ArraySize][i % ArraySize]);
			if (orig[i / ArraySize][i % ArraySize] > 0)
			    InsertText(hdc, lpText, ((IsHEX) ? 8 : 16), 0, ((IsHEX) ? 40 : 48), RGB(240,0,0), RGB(240,240,240));
			else
			    InsertText(hdc, lpText, ((IsHEX) ? 8 : 16), 0, ((IsHEX) ? 40 : 48), RGB(0,0,0), RGB(240,240,240));
			ReleaseDC(hCtrl, hdc);
			HideAllCandidates(hDlg, row, col);			// remove the little buttonsShowCell
//			for (k = 0; k < SquareSize * SquareSize; k++)
//			    SetWindowState(hDlg, i * 10 + 1 + k, SW_HIDE);
			}
		    else
//			{
			SetWindowState(hDlg, i * 10, SW_HIDE);
//			for (k = 0; k < SquareSize * SquareSize; k++)
//			    SetWindowState(hDlg, i * 10 + 1 + k, SW_SHOWNORMAL);
//			}
		    }
		for (i = 0; i < ArraySize * ArraySize; i++)
		    SetWindowState(hDlg, i * 10, SW_HIDE);
	        return FALSE ;

	  case WM_PAINT:
	        {
		PAINTSTRUCT 	ps;
//		int		i, j, k;
		char		lpText[14];
		HDC		hdc;

		BeginPaint(hDlg, &ps);
		for (i = 0; i < ArraySize * ArraySize; i++)
		    {
		    hCtrl = GetDlgItem (hDlg, i * 10 + IDC_BUTTON1);
		    hdc = GetDC(hCtrl);

//		    BeginPaint(hCtrl, &ps);
	    	    if (matrix[i / ArraySize][i % ArraySize] > 0)		// 0 means no value set or found
			{
//			ShowWindow(hCtrl, SW_SHOWNORMAL);			// show the main button
//			for (k = 0; k < SquareSize * SquareSize; k++)
//			    {
//			    hLittleCtrl = GetDlgItem(hDlg, (i * 10 + IDC_BUTTON1 + 1 + k));
//			    ShowWindow(hLittleCtrl, SW_HIDE);			// hide the little buttons
//			    }
			if (IsHEX)
			    sprintf(lpText, "%X", matrix[i / ArraySize][i % ArraySize] - 1);
			else
			    sprintf(lpText, "%d", matrix[i / ArraySize][i % ArraySize]);
			if (orig[i / ArraySize][i % ArraySize] > 0)
//			if (known[i / ArraySize][i % ArraySize] > 0)
			    InsertText(hdc, lpText, ((IsHEX) ? 8 : 16), 0, ((IsHEX) ? 40 : 48), RGB(240,0,0), RGB(240,240,240));
			else
			    InsertText(hdc, lpText, ((IsHEX) ? 8 : 16), 0, ((IsHEX) ? 40 : 48), RGB(0,0,0), RGB(240,240,240));
			}
		    else
			{
//			ShowWindow(hCtrl, SW_HIDE);						// hide the main button
//			for (j = 0; j < SquareSize; j++)
//			    {
//			    for (k = 0; k < SquareSize; k++)
//				{
//				hLittleCtrl = GetDlgItem(hDlg, (i * 10 + IDC_BUTTON1 + 1 + j * SquareSize + k));
//				if (IsHEX)
//				    sprintf(lpText, "%X", j * SquareSize + k);
//				else
//				    ShowWindow(hLittleCtrl, SW_SHOWNORMAL);			// show the little buttons
//				    sprintf(lpText, "%d", j * SquareSize + k + 1);
//				    {
//				if (is_available(i / ArraySize, i % ArraySize , j * SquareSize + k + 1))
//				    if (PossibleValues[i / 9][i % 9][j * 3 + k])		// 1 means remaining possible values
//				    {
//				    if (IsHEX)
//					InsertText(ps.hdc, lpText, k * 9 + 2,  j * 10, 10, RGB(0,0,0), RGB(240,240,240));
//				    else
//					{
//					hLittleCtrl = GetDlgItem(hDlg, (i * 10 + IDC_BUTTON1 + 1 + k));
//					ShowWindow(hLittleCtrl, SW_SHOWNORMAL);			// show the little buttons
//					}
//					InsertText(ps.hdc, lpText, k * 16 + 4,  j * 16, 16, RGB(0,0,0), RGB(240,240,240));
//					}
//				    }
//				else
//				    ShowWindow(hLittleCtrl, SW_HIDE);			// show the little buttons
//				}
//			    }
			}
		    ReleaseDC(hCtrl, hdc);
//		    EndPaint(hCtrl, &ps);
		    }
		}
		EndPaint(hDlg, &ps);
	        return TRUE ;


	  case WM_DESTROY :
		EndDialog (hDlg, TRUE);
		return FALSE;

	  case WM_COMMAND:
	        CommandNumber = (int) LOWORD(wParam);
		if (CommandNumber >= IDC_BUTTON1 && CommandNumber <= IDC_BUTTON810)
		    {
		    button = (CommandNumber - IDC_BUTTON1) / 10;
		    row = button / ArraySize;
		    col = button % ArraySize;
		    if (known[row][col] == 1)		    // undo
			{
			value = matrix[row][col];
			clear_cell(row, col);
			known[row][col] = 0;
//	sprintf (s, "button=%d, [%d][%d], value = %d", button, row, col, value) ;
//	MessageBox(GlobalHwnd, s, "undo button press Sudoku", MB_ICONEXCLAMATION | MB_OK);
			SetWindowState(hDlg, button * 10, SW_HIDE);
			for (k = 0; k < SquareSize * SquareSize; k++)
			    SetWindowState(hDlg, button * 10 + 1 + k, SW_SHOWNORMAL);
			for (i = 0; i < ArraySize; i++)
			    {
			    if (known[i][col] == 0)
//			    if ((cols[i] & bits[value]) == 0)
				SetWindowState(hDlg, (col + i * ArraySize) * 10 + value, SW_SHOWNORMAL);
			    if (known[row][i] == 0)
//			    if ((rows[i] & bits[value]) == 0)
				SetWindowState(hDlg, (row * ArraySize + i) * 10 + value, SW_SHOWNORMAL);
			    }
			for (i = 0; i < SquareSize; i++)
			    {
			    int ButtonLocation = (row / SquareSize) * SquareSize + i;
			    for (j = 0; j < SquareSize; j++)
				{
//				if ((squares[square(i, j)] & bits[value]) == 0)
				if (known[i][j] == 0)
				    SetWindowState(hDlg, (ButtonLocation * ArraySize + j + (col / SquareSize) * SquareSize) * 10 + value, SW_SHOWNORMAL);
				}
			    }
			}
		    else
			{
			value = (CommandNumber - IDC_BUTTON1) % 10;
//			if (is_available(row, col , value))
			    {
			    set_cell(row, col, value);
			    known[row][col] = 1;
			    SetWindowState(hDlg, button * 10, SW_SHOWNORMAL);
			    HideAllCandidates(hDlg, row, col);				// remove the little buttons
//			    for (k = 0; k < SquareSize * SquareSize; k++)
//				SetWindowState(hDlg, button * 10 + 1 + k, SW_HIDE);	// remove the little buttons
			    for (i = 0; i < ArraySize; i++)
				{
				SetWindowState(hDlg, (col + i * ArraySize) * 10 + value, SW_HIDE);	// remove the little buttons
				SetWindowState(hDlg, (row * ArraySize + i) * 10 + value, SW_HIDE);	// remove the little buttons
				}
			    for (i = 0; i < SquareSize; i++)
				{
				int ButtonLocation = (row / SquareSize) * SquareSize + i;
				for (j = 0; j < SquareSize; j++)
				    SetWindowState(hDlg, (ButtonLocation * ArraySize + j + (col / SquareSize) * SquareSize) * 10 + value, SW_HIDE);	// remove the little buttons
				}
			    sprintf (s, "button=%d, [%d][%d], value = %d, known = %d", button, row, col, value, known[row][col]) ;
			    DoCaption(GlobalHwnd, s);
			    }
//			else
//			    DoCaption(GlobalHwnd, "Not available");
			}
//		    InvalidateRect(hDlg, NULL, FALSE);      
//		    SendMessage (hDlg, WM_COMMAND, IDM_SELECTSUDOKU, 0L);
//		    EndDialog (hDlg, CHOOSEVALUE);
		    return TRUE;
		    }
	        switch ((int) LOWORD(wParam))
		    {
		    case IDOK:		    // this means calculate
//			init_known();
			if (solve_sudoku() < 0)
			    EndDialog (hDlg, IMPASSE);
//			print_matrix();
//			InvalidateRect(hDlg, NULL, FALSE);
			else
			    EndDialog (hDlg, CALCULATE);
			IsOrig = FALSE;
			return TRUE;
		    case IDCANCEL:
		    case IDC_FINISH:
			EndDialog (hDlg, FINISH);
			return TRUE;
		    case IDC_REFRESH:
			init_arrays();
//			InvalidateRect(hDlg, NULL, FALSE);      
			EndDialog (hDlg, REFRESH);
			return TRUE;
		    case IDC_SAVESUDOKU:
			SaveData(hDlg, szFileName, szAppName, FALSE);
			return TRUE;
/*
		    case IDC_BUTTON1:	case IDC_BUTTON2:   case IDC_BUTTON3:	case IDC_BUTTON4:   case IDC_BUTTON5:	case IDC_BUTTON6:   case IDC_BUTTON7:	case IDC_BUTTON8:
		    case IDC_BUTTON9:	case IDC_BUTTON10:  case IDC_BUTTON11:	case IDC_BUTTON12:  case IDC_BUTTON13:  case IDC_BUTTON14:  case IDC_BUTTON15:	case IDC_BUTTON16:
		    case IDC_BUTTON17:	case IDC_BUTTON18:  case IDC_BUTTON19:	case IDC_BUTTON20:  case IDC_BUTTON21:  case IDC_BUTTON22:  case IDC_BUTTON23:	case IDC_BUTTON24:  
		    case IDC_BUTTON25:	case IDC_BUTTON26:  case IDC_BUTTON27:	case IDC_BUTTON28:  case IDC_BUTTON29:	case IDC_BUTTON30:  case IDC_BUTTON31:	case IDC_BUTTON32:  
		    case IDC_BUTTON33:	case IDC_BUTTON34:  case IDC_BUTTON35:	case IDC_BUTTON36:  case IDC_BUTTON37:	case IDC_BUTTON38:  case IDC_BUTTON39:	case IDC_BUTTON40:
		    case IDC_BUTTON41:	case IDC_BUTTON42:  case IDC_BUTTON43:	case IDC_BUTTON44:  case IDC_BUTTON45:	case IDC_BUTTON46:  case IDC_BUTTON47:	case IDC_BUTTON48:
		    case IDC_BUTTON49:	case IDC_BUTTON50:  case IDC_BUTTON51:	case IDC_BUTTON52:  case IDC_BUTTON53:	case IDC_BUTTON54:  case IDC_BUTTON55:	case IDC_BUTTON56:
		    case IDC_BUTTON57:	case IDC_BUTTON58:  case IDC_BUTTON59:	case IDC_BUTTON60:  case IDC_BUTTON61:	case IDC_BUTTON62:  case IDC_BUTTON63:	case IDC_BUTTON64:
		    case IDC_BUTTON65:	case IDC_BUTTON66:  case IDC_BUTTON67:	case IDC_BUTTON68:  case IDC_BUTTON69:	case IDC_BUTTON70:  case IDC_BUTTON71:	case IDC_BUTTON72:
		    case IDC_BUTTON73:	case IDC_BUTTON74:  case IDC_BUTTON75:	case IDC_BUTTON76:  case IDC_BUTTON77:	case IDC_BUTTON78:  case IDC_BUTTON79:	case IDC_BUTTON80:
		    case IDC_BUTTON81:	case IDC_BUTTON82:  case IDC_BUTTON83:	case IDC_BUTTON84:  case IDC_BUTTON85:	case IDC_BUTTON86:  case IDC_BUTTON87:	case IDC_BUTTON88:
		    case IDC_BUTTON89:	case IDC_BUTTON90:  case IDC_BUTTON91:	case IDC_BUTTON92:  case IDC_BUTTON93:	case IDC_BUTTON94:  case IDC_BUTTON95:	case IDC_BUTTON96:
		    case IDC_BUTTON97:	case IDC_BUTTON98:  case IDC_BUTTON99:	case IDC_BUTTON100:
		    case IDC_BUTTON101:	case IDC_BUTTON102: case IDC_BUTTON103:	case IDC_BUTTON104: case IDC_BUTTON105:	case IDC_BUTTON106: case IDC_BUTTON107:	case IDC_BUTTON108:
		    case IDC_BUTTON109:	case IDC_BUTTON110: case IDC_BUTTON111:	case IDC_BUTTON112: case IDC_BUTTON113: case IDC_BUTTON114: case IDC_BUTTON115:	case IDC_BUTTON116:
		    case IDC_BUTTON117:	case IDC_BUTTON118: case IDC_BUTTON119:	case IDC_BUTTON120: case IDC_BUTTON121: case IDC_BUTTON122: case IDC_BUTTON123:	case IDC_BUTTON124:  
		    case IDC_BUTTON125:	case IDC_BUTTON126: case IDC_BUTTON127:	case IDC_BUTTON128: case IDC_BUTTON129:	case IDC_BUTTON130: case IDC_BUTTON131:	case IDC_BUTTON132:  
		    case IDC_BUTTON133:	case IDC_BUTTON134: case IDC_BUTTON135:	case IDC_BUTTON136: case IDC_BUTTON137:	case IDC_BUTTON138: case IDC_BUTTON139:	case IDC_BUTTON140:
		    case IDC_BUTTON141:	case IDC_BUTTON142: case IDC_BUTTON143:	case IDC_BUTTON144: case IDC_BUTTON145:	case IDC_BUTTON146: case IDC_BUTTON147:	case IDC_BUTTON148:
		    case IDC_BUTTON149:	case IDC_BUTTON150: case IDC_BUTTON151:	case IDC_BUTTON152: case IDC_BUTTON153:	case IDC_BUTTON154: case IDC_BUTTON155:	case IDC_BUTTON156:
		    case IDC_BUTTON157:	case IDC_BUTTON158: case IDC_BUTTON159:	case IDC_BUTTON160: case IDC_BUTTON161:	case IDC_BUTTON162: case IDC_BUTTON163:	case IDC_BUTTON164:
		    case IDC_BUTTON165:	case IDC_BUTTON166: case IDC_BUTTON167:	case IDC_BUTTON168: case IDC_BUTTON169:	case IDC_BUTTON170: case IDC_BUTTON171:	case IDC_BUTTON172:
		    case IDC_BUTTON173:	case IDC_BUTTON174: case IDC_BUTTON175:	case IDC_BUTTON176: case IDC_BUTTON177:	case IDC_BUTTON178: case IDC_BUTTON179:	case IDC_BUTTON180:
		    case IDC_BUTTON181:	case IDC_BUTTON182: case IDC_BUTTON183:	case IDC_BUTTON184: case IDC_BUTTON185:	case IDC_BUTTON186: case IDC_BUTTON187:	case IDC_BUTTON188:
		    case IDC_BUTTON189:	case IDC_BUTTON190: case IDC_BUTTON191:	case IDC_BUTTON192: case IDC_BUTTON193:	case IDC_BUTTON194: case IDC_BUTTON195:	case IDC_BUTTON196:
		    case IDC_BUTTON197:	case IDC_BUTTON198: case IDC_BUTTON199:	case IDC_BUTTON200:
		    case IDC_BUTTON201:	case IDC_BUTTON202: case IDC_BUTTON203:	case IDC_BUTTON204: case IDC_BUTTON205:	case IDC_BUTTON206: case IDC_BUTTON207:	case IDC_BUTTON208:
		    case IDC_BUTTON209:	case IDC_BUTTON210: case IDC_BUTTON211:	case IDC_BUTTON212: case IDC_BUTTON213: case IDC_BUTTON214: case IDC_BUTTON215:	case IDC_BUTTON216:
		    case IDC_BUTTON217:	case IDC_BUTTON218: case IDC_BUTTON219:	case IDC_BUTTON220: case IDC_BUTTON221: case IDC_BUTTON222: case IDC_BUTTON223:	case IDC_BUTTON224:  
		    case IDC_BUTTON225:	case IDC_BUTTON226: case IDC_BUTTON227:	case IDC_BUTTON228: case IDC_BUTTON229:	case IDC_BUTTON230: case IDC_BUTTON231:	case IDC_BUTTON232:  
		    case IDC_BUTTON233:	case IDC_BUTTON234: case IDC_BUTTON235:	case IDC_BUTTON236: case IDC_BUTTON237:	case IDC_BUTTON238: case IDC_BUTTON239:	case IDC_BUTTON240:
		    case IDC_BUTTON241:	case IDC_BUTTON242: case IDC_BUTTON243:	case IDC_BUTTON244: case IDC_BUTTON245:	case IDC_BUTTON246: case IDC_BUTTON247:	case IDC_BUTTON248:
		    case IDC_BUTTON249:	case IDC_BUTTON250: case IDC_BUTTON251:	case IDC_BUTTON252: case IDC_BUTTON253:	case IDC_BUTTON254: case IDC_BUTTON255:	case IDC_BUTTON256:
			button = (int) LOWORD(wParam) - IDC_BUTTON1;
			EndDialog (hDlg, CHOOSEVALUE);
			return (DLGPROC)TRUE;
*/
		   }
		   break;
	    }
      return FALSE ;
      }

/**************************************************************************
	Display the Results Dialog Box
**************************************************************************/

INT_PTR CALLBACK    DisplayResultsDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
     {
     RECT	    rect, DLGRect;
     int	    VertPos, HorPos;
//     HWND	    hCtrl;
     HWND	    hCtrl;
//     int	    i, k;

     switch (message)
	  {
	  case WM_INITDIALOG:
		hCtrl = GetDlgItem (hDlg, IDOK);
		ShowWindow(hCtrl, SW_HIDE);

		hCtrl = GetDlgItem (hDlg, IDC_SAVESUDOKU);
		GetWindowRect(hCtrl, &rect);
		GetWindowRect(hDlg, &DLGRect);
		VertPos = rect.top - DLGRect.top - RHEIGHT(rect) - 1;
		MoveWindow (hCtrl, ((IsHEX) ? 400 : 330), VertPos, RWIDTH(rect), RHEIGHT(rect), TRUE);
		ShowWindow (hCtrl, SW_SHOWNORMAL);

		hCtrl = GetDlgItem (hDlg, IDC_SAVE_CSV);
		MoveWindow (hCtrl, ((IsHEX) ? 250 : 180), VertPos, RWIDTH(rect), RHEIGHT(rect), TRUE);
		ShowWindow (hCtrl, SW_SHOWNORMAL);

		// set the remaining buttons to the same vertical position
		hCtrl = GetDlgItem(hDlg, IDC_REFRESH);
		GetWindowRect(hCtrl, &rect);
		HorPos = rect.left - DLGRect.left;
		MoveWindow(hCtrl, HorPos, VertPos, RWIDTH(rect), RHEIGHT(rect), TRUE);
		ShowWindow(hCtrl, SW_SHOWNORMAL);

		hCtrl = GetDlgItem(hDlg, IDC_FINISH);
		GetWindowRect(hCtrl, &rect);
		HorPos = rect.left - DLGRect.left;
		MoveWindow(hCtrl, HorPos, VertPos, RWIDTH(rect), RHEIGHT(rect), TRUE);
		ShowWindow(hCtrl, SW_SHOWNORMAL);

		for (int row = 0; row < ArraySize; row++)
		    {
		    for (int col = 0; col < ArraySize; col++)
			{
			if (known[row][col])
			    {
			    ShowCell(hDlg, row, col);
			    HideAllCandidates(hDlg, row, col);
			    }
			else
			    {
			    HideCell(hDlg, row, col);
			    ShowAllCandidates(hDlg, row, col);
			    }
			}
		    }

	        return FALSE ;

	  case WM_PAINT:
	        {
		PAINTSTRUCT 	ps;
		int		i;
//		int		i, j, k;
		char		lpText[14];

//		BeginPaint(hDlg, &ps);
		for (i = 0; i < ArraySize * ArraySize; i++)
		    {
		    hCtrl = GetDlgItem (hDlg, i * 10 + IDC_BUTTON1);
		    BeginPaint(hCtrl, &ps);
	    	    if (matrix[i / ArraySize][i % ArraySize] > 0)		// 0 means no value set or found
			{
			if (IsHEX)
			    sprintf(lpText, "%X", matrix[i / ArraySize][i % ArraySize] - 1);
			else
			    sprintf(lpText, "%d", matrix[i / ArraySize][i % ArraySize]);
//			sprintf(lpText, "%d", matrix[i / ArraySize][i % ArraySize]);
//			if (known[i / ArraySize][i % ArraySize] > 0)
			if (orig[i / ArraySize][i % ArraySize] > 0)
			    InsertText(ps.hdc, lpText, ((IsHEX) ? 8 : 16), 0, ((IsHEX) ? 40 : 48), RGB(240,0,0), RGB(240,240,240));
			else
			    InsertText(ps.hdc, lpText, ((IsHEX) ? 8 : 16), 0, ((IsHEX) ? 40 : 48), RGB(0,0,0), RGB(240,240,240));
//			    InsertText(ps.hdc, lpText, 16,  0, 48, RGB(240,0,0), RGB(240,240,240));
//			else
//			    InsertText(ps.hdc, lpText, 16,  0, 48, RGB(0,0,0), RGB(240,240,240));
			}
		    EndPaint(hCtrl, &ps);
		    }
//		EndPaint(hCtrl, &ps);
		}
	        return TRUE ;


/*
	  case WM_PAINT:
	        {
		PAINTSTRUCT 	ps;
//		int		i, j, k;
		char		lpText[14];
		HDC		hdc;

		BeginPaint(hDlg, &ps);
		for (i = 0; i < ArraySize * ArraySize; i++)
		    {
		    hCtrl = GetDlgItem (hDlg, i * 10 + IDC_BUTTON1);
		    hdc = GetDC(hCtrl);

//		    BeginPaint(hCtrl, &ps);
	    	    if (matrix[i / ArraySize][i % ArraySize] > 0)		// 0 means no value set or found
			{
			ShowWindow(hCtrl, SW_SHOWNORMAL);			// show the main button
//			for (k = 0; k < SquareSize * SquareSize; k++)
//			    {
//			    hLittleCtrl = GetDlgItem(hDlg, (i * 10 + IDC_BUTTON1 + 1 + k));
//			    ShowWindow(hLittleCtrl, SW_HIDE);			// hide the little buttons
//			    }
			if (IsHEX)
			    sprintf(lpText, "%X", matrix[i / ArraySize][i % ArraySize] - 1);
			else
			    sprintf(lpText, "%d", matrix[i / ArraySize][i % ArraySize]);
			if (orig[i / ArraySize][i % ArraySize] > 0)
//			if (known[i / ArraySize][i % ArraySize] > 0)
			    InsertText(hdc, lpText, ((IsHEX) ? 8 : 16), 0, ((IsHEX) ? 40 : 48), RGB(240,0,0), RGB(240,240,240));
			else
			    InsertText(hdc, lpText, ((IsHEX) ? 8 : 16), 0, ((IsHEX) ? 40 : 48), RGB(0,0,0), RGB(240,240,240));
			}
		    else
			{
			ShowWindow(hCtrl, SW_HIDE);						// hide the main button
			for (j = 0; j < SquareSize; j++)
			    {
			    for (k = 0; k < SquareSize; k++)
				{
				hLittleCtrl = GetDlgItem(hDlg, (i * 10 + IDC_BUTTON1 + 1 + j * SquareSize + k));
				if (IsHEX)
				    sprintf(lpText, "%X", j * SquareSize + k);
				else
				    ShowWindow(hLittleCtrl, SW_SHOWNORMAL);			// show the little buttons
//				    sprintf(lpText, "%d", j * SquareSize + k + 1);
//				    {
				if (is_available(i / ArraySize, i % ArraySize , j * SquareSize + k + 1))
//				    if (PossibleValues[i / 9][i % 9][j * 3 + k])		// 1 means remaining possible values
				    {
				    if (IsHEX)
					InsertText(ps.hdc, lpText, k * 9 + 2,  j * 10, 10, RGB(0,0,0), RGB(240,240,240));
//				    else
//					{
//					hLittleCtrl = GetDlgItem(hDlg, (i * 10 + IDC_BUTTON1 + 1 + k));
//					ShowWindow(hLittleCtrl, SW_SHOWNORMAL);			// show the little buttons
//					}
//					InsertText(ps.hdc, lpText, k * 16 + 4,  j * 16, 16, RGB(0,0,0), RGB(240,240,240));
//					}
				    }
				else
				    ShowWindow(hLittleCtrl, SW_HIDE);			// show the little buttons
				}
			    }
			}
//		    EndPaint(hCtrl, &ps);
		    }
		}
		EndPaint(hDlg, &ps);
	        return (DLGPROC)TRUE ;
*/



	  case WM_DESTROY :
		EndDialog (hDlg, TRUE);
		return FALSE;

	  case WM_COMMAND:
	        switch ((int) LOWORD(wParam))
		    {
		    case IDC_FINISH:
		    case IDCANCEL:
			EndDialog (hDlg, FINISH);
			return TRUE;
		    case IDC_REFRESH:
			init_arrays();
//			InvalidateRect(hDlg, NULL, FALSE);      
			EndDialog (hDlg, REFRESH);
			return TRUE;
		    case IDC_SAVE_CSV:
			SaveData(hDlg, szFileName, szAppName, TRUE);
			return TRUE;
		    case IDC_SAVESUDOKU:
			SaveData(hDlg, szFileName, szAppName, FALSE);
			return TRUE;
		   }
		   break;
	    }
      return FALSE ;
      }

/*-----------------------------------------
	Get address of the cell button
  -----------------------------------------*/

int CellButton(int row, int col)
    {
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
    SetWindowState(hDlg, CandidateButton(row, col, value), SW_SHOWNORMAL);
    }

void HideCandidate(HWND hDlg, int row, int col, int value)
    {
    SetWindowState(hDlg, CandidateButton(row, col, value), SW_HIDE);
    }

void HideAllCandidates(HWND hDlg, int row, int col)
    {
    for (int k = 1; k <= SquareSize * SquareSize; k++)
	SetWindowState(hDlg, CellButton(row, col) + k, SW_HIDE);
    }

void ShowAllCandidates(HWND hDlg, int row, int col)
    {
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


