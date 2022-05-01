#pragma once

#include "resource.h"
#include "../model/aoe2scenario.h"
#include "../util/utf8conv/utf8conv.h"
#include "../model/aoejson.h"
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
	TC_LOAD_PARAM,
};

class ItemData
{
public:
    enum DataType
    {
        NONE,
        TRIGGER,
        CONDITION,
        EFFECT,
    };

    ItemData(DataType type, uint32_t index) :type(type), idx(index)
    {}
    DataType type;
    uint32_t idx;
};

extern StringMap stMap;
extern AoEJsonData GameStr;
extern AoE2ScenarioNamespace::AoE2Scenario Scen;
extern HWND hEditor;
extern HWND hViewer;
extern HWND hInfo;

HWND CreateToolTip(int toolID, HWND hDlg, PCTSTR pszText);

#define MAX_LOADSTRING 100
