#pragma once

#include "resource.h"
#include "../aoe2scenario.h"
#include "../util/utf8conv/utf8conv.h"
#include "../aoejson.h"

enum TC_MSG
{
	TC_LOAD = WM_APP,
	TC_SAVE,
	TC_CLOSE,
};

extern AoEJsonData GameStr;
extern AoE2ScenarioNamespace::AoE2Scenario Scen;

#define MAX_LOADSTRING 100
