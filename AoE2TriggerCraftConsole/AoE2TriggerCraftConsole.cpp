﻿
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../aoe2scenario.h"

using namespace std;
using namespace AoE2ScenarioNamespace;
AoE2Scenario scen;

const char scn_path[] = "D:/Age of Empires/AoE2TriggerCraft/AoE2TriggerCraftConsole/minato007.aoe2scenario";
const char save_path[] = "D:/Age of Empires/AoE2TriggerCraft/AoE2TriggerCraftConsole/saveto_minato007.aoe2scenario";
int main()
{
	scen.open(scn_path);
    scen.save(save_path);
    AoE2Scenario* scen3 = new AoE2Scenario(save_path);
    delete scen3;
    return 0;
}
