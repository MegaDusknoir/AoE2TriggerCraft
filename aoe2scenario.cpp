
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
    AoE2Scenario::AoE2Scenario(const wchar_t* file_path)
    {
        open(file_path);
    }
    void AoE2Scenario::open(const char* file_path)
    {
        AutoFile fin(file_path, ios::in | ios::binary);
        open(fin);
    }
    void AoE2Scenario::open(const wchar_t* file_path)
    {
        AutoFile fin(file_path, ios::in | ios::binary);
        open(fin);
    }
	void AoE2Scenario::open(AutoFile& fin)
    {
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
            string err_what;
            err_what = err_what + "Version " + check_version + " not supported yet.";
            throw std::runtime_error(err_what);
#ifndef NDEBUG
            std::cout << "Version " << check_version << " not supported yet." << std::endl;
#endif
        }
    }

    void AoE2Scenario::save(const char* file_path)
    {
        AutoFile fout(file_path, ios::out | ios::binary);
        save(fout);
    }
    void AoE2Scenario::save(const wchar_t* file_path)
    {
        AutoFile fout(file_path, ios::out | ios::binary);
        save(fout);
    }
    void AoE2Scenario::save(AutoFile& fout)
    {
#ifndef NDEBUG
        auto t1 = clock();
#endif
        string shead(1 * 1024 * 1024, '\0');
        string sbody(16 * 1024 * 1024, '\0');
        shead.resize(scen.header.write(&shead[0]));
        sbody.resize(scen.body.write(&sbody[0]));
        deflate_compress(sbody);
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
    void AoE2Scenario::undo(void)
    {
        if (operator_backward.empty() == false)
        {
            operator_backward.back()->undo();
            operator_forward.push_back(std::move(operator_backward.back()));
            operator_backward.pop_back();
        }
    }
    void AoE2Scenario::redo(void)
    {
        if (operator_forward.empty() == false)
        {
            operator_forward.back()->redo();
            operator_backward.push_back(std::move(operator_forward.back()));
            operator_forward.pop_back();
        }
    }
    iterTrigger AoE2Scenario::add_trigger(int32_t location)
    {
        operator_backward.push_back(unique_ptr<BaseOperator>(new OperatorAddTrigger(*this, location)));
        operator_forward.clear();
        return dynamic_cast<OperatorAddTrigger*>(operator_backward.back().get())->get_return();
    }
    iterTrigger AoE2Scenario::delete_trigger(int32_t front, int32_t back)
    {
        operator_backward.push_back(unique_ptr<BaseOperator>(new OperatorDelTrigger(*this, front, back)));
        operator_forward.clear();
        return dynamic_cast<OperatorDelTrigger*>(operator_backward.back().get())->get_return();
    }
    void AoE2Scenario::sort_trigger()
    {
        operator_backward.push_back(unique_ptr<BaseOperator>(new OperatorSortTrigger(*this)));
        operator_forward.clear();
        //return dynamic_cast<OperatorSortTrigger*>(operator_backward.back().get())->get_return();
    }
}
