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
        void open_file(ifstream& fin)
        {
            stringstream ssin;
            ssin << fin.rdbuf();
            string raw_binary(ssin.str());
            auto check_version = raw_binary.substr(0, 4);
			if (check_version == string(current_version))
			{
#ifndef NDEBUG
                auto t1 = clock();
#endif
				auto header_length = scen.Header.read(raw_binary.data());
				string MapData = raw_binary.substr(header_length);
				Decompress(MapData);
#ifndef NDEBUG
				auto raw_else(MapData.substr(scen.Data.read(MapData.data())));
				auto t2 = clock();
				std::cout << "Read completed in " << (double)(t2 - t1) / CLOCKS_PER_SEC << "ms" << std::endl;
#endif
            }
            else if (check_version == string("1.41"))
            {
            }
            else
            {
#ifndef NDEBUG
                std::cout << "Version " << check_version << " not supported yet." << std::endl;
#endif
            }
        }

    private:
        AoE2ScenarioCurrent scen;
        void Decompress(string& raw)
        {
            uint8_t* out = new uint8_t[raw.size() * 128];
            auto out_size = em_inflate(raw.data(), raw.size(), out, _UI32_MAX);
            string decompressed;
            decompressed.append((char*)out, out_size);
            delete[] out;
            raw = std::move(decompressed);
        }

    };
}
