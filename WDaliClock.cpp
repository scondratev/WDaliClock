// WDALICLOCK.CPP
// Handles the Windows UI for the application, only supports W2K+
// Earle F. Philhower, III <earle@ziplabel.com>
// --- Please excuse the mess ---

#include "stdafx.h"
#include "WDaliClock.h"
#include <process.h>
#include <time.h>
#include <windowsx.h>
#include <commdlg.h>
#include <shlobj.h>
#include <stdio.h>
#include "daliclock.h"
#if SCREENSAVER
#include <scrnsave.h>
#endif
#include <pthread.h>

pthread_t thread;

COLORREF bc=RGB(255,255,192);
COLORREF fc=RGB(0,0,0);
int trans=1;
int top=1;
int cycle=1;
int fontid=2;
int h24=0;
HPEN back=NULL;
HPEN fore=NULL;

// Poor man's multi-thread communications
// This will work as long as 32-bit writes are atomic
int gforceit;
int changeid=-1;

// Global Variables:
#define MAX_LOADSTRING 100
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	ScreenSaverProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	Config(HWND, UINT, WPARAM, LPARAM);

#if !SCREENSAVER
int APIENTRY WinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WDALICLOCK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_WDALICLOCK);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}


BOOL
WINAPI
MySetLayeredWindowAttributes(
    HWND hwnd,
    COLORREF crKey,
    BYTE bAlpha,
    DWORD dwFlags)
{
	static int loaded=FALSE;
	static BOOL (WINAPI *fn)(HWND, COLORREF, BYTE, DWORD) = NULL;
	if (!loaded) {
		HINSTANCE hUSER32;
		hUSER32 = LoadLibrary("user32.dll");
		fn=(BOOL (WINAPI *)(HWND,COLORREF,BYTE,DWORD))GetProcAddress(hUSER32 , "SetLayeredWindowAttributes");
		loaded = TRUE;
	}
	if (fn)
		return fn(hwnd, crKey, bAlpha, dwFlags);
	else
		return FALSE;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ScreenSaverProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_WDALICLOCK);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; //(LPCTSTR)IDC_WDALICLOCK;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL; //LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	// initialize_digits(fontid);

	hInst = hInstance; // Store instance handle in our global variable
	// Get rid of taskbar icon
	hWnd = CreateWindowEx(NULL, _T("STATIC"), NULL, WS_POPUP, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
	hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT, 6*character_width+2*colon_char_width+0, character_height+0,
		hWnd/*NULL*/, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}


	return TRUE;
}

#endif

// move this shit to registry

void LoadDefaults(HWND hWnd)
{
	TCHAR szPath[MAX_PATH];
	TCHAR file[2*MAX_PATH];
	trans = 1;
	top = 1;
	cycle = 0;
	bc = RGB(255,255,255);
	fc = RGB(0,0,192);
	changeid = 2;
	TCHAR buff[8];
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, buff, 8);
	if (buff[0]==_T('1')) h24=1;
	else h24=0;

	if(!SUCCEEDED(SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, TRUE)))
		return;
	FILE *fp;
	sprintf(file, "%s\\WDaliClock.Settings.txt", szPath);
	fp = fopen(file, "r");
	if (fp) {
		fscanf(fp, "TRANSPARENT %d\n", &trans);
		fscanf(fp, "TOPMOST %d\n", &top);
		fscanf(fp, "CYCLE %d\n", &cycle);
		fscanf(fp, "24HOUR %d\n", &h24);
		fscanf(fp, "BACKGROUND %08x\n", &bc);
		fscanf(fp, "FOREGROUND %08x\n", &fc);
		fscanf(fp, "FONTNUMBER %d\n", &changeid);
		int xpos, ypos;
		xpos = 0;
		ypos = 0;
		fscanf(fp, "XPOS %d\n", &xpos);
		fscanf(fp, "YPOS %d", &ypos);
		#if !SCREENSAVER
		MoveWindow(hWnd, xpos, ypos, 10, 10, FALSE);
		#endif
		fclose(fp);
	}
	if (trans) trans=1;
	if (top) top=1;
	if (cycle) cycle=1;
	if (changeid<0) changeid=0;
	if (changeid>3) changeid=3;
	if (trans) bc=RGB(255,255,255);
}

void SaveDefaults(HWND hWnd)
{
	TCHAR szPath[MAX_PATH];
	TCHAR file[2*MAX_PATH];
	WINDOWPLACEMENT wp;
	wp.length=sizeof(wp);
	GetWindowPlacement(hWnd, &wp);
	if(!SUCCEEDED(SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, TRUE)))
		return;
	FILE *fp;
	sprintf(file, "%s\\WDaliClock.Settings.txt", szPath);
	fp = fopen(file, "w");
	if (fp) {
		fprintf(fp, "TRANSPARENT %d\n", trans);
		fprintf(fp, "TOPMOST %d\n", top);
		fprintf(fp, "CYCLE %d\n", cycle);
		fprintf(fp, "24HOUR %d\n", h24);
		fprintf(fp, "BACKGROUND %08x\n", bc);
		fprintf(fp, "FOREGROUND %08x\n", fc);
		fprintf(fp, "FONTNUMBER %d\n", (changeid>=0)?changeid:fontid);
		fprintf(fp, "XPOS %d\n", wp.rcNormalPosition.left);
		fprintf(fp, "YPOS %d\n", wp.rcNormalPosition.top);
		fclose(fp);
	}
}


void* MyThread(void *xxx)
{
	HWND hWnd;
	HDC hDc;
	int ctr, forceit, oncethru, colonSize, sync;
	int ld[6]; // Last digit , -1=space
	int nd[6]; // Next digit

	back=CreatePen(0,0,bc);
	fore=CreatePen(0,0,fc);
	hWnd = (HWND)xxx;
	hDc = GetDC(hWnd);
	ctr=0;
	forceit=1;
	oncethru=0;
	memset(ld, 0, 8*sizeof(int));
	int DlgWidth, DlgHeight;
	
	while(1) {
		if (changeid>=0) {
			initialize_digits(changeid);

#if SCREENSAVER
			DlgWidth  = GetSystemMetrics(SM_CXSCREEN);
			DlgHeight = GetSystemMetrics(SM_CYSCREEN);
#else
			DlgWidth = 6*character_width+2*colon_char_width+0;
			DlgHeight = character_height+0;
#endif
			
			SetWindowPos(hWnd, 0, 0, 0, DlgWidth, DlgHeight, SWP_NOMOVE|SWP_NOZORDER);
			fontid=changeid;
			changeid=-1;
		}
		if (!oncethru) {
			// We need to map the window, all the settings are finally available
			// if (trans) {
			// 	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
			// 	MySetLayeredWindowAttributes(hWnd, RGB(255,255,255), 255, LWA_COLORKEY);
			// }

			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);
			SetWindowPos(hWnd, top?HWND_TOPMOST:HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			oncethru = 1;

			time_t sync;
			time(&sync);
			while (sync == time((time_t *) 0))
			  {
			    Sleep(1);
			  }
		}
		if (cycle) {cycle_colors(); forceit=1;}
		
		time_t clock = time ((time_t *) 0);
		struct tm *tm = localtime (&clock);
		if (!h24) {
			tm->tm_hour = tm->tm_hour%12;
			if (tm->tm_hour==0) tm->tm_hour=12;
		}
		if (!h24) {
			if (tm->tm_hour/10) nd[0]=tm->tm_hour/10; else nd[0] = -1;
		} else {
			nd[0] = tm->tm_hour/10;
		}
		nd[1] = tm->tm_hour%10;
		nd[2] = tm->tm_min/10;
		nd[3] = tm->tm_min%10;
		nd[4] = tm->tm_sec/10;
		nd[5] = tm->tm_sec%10;
	
		DWORD secTc = GetTickCount();
		int cycleTime;
		for (int i=0; i<10; i++) {
			DWORD tc = GetTickCount();
			
			int x, y;
#if SCREENSAVER
			x=DlgWidth/2-character_width*3;
			y=DlgHeight/2-character_height/2;
#else
			x=0;
			y=0;
#endif
			for (int j=0; j<6; j++) {
				if (nd[j]==-1)
					DrawSpace(hDc, x, y);
				else if (ld[j]==-1)
				  {
				    // cache the glyph here
					DrawStage(hDc, x, y, nd[j], nd[j], 1);
				  }
				else if (forceit || ld[j]!=nd[j])
					DrawStage(hDc, x, y, ld[j], nd[j], i);
				x += character_width;
				if (j==1 || j==3)
				  {
				    if (forceit) colonSize = DrawColon(hDc, x, y);
				    x += colonSize;
				  }
				    
			}
			// if (trans) {
			// 	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
			// 	MySetLayeredWindowAttributes(hWnd, RGB(255,255,255), 255, LWA_COLORKEY);
			// } else
			SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);

			const int timeout = 1000 / 12;
			// first init may be abysmal
			cycleTime = (GetTickCount() - tc);
			if (cycleTime < timeout)
			  Sleep(timeout - cycleTime);
			forceit=gforceit;
			gforceit=0;
		}
		
		cycleTime = (GetTickCount() - secTc);
		if (cycleTime < 1000)
		  Sleep(1000 - cycleTime);
		memcpy(ld, nd, 8*sizeof(int));
	}
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static int indrag = 0;
	static POINT st, pt;
	static WINDOWINFO wi;
	
	switch (message) 
	{
	case WM_CREATE:
		LoadDefaults(hWnd);
		// MyThread(hWnd);
		pthread_create(&thread, NULL, MyThread, hWnd);
		// _beginthread(MyThread, 0, hWnd);
		break;
#if !SCREENSAVER
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_CONFIG:
			DialogBox(hInst, (LPCTSTR)IDD_CONFIG, hWnd, (DLGPROC)Config);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUT, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
 		gforceit=1;
		EndPaint(hWnd, &ps);
		break;

	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		indrag=1;
		st.x = GET_X_LPARAM(lParam);
		st.y = GET_Y_LPARAM(lParam);
		ClientToScreen(hWnd, &st);
		wi.cbSize = sizeof(wi);
		GetWindowInfo(hWnd, &wi);
		return TRUE;

	case WM_LBUTTONUP:
		if (!indrag) break;
		ReleaseCapture();
		indrag=0;
		return TRUE;

	case WM_MOUSEMOVE:
		if (!indrag) break;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		ClientToScreen(hWnd, &pt);

		pt.x -= st.x;
		pt.y -= st.y;
		SetWindowPos(hWnd, NULL, wi.rcClient.left+pt.x, wi.rcClient.top+pt.y, 0, 0, SWP_NOSIZE);
		return TRUE;

	case WM_CONTEXTMENU:
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		if (lParam==MAKELPARAM(-1,-1)) GetCursorPos(&pt);

		TrackPopupMenu(GetSubMenu(LoadMenu(hInst, (LPCTSTR)IDC_WDALICLOCK), 0),
					   TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON,
					   pt.x, pt.y, 0, hWnd, NULL);
		return TRUE;
#endif
	case WM_DESTROY:
		SaveDefaults(hWnd);
		PostQuitMessage(0);
		break;

	default:
#if SCREENSAVER
		return DefScreenSaverProc(hWnd, message, wParam, lParam);
#else
		return DefWindowProc(hWnd, message, wParam, lParam);
#endif
	}
	return 0;
}

#if !SCREENSAVER
// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
#endif

#if SCREENSAVER
BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
  #else
LRESULT CALLBACK Config(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
#endif
{
	switch (message)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDC_TRANS, BM_SETCHECK, trans?BST_CHECKED:BST_UNCHECKED,0);
		SendDlgItemMessage(hDlg, IDC_TOPMOST, BM_SETCHECK, top?BST_CHECKED:BST_UNCHECKED,0);
		SendDlgItemMessage(hDlg, IDC_CYCLE, BM_SETCHECK, cycle?BST_CHECKED:BST_UNCHECKED,0);
		SendDlgItemMessage(hDlg, IDC_24HOUR, BM_SETCHECK, h24?BST_CHECKED:BST_UNCHECKED,0);
		SendDlgItemMessage(hDlg, IDC_FONTID, CB_ADDSTRING, 0, (LPARAM)"Font 1");
		SendDlgItemMessage(hDlg, IDC_FONTID, CB_ADDSTRING, 0, (LPARAM)"Font 2");
		SendDlgItemMessage(hDlg, IDC_FONTID, CB_ADDSTRING, 0, (LPARAM)"Font 3");
		SendDlgItemMessage(hDlg, IDC_FONTID, CB_ADDSTRING, 0, (LPARAM)"Font 4");
		SendDlgItemMessage(hDlg, IDC_FONTID, CB_SETCURSEL, fontid, 0);
		EnableWindow(GetDlgItem(hDlg,IDC_BACK), !trans);
		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam)) {
	case IDOK:
	case IDCANCEL:
		int newfontid;
		newfontid=(int)SendDlgItemMessage(hDlg, IDC_FONTID, CB_GETCURSEL, 0, 0);
		if (newfontid!=fontid) {
			changeid=newfontid;
		}
		SaveDefaults(GetParent(hDlg));
		EndDialog(hDlg, LOWORD(wParam));
		return TRUE;

	case IDC_BACK:
		{
			CHOOSECOLOR cc;                 // common dialog box structure 
			static COLORREF acrCustClr[16]; // array of custom colors 

			// Initialize CHOOSECOLOR 
			ZeroMemory(&cc, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hDlg;
			cc.lpCustColors = (LPDWORD) acrCustClr;
			cc.rgbResult = bc;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc)==TRUE) {
				DeleteObject(back);
				back = CreatePen(0,0,bc=cc.rgbResult);
				gforceit=1;
			}
		}
		break;

	case IDC_FORE:
		{
			CHOOSECOLOR cc;                 // common dialog box structure 
			static COLORREF acrCustClr[16]; // array of custom colors 

			ZeroMemory(&cc, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hDlg;
			cc.lpCustColors = (LPDWORD) acrCustClr;
			cc.rgbResult = fc;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc)==TRUE) {
				DeleteObject(fore);
				fore= CreatePen(0,0,fc=cc.rgbResult);
				gforceit=1;
			}
		}
		break;

	case IDC_TRANS:
		if (SendDlgItemMessage(hDlg, IDC_TRANS, BM_GETCHECK, 0, 0)==BST_CHECKED)
			trans=1;
		else
			trans=0;
		EnableWindow(GetDlgItem(hDlg,IDC_BACK), !trans);
		if (trans) bc=RGB(255,255,255);
		DeleteObject(back);
		back= CreatePen(0,0,bc);
		gforceit=1;
		break;

	case IDC_CYCLE:
		if (SendDlgItemMessage(hDlg, IDC_CYCLE, BM_GETCHECK, 0, 0)==BST_CHECKED)
			cycle=1;
		else
			cycle=0;
		gforceit=1;
		break;

	case IDC_24HOUR:
		if (SendDlgItemMessage(hDlg, IDC_24HOUR, BM_GETCHECK, 0, 0)==BST_CHECKED)
			h24=1;
		else
			h24=0;
		gforceit=1;
		break;


	case IDC_TOPMOST:
		if (SendDlgItemMessage(hDlg, IDC_TOPMOST, BM_GETCHECK, 0, 0)==BST_CHECKED)
			top=1;
		else
			top=0;
		SetWindowPos(GetParent(hDlg), top?HWND_TOPMOST:HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE); 
		gforceit=1;
		break;
		}
	}
	return FALSE;
}

#ifdef SCREENSAVER
BOOL WINAPI RegisterDialogClasses(HANDLE hInst) { return TRUE; }
#endif
