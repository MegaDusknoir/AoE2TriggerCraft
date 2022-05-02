#pragma once
#include <string>
#include <vector>
#include "../util/gason/gason.h"
#include "../util/utf8conv/utf8conv.h"

struct AoEJsonData
{
    std::vector<std::wstring> condition_name;
    std::vector<std::wstring> effect_name;
};

class AoEJsonRead
{
public:
    AoEJsonRead(AoEJsonData& _paser_data, size_t lang_idx = 0)
        :excepted_key_idx(-1), paser_data(_paser_data)
    {
        set_language(lang_idx);
    }
    void json_load(const char* path);
    void json_load(const wchar_t* path);
    void json_load(std::string& data);
private:
    int excepted_key_idx;
    AoEJsonData& paser_data;
    double json_read(JsonValue o);
    void set_language(size_t lang_idx);
};
