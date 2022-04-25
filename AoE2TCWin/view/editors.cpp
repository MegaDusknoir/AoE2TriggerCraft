
#include "../framework.h"
#include "../AoE2TCWin.h"
#include "editors.h"

#define EDITOR_SHEET_NUM_PAGES 2
DLGPROC procs[EDITOR_SHEET_NUM_PAGES] =
{
    &TrigDlgProc,
    &TrigDlgProc,
};
const WORD PropSheetButtons[] = { IDOK, IDCANCEL, IDHELP };

int CALLBACK EditorSheetProc(HWND sheet, UINT uMsg, LPARAM lParam)
{
    switch (uMsg)
    {
    case PSCB_PRECREATE:
    {
        RECT rect;
        GetClientRect(hEditor, &rect);
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

HWND MakeEditorSheet(HINSTANCE app)
{
    PROPSHEETHEADER header;
    HPROPSHEETPAGE pages[EDITOR_SHEET_NUM_PAGES];
    PROPSHEETPAGE pg;	//used to create each page
    HWND sheet;

    //create pages

    pg.dwSize = sizeof(PROPSHEETPAGE);
    pg.dwFlags = PSP_DEFAULT | PSP_HIDEHEADER;
    pg.hInstance = app;

    for (int i = 0; i < EDITOR_SHEET_NUM_PAGES; i++)
    {
        pg.pszTemplate = MAKEINTRESOURCE(IDD_TRIGGERS + i);	//template IDs are in display order
        pg.pfnDlgProc = procs[i];
        pg.lParam = 0;
        pages[i] = CreatePropertySheetPage(&pg);
    }

    //create sheet

    header.dwSize = sizeof(header);
    header.dwFlags = PSH_MODELESS | PSH_USECALLBACK | PSH_NOMARGIN |
        PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;
    header.hwndParent = hEditor;
    header.hInstance = app;
    header.pszCaption = szEditorWindowClass;
    header.nPages = EDITOR_SHEET_NUM_PAGES;
    header.nStartPage = 0;
    header.phpage = pages;

    header.pfnCallback = &EditorSheetProc;

    sheet = (HWND)PropertySheet(&header);

    return sheet;
}
