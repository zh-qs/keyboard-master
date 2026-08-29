// KeyboardMaster.cpp : Definiuje punkt wejścia dla aplikacji.
//

#include "framework.h"
#include "KeyboardMaster.h"
#include <cstdlib>
#include <ctime>
#include <windowsx.h>
#include <commdlg.h>

#define MAX_LOADSTRING 100

// Zmienne globalne:
HINSTANCE hInst;                                // bieżące wystąpienie
WCHAR szTitle[MAX_LOADSTRING];                  // Tekst paska tytułu
WCHAR szWindowClass[MAX_LOADSTRING];            // nazwa klasy okna głównego
WCHAR szBlackSquareClass[MAX_LOADSTRING];
HWND lowestSquare = NULL;
int blackSquareCount = 0;
int missed = 0;
int wrongKeys = 0;
bool paused = false;

// Przekaż dalej deklaracje funkcji dołączone w tym module kodu:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: W tym miejscu umieść kod.
    srand(time(NULL));
    // Inicjuj ciągi globalne
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_KEYBOARDMASTER, szWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BLACKSQUARE, szBlackSquareClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    RegisterBlackSquare(hInstance);

    // Wykonaj inicjowanie aplikacji:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KEYBOARDMASTER));

    MSG msg;

    // Główna pętla komunikatów:
    while (GetMessage(&msg, nullptr, 0, 0))
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
//  FUNKCJA: MyRegisterClass()
//
//  PRZEZNACZENIE: Rejestruje klasę okna.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KEYBOARDMASTER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_INACTIVECAPTION+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_KEYBOARDMASTER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

ATOM RegisterBlackSquare(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = BlackSquareProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KEYBOARDMASTER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_KEYBOARDMASTER);
    wcex.lpszClassName = szBlackSquareClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNKCJA: InitInstance(HINSTANCE, int)
//
//   PRZEZNACZENIE: Zapisuje dojście wystąpienia i tworzy okno główne
//
//   KOMENTARZE:
//
//        W tej funkcji dojście wystąpienia jest zapisywane w zmiennej globalnej i
//        jest tworzone i wyświetlane okno główne programu.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Przechowuj dojście wystąpienia w naszej zmiennej globalnej

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & (~(WS_SIZEBOX | WS_MAXIMIZEBOX)),
       CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNKCJA: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PRZEZNACZENIE: Przetwarza komunikaty dla okna głównego.
//
//  WM_COMMAND  - przetwarzaj menu aplikacji
//  WM_PAINT    - Maluj okno główne
//  WM_DESTROY  - opublikuj komunikat o wyjściu i wróć
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    const int ofnBufSize = 256;
    TCHAR ofnBuf[ofnBufSize];
    static HBITMAP bmp = NULL;
    static UINT bkGroundMode = BKGM_NONE;
    static HBRUSH bkBrush = NULL;
    static HMENU popupMenuTemplate;

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analizuj zaznaczenia menu:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case ID_PAUSE:
                //pause
                TogglePause(hWnd);
                break;
            case ID_NEWGAME:
                //new game
                EnumChildWindows(hWnd, EnumDestroy, NULL);
                missed = 0;
                wrongKeys = 0;
                UpdateTitle(hWnd);
                break;
            case ID_CONTEXTMENU_BITMAP:
            {
                ofnBuf[0] = _T('\0');
                OPENFILENAME ofn;
                ZeroMemory(&ofn, sizeof(OPENFILENAME));
                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.hwndOwner = hWnd;
                ofn.lpstrFilter = OPENDIALOG_FILTER;
                ofn.lpstrFile = ofnBuf;
                ofn.nMaxFile = ofnBufSize;
                ofn.Flags = OFN_FILEMUSTEXIST;
                if (GetOpenFileName(&ofn))
                {
                    if (bmp != NULL) DeleteObject(bmp);
                    bmp = (HBITMAP)LoadImage(NULL, ofnBuf, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                    bkGroundMode = BKGM_BMPTILE;
                    CheckMenuItem(popupMenuTemplate, ID_CONTEXTMENU_TILE, MF_CHECKED);
                    CheckMenuItem(popupMenuTemplate, ID_CONTEXTMENU_STRETCH, MF_UNCHECKED);
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
                break;
            case ID_CONTEXTMENU_STRETCH:
                if (bkGroundMode == BKGM_BMPTILE)
                {
                    bkGroundMode = BKGM_BMPSTRETCH;
                    CheckMenuItem(popupMenuTemplate, ID_CONTEXTMENU_TILE, MF_UNCHECKED);
                    CheckMenuItem(popupMenuTemplate, ID_CONTEXTMENU_STRETCH, MF_CHECKED);
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                break;
            case ID_CONTEXTMENU_TILE:
                if (bkGroundMode == BKGM_BMPSTRETCH)
                {
                    bkGroundMode = BKGM_BMPTILE;
                    CheckMenuItem(popupMenuTemplate, ID_CONTEXTMENU_TILE, MF_CHECKED);
                    CheckMenuItem(popupMenuTemplate, ID_CONTEXTMENU_STRETCH, MF_UNCHECKED);
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                break;
            case ID_CONTEXTMENU_COLOR:
            {
                CHOOSECOLOR cc;
                ZeroMemory(&cc, sizeof(CHOOSECOLOR));
                COLORREF custColors[16] = { 0 };
                cc.lStructSize = sizeof(CHOOSECOLOR);
                cc.Flags = CC_FULLOPEN;
                cc.hwndOwner = hWnd;
                cc.lpCustColors = custColors;
                if (ChooseColor(&cc))
                {
                    if (bmp != NULL)
                    {
                        DeleteObject(bmp);
                        bmp = NULL;
                    }
                    if (bkBrush != NULL)
                        DeleteObject(bkBrush);
                    bkBrush = CreateSolidBrush(cc.rgbResult);
                    bkGroundMode = BKGM_COLOR;
                    CheckMenuItem(popupMenuTemplate, ID_CONTEXTMENU_TILE, MF_UNCHECKED);
                    CheckMenuItem(popupMenuTemplate, ID_CONTEXTMENU_STRETCH, MF_UNCHECKED);
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (bkGroundMode != BKGM_NONE)
            {
                HDC memDC = CreateCompatibleDC(hdc);
                RECT rc;
                GetClientRect(hWnd, &rc);
                if (bmp != NULL)
                {
                    if (bkGroundMode == BKGM_BMPSTRETCH)
                    {
                        BITMAP bp;
                        GetObject(bmp, sizeof(BITMAP), (LPTSTR)&bp);
                        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, bmp);
                        StretchBlt(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, memDC, 0, 0, bp.bmWidth, bp.bmHeight, SRCCOPY);
                        SelectObject(memDC, oldBmp);
                    }
                    else //BKGM_BMPTILE
                    {
                        HBRUSH patBr = CreatePatternBrush(bmp);
                        HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, patBr);
                        HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
                        HBITMAP oldMemBmp = (HBITMAP)SelectObject(memDC, memBmp);
                        HPEN oldPen = (HPEN)SelectObject(memDC, (HPEN)GetStockObject(NULL_PEN));
                        Rectangle(memDC, rc.left, rc.top, rc.right, rc.bottom);
                        BitBlt(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, memDC, rc.left, rc.top, SRCCOPY);
                        SelectObject(memDC, oldBrush);
                        SelectObject(memDC, oldMemBmp);
                        SelectObject(memDC, oldPen);
                        DeleteObject(memBmp);
                        DeleteObject(patBr);
                    }
                }
                else //BKGM_COLOR
                {
                    HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, bkBrush);
                    HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
                    HBITMAP oldMemBmp = (HBITMAP)SelectObject(memDC, memBmp);
                    HPEN oldPen = (HPEN)SelectObject(memDC, (HPEN)GetStockObject(NULL_PEN));
                    Rectangle(memDC, rc.left, rc.top, rc.right, rc.bottom);
                    BitBlt(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, memDC, rc.left, rc.top, SRCCOPY);
                    SelectObject(memDC, oldBrush);
                    SelectObject(memDC, oldMemBmp);
                    SelectObject(memDC, oldPen);
                    DeleteObject(memBmp);
                }
                DeleteDC(memDC);
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        DestroyMenu(popupMenuTemplate);
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        {
            UpdateTitle(hWnd);
            RECT rc;
            SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
            int centerX = (rc.right - rc.left) / 2;
            int centerY = (rc.bottom - rc.top) / 2;
            SetWindowPos(hWnd, HWND_TOPMOST, centerX / 2, centerY / 2, centerX, centerY, NULL);
            popupMenuTemplate = LoadMenu(hInst, MAKEINTRESOURCE(IDR_CONTEXTMENU));
            SetTimer(hWnd, MAIN_TIMER_ID, GetRandomDelay(),NULL);
        }
        break;
    case WM_TIMER:
        if (!paused && wParam == MAIN_TIMER_ID && blackSquareCount < MAX_SQUARE_COUNT)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);
            int randomPosition = rand() % (rc.right - rc.left - SQUARE_HEIGHT) + rc.left;
            HWND sqhWnd = CreateWindow(szBlackSquareClass, szTitle, WS_CHILD | WS_CLIPSIBLINGS, randomPosition, 0, SQUARE_HEIGHT, SQUARE_HEIGHT, hWnd, nullptr, hInst, nullptr);
            ShowWindow(sqhWnd, SW_SHOW);
            UpdateWindow(sqhWnd);
            blackSquareCount++;
            KillTimer(hWnd, MAIN_TIMER_ID);
            SetTimer(hWnd, MAIN_TIMER_ID, GetRandomDelay(), NULL);
        }
        break;
    case WM_CHAR:
        if (wParam >= _T('A') && wParam <= _T('Z')) wParam += ('a' - 'A');
        if (wParam >= _T('a') && wParam <= _T('z'))
        {
            lowestSquare = NULL;
            EnumChildWindows(hWnd, EnumProc, wParam);
            if (lowestSquare == NULL)
            {
                wrongKeys++;
                UpdateTitle(hWnd);
            }
            else
            {
                DestroyWindow(lowestSquare);
                // global variable int blackSquareCount
                blackSquareCount--;
            }
        }
        break;
    case WM_SETFOCUS:
        ResumeGame(hWnd);
        break;
    case WM_KILLFOCUS:
        PauseGame(hWnd);
        break;
    case WM_CONTEXTMENU:
        TrackPopupMenuEx(GetSubMenu(popupMenuTemplate, 0), NULL, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hWnd, NULL);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK BlackSquareProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            TCHAR* prndc = new TCHAR; 
            *prndc = GetRandomChar();
            SetTimer(hWnd, FALL_TIMER_ID, GetRandomFallTimerTick(), NULL);
            SetProp(hWnd, CHAR_ASSIGNED, prndc);
        }
        break;
    case WM_TIMER:
        if (!paused && wParam == FALL_TIMER_ID)
        {
            RECT rc;
            GetWindowRect(hWnd, &rc);
            MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&rc, 2);
            MoveWindow(hWnd, rc.left, rc.top + FALLING_VELOCITY, SQUARE_HEIGHT, SQUARE_HEIGHT, TRUE);
        }
        break;
    case WM_MOVE:
        {
            RECT rc, rcparent;
            GetWindowRect(hWnd, &rc);
            MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&rc, 2);
            GetClientRect(GetParent(hWnd), &rcparent);
            if (rc.bottom > rcparent.bottom)
            {
                missed++;
                UpdateTitle(GetParent(hWnd));
                // global variable int blackSquareCount
                blackSquareCount--;
                DestroyWindow(hWnd);
            }
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        TCHAR buf[2];
        buf[0] = *((TCHAR*)GetProp(hWnd, CHAR_ASSIGNED));
        buf[1] = 0;
        GetClientRect(hWnd, &rc);
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkColor(hdc, RGB(0, 0, 0));
        DrawText(hdc, buf, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        EndPaint(hWnd, &ps);
    }
        break;
    case WM_NCDESTROY:
    {
        TCHAR* prndc = (TCHAR*)RemoveProp(hWnd, CHAR_ASSIGNED);
        delete prndc;
    }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Procedura obsługi komunikatów dla okna informacji o programie.
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

BOOL CALLBACK EnumProc(HWND child, LPARAM param)
{
    // global variable HWND lowestSquare
    TCHAR squareCh = *((TCHAR*)GetProp(child, CHAR_ASSIGNED));
    TCHAR enteredCh = (TCHAR)param;
    if (squareCh != enteredCh) return TRUE;
    if (lowestSquare == NULL)
    {
        lowestSquare = child;
        return TRUE;
    }
    RECT rc, rcmax;
    GetWindowRect(child, &rc);
    GetWindowRect(lowestSquare, &rcmax);
    if (rc.top > rcmax.top)
        lowestSquare = child;
    return TRUE;
}

BOOL CALLBACK EnumDestroy(HWND child, LPARAM param)
{
    DestroyWindow(child);
    return TRUE;
}

void UpdateTitle(HWND hWnd)
{
    const int bufSize = 256;
    TCHAR buf[bufSize];
    _stprintf_s(buf, bufSize, _T("%s, Missed: %d, Wrong keys: %d"), szTitle, missed, wrongKeys);
    SetWindowText(hWnd, buf);
}

void PauseGame(HWND hWnd)
{
    paused = true;
    CheckMenuItem(GetMenu(hWnd), ID_PAUSE, MF_CHECKED);
}

void ResumeGame(HWND hWnd)
{
    paused = false;
    CheckMenuItem(GetMenu(hWnd), ID_PAUSE, MF_UNCHECKED);
}

void TogglePause(HWND hWnd)
{
    if (paused) ResumeGame(hWnd);
    else PauseGame(hWnd);
}