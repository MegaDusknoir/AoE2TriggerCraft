
#include "aoe2scenario.h"

namespace AoE2ScenarioNamespace
{
    void AoE2Scenario::open_file(ifstream& fin)
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
    void AoE2Scenario::Decompress(string& raw)
    {
        uint8_t* out = new uint8_t[raw.size() * 128];
        auto out_size = em_inflate(raw.data(), raw.size(), out, _UI32_MAX);
        string decompressed;
        decompressed.append((char*)out, out_size);
        delete[] out;
        raw = std::move(decompressed);
    }
}
