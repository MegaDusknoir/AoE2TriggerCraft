
#include "../framework.h"
#include "../AoE2TCWin.h"
#include "viewers.h"
#include <vector>

std::string time_format(time_t timestamp)
{
    tm* stamp_tm = localtime(&timestamp);
    std::string ret;
    ret.resize(24);
    sprintf(&ret[0], "%4d/%d/%d, %02d:%02d:%02d",
        stamp_tm->tm_year + 1900,
        stamp_tm->tm_mon + 1,
        stamp_tm->tm_mday,
        stamp_tm->tm_hour,
        stamp_tm->tm_min,
        stamp_tm->tm_sec);
    return ret;
}

INT_PTR CALLBACK ScenInfoDlgProc(HWND dialog, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        break;
    }
    case WM_COMMAND:
    {
        break;
    }
    case WM_NOTIFY:
    {
        break;
    }
    case TC_LOAD:
    {
        SetDlgItemText(dialog, IDC_TEXT_VERSION, win32::Utf8ToUtf16(Scen.scen.header.version).c_str());
        SetDlgItemText(dialog, IDC_TEXT_AUTHOR, win32::Utf8ToUtf16(Scen.scen.header.creator_name).c_str());
        SetDlgItemText(dialog, IDC_TEXT_TIMESTAMP, win32::Utf8ToUtf16(time_format(Scen.scen.header.timestamp_of_last_save)).c_str());
        SetDlgItemText(dialog, IDC_TEXT_TRIGVERSION, std::to_wstring(round(Scen.scen.body.Triggers.trigger_version)).c_str());
        SetDlgItemText(dialog, IDC_TEXT_INSTRUCTION, win32::Utf8ToUtf16(Scen.scen.header.scenario_instructions.cr_to_crlf()).c_str());
        break;
    }
    default:
        break;
    }

    return 0;
}