#pragma once

#include "../framework.h"

extern HWND hInfo;
extern WCHAR szInfoWindowClass[MAX_LOADSTRING];

HWND MakeInfoSheet(HINSTANCE app);

INT_PTR CALLBACK TrigParamDlgProc(HWND dialog, UINT uMsg, WPARAM wParam, LPARAM lParam);