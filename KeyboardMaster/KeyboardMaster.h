#pragma once

#include "resource.h"
#include <cstdlib>

#define MAIN_TIMER_ID 1
#define FALL_TIMER_ID 2

#define MAX_TICK 60
#define MIN_TICK 15

#define MIN_DELAY 300
#define MAX_DELAY 1000

#define MAX_SQUARE_COUNT 100
#define SQUARE_HEIGHT 25

#define FALLING_VELOCITY 1

#define CHAR_ASSIGNED _T("CA")
#define OPENDIALOG_FILTER _T("Bitmaps (*.bmp)\0*.bmp\0\0")

#define BKGM_NONE 0
#define BKGM_COLOR 1
#define BKGM_BMPTILE 2
#define BKGM_BMPSTRETCH 3

ATOM RegisterBlackSquare(HINSTANCE hInstance);
LRESULT CALLBACK BlackSquareProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int GetRandomDelay() { return rand() % (MAX_DELAY - MIN_DELAY + 1) + MIN_DELAY; }

int GetRandomFallTimerTick() { return rand() % (MAX_TICK - MIN_TICK + 1) + MIN_TICK; }

TCHAR GetRandomChar() { return rand() % (_T('z') - _T('a')) + _T('a'); }

BOOL CALLBACK EnumProc(HWND child, LPARAM param);
BOOL CALLBACK EnumDestroy(HWND child, LPARAM param);

void UpdateTitle(HWND hWnd);

void PauseGame(HWND hWnd);
void ResumeGame(HWND hWnd);
void TogglePause(HWND hWnd);