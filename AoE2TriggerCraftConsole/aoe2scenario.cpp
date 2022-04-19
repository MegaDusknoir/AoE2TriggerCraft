
#include "aoe2scenario.h"

namespace AoE2ScenarioNamespace
{
    constexpr char current_version[] = "1.41";
    AoE2Scenario::AoE2Scenario(const char* file_path)
    {
        ifstream fin(file_path, ios::binary);
        if (fin.fail() == true)
        {
            throw;
        }
        string check_version(4, 0);
        fin.read(reinterpret_cast<char *>(&check_version[0]), 4);
        if (check_version == string(current_version))
        {
            fin.seekg(0, ios::end);
            auto file_size = static_cast<size_t>(fin.tellg());
            fin.seekg(ios::beg);
            string raw_binary(file_size, 0);
            fin.read(reinterpret_cast<char *>(&raw_binary[0]), file_size);
            fin.close();
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
            fin.close();
        }
        else
        {
            fin.close();
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
