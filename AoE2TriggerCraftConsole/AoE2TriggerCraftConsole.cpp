
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../aoe2scenario.h"

using namespace std;
using namespace AoE2ScenarioNamespace;

void test_fun();

int main()
{
    test_fun();
    return 0;
}

AoE2Scenario scen;
const char scn_path[] = "D:/Age of Empires/AoE2TriggerCraft/AoE2TriggerCraftConsole/minato007.aoe2scenario";
const char save_path[] = "D:/Age of Empires/AoE2TriggerCraft/AoE2TriggerCraftConsole/saveto_minato007.aoe2scenario";

void test_fun()
{
    scen.open(scn_path);
    //scen.triggers->sort_by_order();
    AoE2ScenarioCurrent::FileBody::TriggersStruct::TriggerStruct new_trig;
#ifndef NSPEEDTRACE
    auto t1 = clock();
#endif
    scen.triggers->add(new_trig);
    scen.triggers->add(new_trig);
    scen.triggers->add(new_trig);
    scen.triggers->add(new_trig);
    scen.triggers->add(new_trig);
    scen.triggers->mov(12, scen.triggers->size() - 5, scen.triggers->size());
    scen.triggers->del(12, 5);
    scen.triggers->copy_to_all(3, CP_MOD_SOURCE | CP_MOD_COLOR);
    //scen.add_trigger();
    //scen.delete_trigger(112, 78);
    //scen.add_trigger(5);
    //scen.delete_trigger(9,12);
    //scen.add_trigger(9);
    //scen.undo();
    //scen.undo();
    //scen.undo();
    //scen.undo();
    //scen.undo();
    scen.triggers->del(3 + 1, 7);
    scen.triggers->confirm();
#ifndef NSPEEDTRACE
    auto t2 = clock();
    std::cout << "Operate completed in " << (double)(t2 - t1) / CLOCKS_PER_SEC << "sec" << std::endl;
#endif
    scen.save(save_path);
    AoE2Scenario* scen3 = new AoE2Scenario(save_path);
    delete scen3;
}