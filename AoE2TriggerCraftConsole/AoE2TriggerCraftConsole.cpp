
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "aoe2scenario.h"

using namespace std;
using namespace AoE2ScenarioNamespace;

const char scn_path[] = "D:/Age of Empires/AoE2TriggerCraft/AoE2TriggerCraftConsole/minato007.aoe2scenario";
const char scn_path2[] = "D:/Age of Empires/AoE2TriggerCraft/AoE2TriggerCraftConsole/out1_minato007.aoe2scenario";
int main()
{
	AoE2Scenario* scen = new AoE2Scenario(scn_path);
	AoE2Scenario* scen2 = new AoE2Scenario(scn_path2);
    std::cout << "scen1 " << (scen->raw_data == scen2->raw_data ? "==" : "!=") << " scen2." << endl;
    delete scen;
    delete scen2;
    return 0;
}
