
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "aoe2scenario.h"

using namespace std;
using namespace AoE2ScenarioNamespace;

const char scn_path[] = "C:/Users/Dusknior/Games/Age of Empires 2 DE/76561198259589062/resources/_common/scenario/## 金字塔特种兵决定版原件 ##.aoe2scenario";
int main()
{
    AoE2Scenario *scen = new AoE2Scenario(scn_path);
    std::cout << endl;
    delete scen;
    return 0;
}
