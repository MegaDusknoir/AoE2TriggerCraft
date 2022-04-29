#pragma once

#include "../framework.h"

extern HWND hViewer;
extern WCHAR szViewerWindowClass[MAX_LOADSTRING];

HWND MakeViewerSheet(HINSTANCE app);

INT_PTR CALLBACK ScenInfoDlgProc(HWND dialog, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK MapViewDlgProc(HWND dialog, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK PlayerViewDlgProc(HWND dialog, UINT uMsg, WPARAM wParam, LPARAM lParam);
