
#include "../framework.h"
#include "../AoE2TCWin.h"
#include "infos.h"
#include <vector>

void LoadTrigger(HWND dialog, ItemData* pdata)
{
    if (pdata->type == ItemData::TRIGGER)
    {
        auto& trig = Scen.triggers->at(pdata->idx);
        CheckDlgButton(dialog, IDC_PT_ENABLE, trig.enabled);
        CheckDlgButton(dialog, IDC_PT_LOOP, trig.looping);
        CheckDlgButton(dialog, IDC_PT_AS_TITLE, trig.make_header);
        CheckDlgButton(dialog, IDC_PT_MUTE, trig.mute_objectives);
        CheckDlgButton(dialog, IDC_PT_AS_OBJ, trig.display_as_objective);
        CheckDlgButton(dialog, IDC_PT_ON_SCREEN, trig.display_on_screen);
        SetDlgItemText(dialog, IDC_PT_NAME, win32::Utf8ToUtf16(trig.trigger_name.cr_to_crlf()).c_str());
        SetDlgItemText(dialog, IDC_PT_DESC, win32::Utf8ToUtf16(trig.trigger_description.cr_to_crlf()).c_str());
        SetDlgItemText(dialog, IDC_PT_SHORT_DESC, win32::Utf8ToUtf16(trig.short_description.cr_to_crlf()).c_str());
        SetDlgItemInt(dialog, IDC_PT_ORDER, trig.objective_description_order, TRUE);
        SetDlgItemInt(dialog, IDC_PT_DESC_STR, trig.description_string_table_id, TRUE);
        SetDlgItemInt(dialog, IDC_PT_SHORT_DESC_STR, trig.short_description_string_table_id, TRUE);
    }
    else
    {
        /* Reset values. */
        CheckDlgButton(dialog, IDC_PT_ENABLE, BST_UNCHECKED);
        CheckDlgButton(dialog, IDC_PT_LOOP, BST_UNCHECKED);
        CheckDlgButton(dialog, IDC_PT_AS_TITLE, BST_UNCHECKED);
        CheckDlgButton(dialog, IDC_PT_MUTE, BST_UNCHECKED);
        CheckDlgButton(dialog, IDC_PT_AS_OBJ, BST_UNCHECKED);
        CheckDlgButton(dialog, IDC_PT_ON_SCREEN, BST_UNCHECKED);
        SetDlgItemText(dialog, IDC_PT_NAME, L"");
        SetDlgItemText(dialog, IDC_PT_DESC, L"");
        SetDlgItemText(dialog, IDC_PT_SHORT_DESC, L"");
        SetDlgItemText(dialog, IDC_PT_ORDER, L"");
        SetDlgItemText(dialog, IDC_PT_DESC_STR, L"");
        SetDlgItemText(dialog, IDC_PT_SHORT_DESC_STR, L"");
    }
}

INT_PTR CALLBACK TrigParamDlgProc(HWND dialog, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
    case TC_LOAD_PARAM:
    {
        LoadTrigger(dialog, (ItemData*)lParam);
        break;
    }
    default:
        break;
    }

    return 0;
}