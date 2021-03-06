
#include "../framework.h"
#include "../AoE2TCWin.h"
#include "infos.h"

#define INFO_SHEET_NUM_PAGES 3

#define SHEET_TRIGGER 0
#define SHEET_CONDITION 1
#define SHEET_EFFECT 2

LONG_PTR pInfoProc;
static DLGPROC procs[INFO_SHEET_NUM_PAGES] =
{
    &TrigParamDlgProc,
    &TrigParamDlgProc,
    &TrigParamDlgProc,
};
const WORD PropSheetButtons[] = { IDOK, IDCANCEL, IDHELP };

int CALLBACK InfoSheetProc(HWND sheet, UINT uMsg, LPARAM lParam)
{
    switch (uMsg)
    {
    case PSCB_PRECREATE:
    {
        RECT rect;
        GetClientRect(hInfo, &rect);
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

INT_PTR CALLBACK InfoProc(HWND sheet, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case TC_LOAD:
        //SendMessage((HWND)SendMessage(sheet, PSM_GETCURRENTPAGEHWND, 0, 0), TC_LOAD, 0, 0);
        SendMessage(PropSheet_IndexToHwnd(sheet, SHEET_TRIGGER), TC_LOAD, 0, 0);
        SendMessage(PropSheet_IndexToHwnd(sheet, SHEET_CONDITION), TC_LOAD, 0, 0);
        SendMessage(PropSheet_IndexToHwnd(sheet, SHEET_EFFECT), TC_LOAD, 0, 0);
        break;
    case TC_LOAD_PARAM:
        switch (((ItemData*)lParam)->type)
        {
        case ItemData::TRIGGER:
            SendMessage(PropSheet_IndexToHwnd(sheet, SHEET_TRIGGER), TC_LOAD_PARAM, 0, lParam);
            break;
        case ItemData::CONDITION:
            SendMessage(PropSheet_IndexToHwnd(sheet, SHEET_CONDITION), TC_LOAD_PARAM, 0, lParam);
            break;
        case ItemData::EFFECT:
            SendMessage(PropSheet_IndexToHwnd(sheet, SHEET_EFFECT), TC_LOAD_PARAM, 0, lParam);
            break;
        default:
            break;
        }
        break;
    default:
        return CallWindowProc((WNDPROC)pInfoProc, sheet, uMsg, wParam, lParam);
    }
    return 0;
}

HWND MakeInfoSheet(HINSTANCE app)
{
    PROPSHEETHEADER header;
    HPROPSHEETPAGE pages[INFO_SHEET_NUM_PAGES];
    PROPSHEETPAGE pg;	//used to create each page
    HWND sheet;

    //create pages

    pg.dwSize = sizeof(PROPSHEETPAGE);
    pg.dwFlags = PSP_DEFAULT | PSP_HIDEHEADER;
    pg.hInstance = app;

    for (int i = 0; i < INFO_SHEET_NUM_PAGES; i++)
    {
        pg.pszTemplate = MAKEINTRESOURCE(IDD_PARAM_TRIGGER + i);	//template IDs are in display order
        pg.pfnDlgProc = procs[i];
        pg.lParam = 0;
        pages[i] = CreatePropertySheetPage(&pg);
    }

    //create sheet

    header.dwSize = sizeof(header);
    header.dwFlags = PSH_MODELESS | PSH_USECALLBACK | PSH_NOMARGIN |
        PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;
    header.hwndParent = hInfo;
    header.hInstance = app;
    header.pszCaption = szInfoWindowClass;
    header.nPages = INFO_SHEET_NUM_PAGES;
    header.nStartPage = 0;
    header.phpage = pages;

    header.pfnCallback = &InfoSheetProc;

    sheet = (HWND)PropertySheet(&header);

    pInfoProc = SetWindowLongPtr(sheet, DWLP_DLGPROC, (LONG_PTR)&InfoProc);

    return sheet;
}
