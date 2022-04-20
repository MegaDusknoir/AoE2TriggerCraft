
#include "aoe2scenario.h"

namespace AoE2ScenarioNamespace
{
    DeflateClass AoE2Scenario::deflate;
    AoE2Scenario::AoE2Scenario()
    {
    }
    AoE2Scenario::~AoE2Scenario()
    {
    }
    AoE2Scenario::AoE2Scenario(const char* file_path)
    {
        open(file_path);
    }
    void AoE2Scenario::open(const char* file_path)
    {
        AutoFile fin(file_path, ios::in | ios::binary);
        string check_version(4, 0);
        fin->read(reinterpret_cast<char*>(&check_version[0]), 4);
        if (check_version == string(current_version))
        {
#ifndef NDEBUG
            auto t1 = clock();
#endif
            fin->seekg(0, ios::end);
            auto file_size = static_cast<size_t>(fin->tellg());
            fin->seekg(ios::beg);
            string raw_binary(file_size, 0);
            fin->read(reinterpret_cast<char*>(&raw_binary[0]), file_size);
            fin->close();
            auto header_length = scen.header.read(raw_binary.data());
            string scen_body = raw_binary.substr(header_length);
            deflate_decompress(scen_body);
            auto raw_else(scen_body.substr(scen.body.read(scen_body.data())));
#ifndef NDEBUG
            auto t2 = clock();
            std::cout << "Read completed in " << (double)(t2 - t1) / CLOCKS_PER_SEC << "ms" << std::endl;
#endif
        }
        else if (check_version == string("1.41"))
        {
            //Transform 1.41 to newest format.
        }
        else
        {
#ifndef NDEBUG
            std::cout << "Version " << check_version << " not supported yet." << std::endl;
#endif
        }
    }

    void AoE2Scenario::save(const char* file_path)
    {
#ifndef NDEBUG
        auto t1 = clock();
#endif
        string shead(1 * 1024 * 1024, '\0');
        string sbody(16 * 1024 * 1024, '\0');
        shead.resize(scen.header.write(&shead[0]));
        sbody.resize(scen.body.write(&sbody[0]));
        scen.body.read(sbody.data());
        deflate_compress(sbody);
		AutoFile fout(file_path, ios::out | ios::binary);
        fout->write(shead.data(), shead.size());
        fout->write(sbody.data(), sbody.size());
        fout->close();
#ifndef NDEBUG
		auto t2 = clock();
		std::cout << "Write completed in " << (double)(t2 - t1) / CLOCKS_PER_SEC << "ms" << std::endl;
#endif
    }

    void AoE2Scenario::deflate_decompress(string& raw)
    {
        string out(raw.size() * 128, '\0');
        size_t out_size = deflate.decompress(raw.data(), raw.size(), static_cast<void *>(&out[0]), out.size());
        out.resize(out_size);
        raw = std::move(out);
    }
    void AoE2Scenario::deflate_compress(string& raw)
    {
        string out(raw.size() * 32, '\0');
        size_t out_size = deflate.compress(raw.data(), raw.size(), static_cast<void*>(&out[0]), out.size());
        out.resize(out_size);
        raw = std::move(out);
    }
}
