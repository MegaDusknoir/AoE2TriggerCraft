#pragma once

#include "resource.h"
#include "../aoe2scenario.h"
#include "../util/utf8conv/utf8conv.h"
#include "../aoejson.h"
#include <map>

class StringMap
{
public:
    void alloc(UINT uID, int cchBufferMax);
    WCHAR* operator[](UINT uID);
private:
    std::map<size_t, std::unique_ptr<WCHAR[]>> map;
};

enum TC_MSG
{
	TC_LOAD = WM_APP,
	TC_SAVE,
	TC_CLOSE,
};

extern StringMap stMap;
extern AoEJsonData GameStr;
extern AoE2ScenarioNamespace::AoE2Scenario Scen;

HWND CreateToolTip(int toolID, HWND hDlg, PCTSTR pszText);

#define MAX_LOADSTRING 100
