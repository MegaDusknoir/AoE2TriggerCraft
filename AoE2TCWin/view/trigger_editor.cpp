
#include "../framework.h"
#include "../AoE2TCWin.h"
#include "editors.h"
#include <vector>

using std::vector;

WNDPROC TVWndProc;

class TreeItemIterator
{
public:
    TreeItemIterator(HWND _treeview) : treeview(_treeview), item(TreeView_GetNextItem(_treeview, NULL, TVGN_CARET)) {}
    TreeItemIterator(HWND _treeview, HTREEITEM _item) : treeview(_treeview), item(_item) {}
private:
    HTREEITEM item;
    HWND const treeview;
public:
    TreeItemIterator& operator++()
    {
        item = TreeView_GetNextItem(treeview, item, TVGN_NEXT);
        return *this;
    }
    TreeItemIterator& operator++(int)
    {
        auto ret = *this;
        item = TreeView_GetNextItem(treeview, item, TVGN_NEXT);
        return ret;
    }
    TreeItemIterator& operator--()
    {
        item = TreeView_GetNextItem(treeview, item, TVGN_PREVIOUS);
        return *this;
    }
    TreeItemIterator& operator--(int)
    {
        auto ret = *this;
        item = TreeView_GetNextItem(treeview, item, TVGN_PREVIOUS);
        return ret;
    }
    operator HTREEITEM()
    {
        return item;
    }
};

HTREEITEM TreeAddTrig(HWND treeview, uint32_t index, HTREEITEM after)
{
    TVINSERTSTRUCT tvis;
    HTREEITEM trignode;	//parent of condition/effect nodes
    auto& trig = Scen.triggers->at(index);

    /* These paramters are for both triggers and conditions/effects */
    tvis.hInsertAfter = after;
    tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

    /* First add the trigger node. */
    std::wstring trig_name(win32::Utf8ToUtf16(trig.trigger_name) + L'\0');
    tvis.hParent = TVI_ROOT;
    tvis.item.lParam = index;
    tvis.item.pszText = &trig_name[0];		//use TVN_GETDISPINFO.
    tvis.item.iImage = 0;
    tvis.item.iSelectedImage = 0;
    trignode = TreeView_InsertItem(treeview, &tvis);

    /* Then add the condition/effect nodes */
    tvis.hParent = trignode;

    for (size_t i = 0; i != trig.condition_data.size(); ++i)
    {
        std::wstring name(GameStr.condition_name[trig.condition_data[i].condition_type]);
        tvis.item.pszText = &name[0];
        tvis.item.iImage = 1;
        tvis.item.iSelectedImage = 1;
        SendMessage(treeview, TVM_INSERTITEM, 0, (LPARAM)&tvis);
    }
    for (size_t i = 0; i != trig.effect_data.size(); ++i)
    {
        std::wstring name(GameStr.effect_name[trig.effect_data[i].effect_type]);
        tvis.item.pszText = &name[0];
        tvis.item.iImage = 2;
        tvis.item.iSelectedImage = 2;
        SendMessage(treeview, TVM_INSERTITEM, 0, (LPARAM)&tvis);
    }
    return trignode;
}

void TreeReset(HWND treeview, bool refresh)
{
    HCURSOR wait, previous;

    //wait = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_WAIT), IMAGE_CURSOR, 0, 0, LR_SHARED);
    //previous = SetCursor(wait);
    //LockWindowUpdate(treeview);
    //c_trig = NULL;	//since there's no guarantee it exists anymore
    TreeView_SelectItem(treeview, NULL);  //prevents loading every single trigger during deletion
    TreeView_DeleteItem(treeview, TVI_ROOT);
    //LockWindowUpdate(NULL);
    //SetCursor(previous);
    if (refresh)
    {
        for (uint32_t i = 0; i != Scen.triggers->size(); ++i)
        {
            TreeAddTrig(treeview, i, TVI_LAST);
        }
    }
}

LPARAM GetItemParam(HWND treeview, HTREEITEM which)
{
    TVITEM item;

    item.mask = TVIF_HANDLE | TVIF_PARAM;
    item.hItem = which ? which : TreeView_GetNextItem(treeview, NULL, TVGN_CARET);
    item.lParam = 0; //why was -1?

    if (item.hItem)	//TVM_GETITIEM does NOT have NULL-protection
        TreeView_GetItem(treeview, &item);

    return item.lParam;
}

BOOL SetItemParam(HWND treeview, HTREEITEM which, LPARAM param)
{
    TVITEM item;

    item.mask = TVIF_HANDLE | TVIF_PARAM;
    item.hItem = which ? which : TreeView_GetNextItem(treeview, NULL, TVGN_CARET);
    item.lParam = param; //why was -1?

    if (item.hItem)	//TVM_GETITIEM does NOT have NULL-protection
        return TreeView_SetItem(treeview, &item);

    return FALSE;
}

INT_PTR Handle_WM_COMMAND(HWND dialog, WORD code, WORD id, HWND)
{
    HWND treeview = GetDlgItem(dialog, IDC_TRIGGER_TREE);	//all use this

    switch (code)
    {
    case EN_CHANGE:
        switch (id)
        {
        }
        break;
    case BN_CLICKED:
    case CBN_SELCHANGE:
        switch (id)
        {
        case IDC_ADD_TRIG:
        {
            Scen.triggers->at(Scen.triggers->add()).trigger_name = win32::Utf16ToUtf8(make_fmt_string(L"Trigger %d", Scen.triggers->size()));
            TreeAddTrig(treeview, Scen.triggers->size() - 1, TVI_LAST);
            break;
        }
        case IDC_DEL_TRIG:
        {
            //HTREEITEM target = TreeView_GetNextItem(treeview, 0, TVGN_CARET);
            //HTREEITEM next = TreeView_GetNextItem(treeview, target, TVGN_NEXT);
            //TreeView_DeleteItem(treeview, target);
            //TreeView_Select(treeview, next, TVGN_CARET);
            //Todo
            //Scen.triggers->at(GetItemParam(treeview, target));
            //Scen.triggers->at(Scen.triggers->add());
            //TreeAddTrig(treeview, Scen.triggers->size() - 1, TVI_LAST);
            break;
        }
        case IDC_COPY_TO_ALL:
        {
            TreeItemIterator target(treeview);
            auto idx = GetItemParam(treeview, target);//Todo
            auto old_end = Scen.triggers->size();
            if (target)
            {
                auto added_cnt = Scen.triggers->copy_to_all(idx, CP_MOD_SOURCE | CP_MOD_TARGET | CP_STRICT_BASE);
                for (size_t i = 1; i <= added_cnt; ++i)
                {
                    TreeAddTrig(treeview, idx + i, target);
                    ++target;
                }
                ++target;
                TreeView_Select(treeview, target, TVGN_CARET);
                do
                {
                    SetItemParam(treeview, target, GetItemParam(treeview, target) + added_cnt);
                } while (++target);
            }
            break;
        }
        default:
            break;
        }
        break;
    case EN_SETFOCUS:
        //EnableMenuItem(propdata.menu, ID_TS_EDIT_COPY, MF_ENABLED);
        //EnableMenuItem(propdata.menu, ID_TS_EDIT_CUT, MF_ENABLED);
        //if (IsClipboardFormatAvailable(CF_TEXT) || editor_count)
        //	EnableMenuItem(propdata.menu, ID_TS_EDIT_PASTE, MF_ENABLED);

        //PaintCurrent();
        break;

    case EN_KILLFOCUS:
        //EnableMenuItem(propdata.menu, ID_TS_EDIT_COPY, MF_GRAYED);
        //EnableMenuItem(propdata.menu, ID_TS_EDIT_CUT, MF_GRAYED);
        //EnableMenuItem(propdata.menu, ID_TS_EDIT_PASTE, MF_GRAYED);
        break;
    default:
        break;
    }

    // "If an application processes this message, it should return zero."
    return 0;
}

INT_PTR Handle_WM_NOTIFY(HWND dialog, NMHDR const* header)
{
    INT_PTR ret = FALSE;   // reasonable default?

    switch (header->code)
    {
    case TVN_GETDISPINFO:
        //TrigTree_HandleGetDispInfo((NMTVDISPINFO*)header);
        break;

        //case TVN_BEGINLABELEDIT:
        //{
        //	const NMTVDISPINFO* dispinfo = (NMTVDISPINFO*)header;
        //	class ItemData* data = (class ItemData*)dispinfo->item.lParam;
        //	HWND editbox = TreeView_GetEditControl(header->hwndFrom);

        //	if (data->type != TRIGGER)
        //	{
        //		SetWindowLongPtr(dialog, DWLP_MSGRESULT, TRUE);	//cancel the editing
        //		ret = TRUE;
        //	}
        //	else
        //	{
        //		SendMessage(editbox, WM_SETTEXT, NULL, (LPARAM)TEXT(scen.triggers.at(data->index).name));
        //		SendMessage(editbox, EM_SETLIMITTEXT, MAX_TRIGNAME_SAFE_HD - 1, 0);
        //		SubclassTreeEditControl(editbox);
        //	}
        //}
        //break;

        //case TVN_ENDLABELEDIT:
        //{
        //	const NMTVDISPINFO* dispinfo = (NMTVDISPINFO*)header;
        //	class ItemData* data = (class ItemData*)dispinfo->item.lParam;

        //	LPTSTR newname = dispinfo->item.pszText;

        //	if (newname)
        //	{
        //		ret = TRUE;	//activates DWLP_MSGRESULT

        //		strcpy(scen.triggers.at(data->index).name, newname);
        //	}
        //}
        //break;
        ///*
        //   case TVN_SETDISPINFO:
        //   _ASSERT(false);
        //   break;
        //   */
        //case TVN_SELCHANGED:
        //	TrigTree_HandleSelChanged((NMTREEVIEW*)header, dialog);
        //	break;

        //case TVN_KEYDOWN:
        //	TrigTree_HandleKeyDown(dialog, (LPNMTVKEYDOWN)header);
        //	break;

        //case TVN_DELETEITEM:
        //{
        //	const NMTREEVIEW* info = (NMTREEVIEW*)header;
        //	class ItemData* data = (class ItemData*)info->itemOld.lParam;

        //	delete data;
        //}
        //break;

        //case TVN_BEGINDRAG:
        //	TrigTree_HandleDrag(header->hwndFrom, (NMTREEVIEW*)header);
        //	break;

        //case PSN_SETACTIVE:
        //	LoadTriggerSection(dialog);
        //	TrigTree_Reset(GetDlgItem(dialog, IDC_T_TREE), true);
        //	SetWindowText(propdata.statusbar, help_msg);
        //	ret = FALSE;
        //	break;

        //case PSN_KILLACTIVE:
        //	SaveTriggerSection(dialog);
        //	ret = IDOK;
        //	if (editor_count)
        //	{
        //		ret = MessageBox(dialog,
        //			warningEditorsClosing, szTrigTitle, MB_ICONWARNING | MB_OKCANCEL);
        //	}

        //	if (ret == IDOK)
        //	{
        //		//Gray menu items for next window.
        //		Triggers_EditMenu(propdata.menu, false);

        //		/*	Remove selection because this page has no clue what the user
        //			does while it's not active. Note that the following line ends
        //			up saving the current trigger. */
        //		TreeView_SelectItem(GetDlgItem(dialog, IDC_T_TREE), NULL);
        //		c_trig = NULL;
        //	}
        //	SetWindowLongPtr(dialog, DWLP_MSGRESULT, ret != IDOK);	//false to continue
        //	ret = TRUE;
        //	break;
    }

    return ret;
}

BOOL MakeTrigDlg(HWND dialog)
{
    HIMAGELIST il;
    HWND treeview;
    HINSTANCE inst = GetModuleHandle(NULL);
    HBITMAP icon_trig, icon_cond, icon_eff;

    /* Setup image list */
    il = ImageList_Create(16, 16, ILC_COLOR, 12, 1);
    icon_trig = (HBITMAP)LoadImage(inst, MAKEINTRESOURCE(IDB_TRIGGER), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
    icon_cond = (HBITMAP)LoadImage(inst, MAKEINTRESOURCE(IDB_CONDITION), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
    icon_eff = (HBITMAP)LoadImage(inst, MAKEINTRESOURCE(IDB_EFFECT), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

    ImageList_Add(il, icon_trig, NULL);
    ImageList_Add(il, icon_cond, NULL);
    ImageList_Add(il, icon_eff, NULL);

    treeview = GetDlgItem(dialog, IDC_TRIGGER_TREE);
    TreeView_SetImageList(treeview, il, TVSIL_NORMAL);
    SetWindowLong(treeview, GWL_STYLE, GetWindowLong(treeview, GWL_STYLE) | TVS_HASBUTTONS | TVS_HASLINES);

    return TRUE;
}

INT_PTR CALLBACK TrigDlgProc(HWND dialog, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        MakeTrigDlg(dialog);
        break;
    }
    case WM_COMMAND:
    {
        Handle_WM_COMMAND(
            dialog, HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
        break;
    }
    case WM_NOTIFY:
    {
        Handle_WM_NOTIFY(dialog, (NMHDR*)lParam);
        break;
    }
    case TC_LOAD:
    {
        TreeReset(GetDlgItem(dialog, IDC_TRIGGER_TREE), true);
        break;
    }
    default:
        break;
    }

    return 0;
}