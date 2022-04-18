#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "file_types.h"
#include "aoe2scenario_1_41.h"

#include "em_inflate\em_inflate.h"

namespace AoE2ScenarioNamespace
{
    using namespace AoE2ScenarioFileTypesNamespace;
    using namespace AoE2Scenario_1_41_Namespace;
    using AoE2ScenarioCurrent = AoE2Scenario_1_41;
    constexpr char current_version[] = "1.41";

    class AoE2Scenario
    {
    public:
        AoE2Scenario(const char* file_path)
        {
            ifstream fin(file_path, ios::binary);
            open_file(fin);
            fin.close();
        }
        AoE2Scenario(ifstream& fin)
        {
            open_file(fin);
        }
    private:
        void open_file(ifstream& fin);

    private:
        AoE2ScenarioCurrent scen;
        void Decompress(string& raw);
    };
}
