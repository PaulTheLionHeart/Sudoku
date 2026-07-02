/*------------------------------------------
   FILEOPS.CPP -- Read and Write Data Files
  ------------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <time.h>
#include "sudoku.h"

#define	MAXBUFFER	10000
//#define	RECORDDUMP					// dump all records to test.cal

static	OPENFILENAME	ofn ;
static	char	*szFilter[] = { "Sudoku Files (*.SUD)",	"*.sud", "CSV (Text) Files (*.TXT)", "*.txt", "All Files (*.*)", "*.*",	"" };

extern	int	ArraySize, SquareSize;				// ArraySize is the size of the side of the puzzle, SquareSize is the size of the side of a square
extern	BOOL	IsHEX;						// default is 9*9 Sudoku
extern	BOOL	IsOrig;						// Save the original puzzle
extern	int	known[HEXARRAYSIZE][HEXARRAYSIZE];		// dimension for larger array if required
extern	int	matrix[HEXARRAYSIZE][HEXARRAYSIZE];
extern	int	orig[HEXARRAYSIZE][HEXARRAYSIZE];

extern	void	init_arrays(void);
extern	void	set_cell(int i, int j, int n);

void FileInitialize (HWND hwnd)
     {
     ofn.lStructSize	   = sizeof (OPENFILENAME) ;
     ofn.hwndOwner	   = hwnd ;
     ofn.hInstance	   = NULL ;
     ofn.lpstrFilter	   = szFilter [0] ;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex	   = 0 ;
     ofn.lpstrFile	   = NULL ;	     // Set in Open and Close functions
     ofn.nMaxFile	   = _MAX_PATH ;
     ofn.lpstrFileTitle    = NULL ;	     // Set in Open and Close functions
     ofn.nMaxFileTitle	   = _MAX_FNAME + _MAX_EXT ;
     ofn.lpstrInitialDir   = NULL ;
     ofn.lpstrTitle	   = NULL ;
     ofn.Flags		   = 0 ;	     // Set in Open and Close functions
     ofn.nFileOffset	   = 0 ;
     ofn.nFileExtension    = 0 ;
     ofn.lpstrDefExt	   = "sud" ;
     ofn.lCustData	   = 0L ;
     ofn.lpfnHook	   = NULL ;
     ofn.lpTemplateName    = NULL ;
     }

BOOL FileSaveDlg (HWND hwnd, LPSTR lpstrFileName, LPSTR lpstrTitleName, BOOL csv)
     {
     ofn.hwndOwner	   = hwnd ;
     ofn.lpstrFile	   = lpstrFileName ;
     ofn.lpstrFileTitle    = lpstrTitleName ;
     ofn.Flags		   = OFN_OVERWRITEPROMPT ;
     if (csv)
	 {
	 ofn.lpstrDefExt	   = "txt" ;
	 ofn.lpstrFilter	   = szFilter [2] ;
	 }
     else 
	 {
	 ofn.lpstrDefExt	   = "sud" ;
	 ofn.lpstrFilter	   = szFilter [1] ;
	 }
     return GetSaveFileName (&ofn) ;
     }

BOOL FileReadDlg (HWND hwnd, LPSTR lpstrFileName, LPSTR lpstrTitleName, BOOL csv)
     {
     ofn.hwndOwner	   = hwnd ;
     ofn.lpstrFile	   = lpstrFileName ;
     ofn.lpstrFileTitle    = lpstrTitleName ;
     ofn.Flags		   = OFN_CREATEPROMPT ;
     ofn.lpstrDefExt	   = "sud" ;
     ofn.lpstrFilter	   = szFilter [1] ;
     return GetOpenFileName (&ofn) ;
     }

/**************************************************************************
	write the datafile. 
**************************************************************************/

BOOL FileWrite (HWND hwnd, LPSTR lpstrFileName, BOOL csv)
    {
    FILE	*hFile;
    int		i, j;
    DWORD	ErrorValue;
    char	s[240];

    if ((hFile = fopen (lpstrFileName, "w+")) < 0)
	{
	ErrorValue = GetLastError();
	sprintf (s, "Could not open file %s! Error code %ld", lpstrFileName, ErrorValue) ;
	MessageBox(hwnd, s, "Write File", MB_ICONEXCLAMATION | MB_OK);
	return FALSE ;
	}

    if (csv)
	{
	for (i = 0; i < ArraySize; i++)
	    {
	    for (j = 0; j < ArraySize; j++)
		{
		if (IsHEX)
		    fprintf(hFile, "%X", matrix[i][j] - 1);
		else
		    fprintf(hFile, "%d", matrix[i][j]);
		fprintf(hFile, ((j == ArraySize - 1) ? "\n" : ","));
		}
	    }
	}
    else
	{
	fprintf(hFile, "%d\n", IsHEX);
	for (i = 0; i < ArraySize; i++)
	    {
	    for (j = 0; j < ArraySize; j++)
		{
		if (IsOrig)
		    fprintf(hFile, "%d, %d, %d, %d\n", i, j, matrix[i][j], (known[i][j] != 0));
		else
		    fprintf(hFile, "%d, %d, %d, %d\n", i, j, matrix[i][j], orig[i][j]);
		}
	    }
	}
    fclose (hFile) ;
    IsOrig = FALSE;
    return TRUE ;
    }

/**************************************************************************
	write the datafile. 
**************************************************************************/

int	SaveData(HWND hwnd, char *szFileName, char *szAppName, BOOL csv)

    {
    char    s[240];

    FileInitialize(hwnd);
    if (FileSaveDlg (hwnd, szFileName, szAppName, csv))
	{
	if (!FileWrite (hwnd, szFileName, csv))
	    {
	    sprintf (s, "Could not write file %s!", szFileName) ;
	    MessageBox(hwnd, s, "Sudoku File", MB_ICONEXCLAMATION | MB_OK);
	    szFileName  [0] = '\0';
	    return -1;
	    }
	}
    return 0;
    }

/**************************************************************************
	Read the datafile. 
**************************************************************************/

BOOL FileRead (HWND hwnd, LPSTR lpstrFileName)
    {
    FILE	*hFile;
    int		i, j, value, isorig, result;
    DWORD	ErrorValue;
    char	s[240];
    char	data[240];

    if ((hFile = fopen (lpstrFileName, "r")) < 0)
	{
	ErrorValue = GetLastError();
	sprintf (s, "Could not open file %s! Error code %ld", lpstrFileName, ErrorValue) ;
	MessageBox(hwnd, s, "Read File", MB_ICONEXCLAMATION | MB_OK);
	return FALSE ;
	}

    if (!fgets(data, 20, hFile))
	{
	sprintf (s, "Could read file %s!", lpstrFileName) ;
	MessageBox(hwnd, s, "Read File", MB_ICONEXCLAMATION | MB_OK);
	return FALSE ;
	}
    else
	{
	IsHEX = (data[0] == '1');
	ArraySize = (IsHEX) ? HEXARRAYSIZE : NORMALARRAYSIZE;		// dimension array indexing
	SquareSize = (IsHEX) ? HEXSQUARESIZE : NORMALSQUARESIZE;
	init_arrays();
	}
    while (fgets(data, 20, hFile))
	{
	result = sscanf(data, "%d, %d, %d, %d", &i, &j, &value, &isorig);
	if (i < ArraySize && j < ArraySize && value > 0)
	    {
	    matrix[i][j] = value;
	    known[i][j] = 1;
	    orig[i][j] = (result == 3) ? 1 : isorig;
	    set_cell(i, j, value);
	    }
	}
    fclose (hFile) ;
    return TRUE ;
    }

/**************************************************************************
	Read the datafile. 
**************************************************************************/

int	GetData(HWND hwnd, char *szFileName, char *szAppName)

    {
    char    s[240];

    FileInitialize(hwnd);
    if (FileReadDlg (hwnd, szFileName, szAppName, FALSE))
	{
	if (!FileRead (hwnd, szFileName))
	    {
//	    sprintf (s, "Could not read file %s!", szFileName) ;	// silly virus scanner has a hissy fit on this line of code
	    sprintf_s (s, 235, "Could not read file %s!", szFileName) ;
	    MessageBox(hwnd, s, "Sudoku File", MB_ICONEXCLAMATION | MB_OK);
	    szFileName  [0] = '\0';
	    return -1;
	    }
	}
    return 0;
    }

