#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "util/file_types.h"
#include "aoe2scenario_1_41.h"
#include "aoe2scenario_1_45.h"
#include "util/utilio.h"

namespace AoE2ScenarioNamespace
{
    using namespace AoE2ScenarioFileTypesNamespace;
    using namespace AoE2Scenario_1_41_Namespace;
    using namespace AoE2Scenario_1_45_Namespace;
    using AoE2ScenarioCurrent = AoE2Scenario_1_45;

    class AoE2Scenario
	{
	public:
        AoE2Scenario();
		AoE2Scenario(const char* file_path);
        ~AoE2Scenario();
        void open(const char* file_path);
        void save(const char* file_path);
    private:
        static constexpr char current_version[] = "1.45";
        static DeflateClass deflate;
        string file_path;
        AoE2ScenarioCurrent scen;
        void deflate_decompress(string& raw);
        void deflate_compress(string& raw);
    };
}
