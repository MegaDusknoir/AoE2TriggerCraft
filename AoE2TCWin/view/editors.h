#pragma once

#include "../framework.h"

extern HWND hEditor;
extern WCHAR szEditorWindowClass[MAX_LOADSTRING];

HWND MakeEditorSheet(HINSTANCE app);

INT_PTR CALLBACK TrigDlgProc(HWND dialog, UINT uMsg, WPARAM wParam, LPARAM lParam);