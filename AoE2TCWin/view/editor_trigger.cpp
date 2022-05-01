
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
private:
    TreeItemIterator& _increase(int operand)
    {
        assert(operand >= 0);
        while (operand != 0 && item != nullptr)
        {
            item = TreeView_GetNextItem(treeview, item, TVGN_NEXT);
            --operand;
        }
        return *this;
    }
    TreeItemIterator& _decrease(int operand)
    {
        assert(operand >= 0);
        while (operand != 0 && item != nullptr)
        {
            item = TreeView_GetNextItem(treeview, item, TVGN_PREVIOUS);
            --operand;
        }
        return *this;
    }
public:
    TreeItemIterator& parent()
    {
        item = TreeView_GetNextItem(treeview, item, TVGN_PARENT);
        return *this;
    }
    TreeItemIterator& child()
    {
        item = TreeView_GetNextItem(treeview, item, TVGN_CHILD);
        return *this;
    }
    TreeItemIterator get_parent()
    {
        TreeItemIterator ret(treeview, TreeView_GetNextItem(treeview, item, TVGN_PARENT));
        return ret;
    }
    TreeItemIterator get_child()
    {
        TreeItemIterator ret(treeview, TreeView_GetNextItem(treeview, item, TVGN_CHILD));
        return ret;
    }
    LPARAM lparam()
    {
        TVITEM tvitem;

        tvitem.mask = TVIF_HANDLE | TVIF_PARAM;
        tvitem.hItem = item ? item : TreeView_GetNextItem(treeview, NULL, TVGN_CARET);
        tvitem.lParam = NULL;

        if (tvitem.hItem)	//TVM_GETITIEM does NOT have NULL-protection
            TreeView_GetItem(treeview, &tvitem);

        return tvitem.lParam;
    }
    TreeItemIterator& operator+=(int operand)
    {
        return operand >= 0 ? this->_increase(operand) : this->_decrease(operand);
    }
    TreeItemIterator& operator-=(int operand)
    {
        return operand >= 0 ? this->_decrease(operand) : this->_increase(operand);
    }
    TreeItemIterator operator+(int operand)
    {
        auto ret = *this;
        return operand >= 0 ? ret._increase(operand) : ret._decrease(operand);
    }
    TreeItemIterator operator-(int operand)
    {
        auto ret = *this;
        return operand >= 0 ? ret._decrease(operand) : ret._increase(operand);
    }
    TreeItemIterator& operator++()
    {
        item = TreeView_GetNextItem(treeview, item, TVGN_NEXT);
        return *this;
    }
    TreeItemIterator operator++(int)
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
    TreeItemIterator operator--(int)
    {
        auto ret = *this;
        item = TreeView_GetNextItem(treeview, item, TVGN_PREVIOUS);
        return ret;
    }
    bool operator!=(const TreeItemIterator& rhs)
    {
        return this->item != rhs.item || this->treeview != rhs.treeview;
    }
    bool operator==(const TreeItemIterator& rhs)
    {
        return !(*this != rhs);
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
    tvis.item.lParam = (LPARAM)new ItemData(ItemData::TRIGGER, index);
    tvis.item.pszText = &trig_name[0];		//use TVN_GETDISPINFO.
    tvis.item.iImage = 0;
    tvis.item.iSelectedImage = 0;
    trignode = TreeView_InsertItem(treeview, &tvis);

    /* Then add the condition/effect nodes */
    tvis.hParent = trignode;

    for (size_t i = 0; i != trig.condition_data.size(); ++i)
    {
        std::wstring name(GameStr.condition_name[trig.condition_data[i].condition_type]);
        tvis.item.lParam = (LPARAM)new ItemData(ItemData::CONDITION, i);
        tvis.item.pszText = &name[0];
        tvis.item.iImage = 1;
        tvis.item.iSelectedImage = 1;
        SendMessage(treeview, TVM_INSERTITEM, 0, (LPARAM)&tvis);
    }
    for (size_t i = 0; i != trig.effect_data.size(); ++i)
    {
        std::wstring name(GameStr.effect_name[trig.effect_data[i].effect_type]);
        tvis.item.lParam = (LPARAM)new ItemData(ItemData::EFFECT, i);
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
            HTREEITEM new_trig;
            Scen.triggers->at(Scen.triggers->add()).trigger_name = win32::Utf16ToUtf8(make_fmt_string(stMap[IDS_NAME_NEW_TRIGGER], Scen.triggers->size()));
            TreeItemIterator select(treeview);
            if (select)
            {
                auto select_item = (ItemData*)select.lparam();
                if (select_item->type != ItemData::TRIGGER)
                {
                    select.parent();
                    select_item = (ItemData*)select.lparam();
                }
                auto idx_end = Scen.triggers->size();
                Scen.triggers->mov(select_item->idx + 1, idx_end - 1, idx_end);
                auto to_iter = select;
                while (++to_iter)
                {
                    ++(((ItemData*)to_iter.lparam())->idx);
                }
                new_trig = TreeAddTrig(treeview, select_item->idx + 1, select);
            }
            else
            {
                new_trig = TreeAddTrig(treeview, Scen.triggers->size() - 1, TVI_LAST);
            }
            TreeView_Select(treeview, new_trig, TVGN_CARET);
            SetFocus(treeview);
            break;
        }
        case IDC_DEL_TRIG:
        {
            TreeItemIterator select(treeview);
            if (select)
            {
                auto select_item = (ItemData*)select.lparam();
                HTREEITEM next_item = select + 1 ? select + 1 : select - 1;
                switch (select_item->type)
                {
                case ItemData::TRIGGER:
                {
                    Scen.triggers->del(select_item->idx);
                    auto to_iter = select;
                    while (++to_iter)
                    {
                        --(((ItemData*)to_iter.lparam())->idx);
                    }
                    TreeView_DeleteItem(treeview, select);
                    TreeView_Select(treeview, next_item, TVGN_CARET);
                    break;
                }
                default:
                    break;
                }
                SetFocus(treeview);
            }
            break;
        }
        case IDC_COPY_TO_ALL:
        {
            TreeItemIterator target(treeview);
            if (target)
            {
                auto select_item = (ItemData*)target.lparam();
                switch (select_item->type)
                {
                case ItemData::TRIGGER:
                {
                    auto old_end = Scen.triggers->size();
                    auto added_cnt = Scen.triggers->copy_to_all(select_item->idx, CP_MOD_SOURCE | CP_MOD_TARGET | CP_STRICT_BASE);
                    for (size_t i = 1; i <= added_cnt; ++i)
                    {
                        TreeAddTrig(treeview, select_item->idx + i, target);
                        ++target;
                    }
                    ++target;
                    TreeView_Select(treeview, target, TVGN_CARET);
                    do
                    {
                        ((ItemData*)target.lparam())->idx += added_cnt;
                    } while (++target);
                    SetFocus(treeview);
                    break;
                }
                case ItemData::CONDITION:
                    break;
                case ItemData::EFFECT:
                    break;
                default:
                    break;
                }
            }
            break;
        }
        case IDC_SORT:
        {
            Scen.triggers->sort_by_order();
            MessageBox(NULL,stMap[IDS_SORT_COMPLETE], stMap[TIPS_TRIGGER_SORT], NULL);
            //TreeReset(treeview, true);
            SetFocus(treeview);
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
    case TVN_SELCHANGED:
        if (((NMTREEVIEW*)header)->itemNew.lParam)
        {
            SendMessage(hInfo, TC_LOAD_PARAM, 0, ((NMTREEVIEW*)header)->itemNew.lParam);
        }
        break;

        //case TVN_KEYDOWN:
        //	TrigTree_HandleKeyDown(dialog, (LPNMTVKEYDOWN)header);
        //	break;

    case TVN_DELETEITEM:
    {
        const NMTREEVIEW* info = (NMTREEVIEW*)header;
        ItemData* data = (ItemData*)info->itemOld.lParam;
        delete data;
        break;
    }

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

    SendMessage(GetDlgItem(dialog, IDC_ADD_TRIG), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadIcon(inst, MAKEINTRESOURCE(IDI_B_NEW_TRIG)));
    SendMessage(GetDlgItem(dialog, IDC_DEL_TRIG), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadIcon(inst, MAKEINTRESOURCE(IDI_B_DEL_TRIG)));
    SendMessage(GetDlgItem(dialog, IDC_COPY_TO_ALL), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadIcon(inst, MAKEINTRESOURCE(IDI_B_COPY_TO_ALL)));
    SendMessage(GetDlgItem(dialog, IDC_SORT), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadIcon(inst, MAKEINTRESOURCE(IDI_B_SORT)));

    //SendMessage(GetDlgItem(dialog, IDC_ADD_TRIG), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)icon_trig);
    CreateToolTip(IDC_ADD_TRIG, dialog, stMap[TIPS_ADD_TRIGGER]);
    CreateToolTip(IDC_DEL_TRIG, dialog, stMap[TIPS_DEL_TREEITEM]);
    CreateToolTip(IDC_COPY_TO_ALL, dialog, stMap[TIPS_TRIGGER_COPY_TO_ALL]);
    CreateToolTip(IDC_SORT, dialog, stMap[TIPS_TRIGGER_SORT]);

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