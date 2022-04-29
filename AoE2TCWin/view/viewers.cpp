
#include "../framework.h"
#include "../AoE2TCWin.h"
#include "viewers.h"

#define VIEWER_SHEET_NUM_PAGES 3
LONG_PTR pViewerProc;
static DLGPROC procs[VIEWER_SHEET_NUM_PAGES] =
{
    &MapViewDlgProc,
    &ScenInfoDlgProc,
    &PlayerViewDlgProc,
};

const WORD PropSheetButtons[] = { IDOK, IDCANCEL, IDHELP };

int CALLBACK ViewerSheetProc(HWND sheet, UINT uMsg, LPARAM lParam)
{
    switch (uMsg)
    {
    case PSCB_PRECREATE:
    {
        RECT rect;
        GetClientRect(hViewer, &rect);
        DLGTEMPLATE* templ = (DLGTEMPLATE*)lParam;
        templ->style |= WS_OVERLAPPED | WS_CHILD | WS_VISIBLE | WS_MAXIMIZE;
        templ->style &= ~(WS_POPUP | WS_BORDER | DS_CENTER | DS_MODALFRAME | DS_3DLOOK | DS_ABSALIGN | WS_CAPTION | WS_DLGFRAME | WS_SYSMENU);
        templ->x = (short)rect.left;
        templ->y = (short)rect.top;
        templ->cx = (short)rect.right;
        templ->cy = (short)rect.bottom;
    }
    break;
    case PSCB_INITIALIZED:
    {
        /* Remove unused buttons. */
        for (int i = 0; i < sizeof(PropSheetButtons) / sizeof(WORD); i++)
        {
            HWND hWnd = GetDlgItem(sheet, PropSheetButtons[i]);
            if (hWnd != NULL)
            {
                ShowWindow(hWnd, SW_HIDE);
                EnableWindow(hWnd, FALSE);
            }
        }
        break;
    }
    default:
        return 0;
    }
    return 0;
}

INT_PTR CALLBACK ViewerProc(HWND sheet, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case TC_LOAD:
        //SendMessage((HWND)SendMessage(sheet, PSM_GETCURRENTPAGEHWND, 0, 0), TC_LOAD, 0, 0);
        SendMessage(PropSheet_IndexToHwnd(sheet, 0), TC_LOAD, 0, 0);
        SendMessage(PropSheet_IndexToHwnd(sheet, 1), TC_LOAD, 0, 0);
        SendMessage(PropSheet_IndexToHwnd(sheet, 2), TC_LOAD, 0, 0);
        break;
    default:
        return CallWindowProc((WNDPROC)pViewerProc, sheet, uMsg, wParam, lParam);
    }
    return 0;
}

extern WCHAR szEditorWindowClass[MAX_LOADSTRING];
HWND MakeViewerSheet(HINSTANCE app)
{
    PROPSHEETHEADER header;
    HPROPSHEETPAGE pages[VIEWER_SHEET_NUM_PAGES];
    PROPSHEETPAGE pg;	//used to create each page
    HWND sheet;

    //create pages

    pg.dwSize = sizeof(PROPSHEETPAGE);
    pg.dwFlags = PSP_DEFAULT | PSP_HIDEHEADER;
    pg.hInstance = app;

    for (int i = 0; i < VIEWER_SHEET_NUM_PAGES; i++)
    {
        pg.pszTemplate = MAKEINTRESOURCE(IDD_MAPVIEW + i);	//template IDs are in display order
        pg.pfnDlgProc = procs[i];
        pg.lParam = 0;
        pages[i] = CreatePropertySheetPage(&pg);
    }

    //create sheet

    header.dwSize = sizeof(header);
    header.dwFlags = PSH_MODELESS | PSH_USECALLBACK | PSH_NOMARGIN |
        PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;
    header.hwndParent = hViewer;
    header.hInstance = app;
    header.pszCaption = szViewerWindowClass;
    header.nPages = VIEWER_SHEET_NUM_PAGES;
    header.nStartPage = 1;
    header.phpage = pages;

    header.pfnCallback = &ViewerSheetProc;

    sheet = (HWND)PropertySheet(&header);

    pViewerProc = SetWindowLongPtr(sheet, DWLP_DLGPROC, (LONG_PTR)&ViewerProc);

    return sheet;
}
