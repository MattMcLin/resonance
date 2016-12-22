// Checkers.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Checkers.h"
#include "Board.h"
#include <windowsx.h>
#include <stdio.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

const int squareSize = 60;   // square size in pixels
Board g_board;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void renderBoard(HDC hDC, Board* pBoard);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CHECKERS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHECKERS));

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
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHECKERS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CHECKERS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
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

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

    // start & end coordinates for tracking mouse drags
    static int startX = 0;
    static int startY = 0;
    static int endX = 0;
    static int endY = 0;
    static bool movingPiece = false;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
    case WM_LBUTTONDOWN:
    {
        if (!g_board.gameOver())
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            if (movingPiece)
            {
                endX = xPos / squareSize;
                endY = yPos / squareSize;
                Cell start(startX, startY);
                Cell end(endX, endY);
                g_board.move(start, end);
                InvalidateRect(hWnd, NULL, FALSE);
                movingPiece = false;
            }
            else
            {
                startX = xPos / squareSize;
                startY = yPos / squareSize;
                movingPiece = true;
            }
        }
    } break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
        renderBoard(hdc, &g_board);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void drawCenteredText(HDC hdc, int x, int y, const char* st)
{
    const int yOffset = 5;
    y += yOffset;
    SetTextAlign(hdc, TA_CENTER | TA_BASELINE | TA_NOUPDATECP);
    TextOutA(hdc, x, y, st, strlen(st));
}

void renderBoard(HDC hdc, Board * pBoard)
{
    if (pBoard)
    {
        SetTextAlign(hdc, TA_CENTER| TA_BASELINE|TA_NOUPDATECP);
        // There is no option for vertically centered alignment, so we have
        // to provide our own offset.
        for (int j = 0; j < NUM_SQUARES; ++j)
        {
            int y = j*squareSize;
            for (int i = 0; i < NUM_SQUARES; ++i)
            {
                int x = i*squareSize;
                if (g_board.cellIsWhite(i, j))
                {
                    SelectObject(hdc, GetStockObject(LTGRAY_BRUSH));
                }
                else
                {
                    SelectObject(hdc, GetStockObject(BLACK_BRUSH));
                }
                Rectangle(hdc, x, y, x + squareSize, y + squareSize);

                Cell pos(i, j);
                if (!g_board.isEmpty(pos))
                {
                    if (g_board.pieceAt(pos).isWhite())
                    {
                        SelectObject(hdc, GetStockObject(GRAY_BRUSH));
                    }
                    else
                    {
                        SelectObject(hdc, GetStockObject(DKGRAY_BRUSH));
                    }
                    Ellipse(hdc, x, y, x + squareSize, y + squareSize);

                    if (g_board.pieceAt(pos).isKing())
                    {
                        SelectObject(hdc, GetStockObject(BLACK_BRUSH));
                        drawCenteredText(hdc, x + squareSize / 2, y + squareSize / 2, "K");
                    }
                }
            }
        }

        int middle = (NUM_SQUARES * squareSize) / 2;
        SelectObject(hdc, GetStockObject(BLACK_BRUSH));
        char sScore[256];
        sprintf_s(sScore, 256, "White score: %d, black score: %d", g_board.score(WHITE_PLAYER), g_board.score(BLACK_PLAYER));
        drawCenteredText(hdc, middle, ((NUM_SQUARES + 1) * squareSize), sScore);
        if (g_board.gameOver())
        {
            drawCenteredText(hdc, middle, middle, "Game Over");

            if (g_board.isWinner(WHITE_PLAYER))
            {
                drawCenteredText(hdc, middle, middle + 20, "White Wins!");
            }
            else if (g_board.isWinner(BLACK_PLAYER))
            {
                drawCenteredText(hdc, middle, middle + 20, "Black Wins!");
            }
        }
    }
}
