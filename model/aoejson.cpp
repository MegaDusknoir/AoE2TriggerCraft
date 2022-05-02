#include <fstream>
#include <string>
#include <vector>
#include "aoejson.h"
#include "../util/gason/gason.h"
#include "../util/utf8conv/utf8conv.h"

using std::ifstream;
using std::string;
using std::wstring;
using std::vector;
using std::ios;

static const char* language_key[] = {
    "zh_cn",
    "en",
};

const char* excepted_key[] = {
    nullptr,
    "condition_name",
    "effect_name"
};

void AoEJsonRead::json_load(const char* path)
{
    ifstream fin(path, ios::binary);
    if (fin.fail())
    {
        throw std::runtime_error("Can not load json file");
    }
    fin.seekg(0, ios::end);
    auto file_size = static_cast<size_t>(fin.tellg());
    fin.seekg(ios::beg);
    string s(file_size, 0);
    fin.read(&s[0], file_size);
    fin.close();
    json_load(s);
}
void AoEJsonRead::json_load(const wchar_t* path)
{
    ifstream fin(path, ios::binary);
    if (fin.fail())
    {
        throw std::runtime_error("Can not load json file");
    }
    fin.seekg(0, ios::end);
    auto file_size = static_cast<size_t>(fin.tellg());
    fin.seekg(ios::beg);
    string s(file_size, 0);
    fin.read(&s[0], file_size);
    fin.close();
    json_load(s);
}
void AoEJsonRead::json_load(string& data)
{
    JsonAllocator json_alloc;
    JsonValue json_val;
    data.push_back('\0');
    char* endptr;

    int status = jsonParse(&data[0], &endptr, &json_val, json_alloc);
    if (status != JSON_OK)
    {
        throw;
    }
    json_read(json_val);
}
double AoEJsonRead::json_read(JsonValue o)
{
    double sum = 0;
    switch (o.getTag()) {
    case JSON_NUMBER:
        o.toNumber();
        sum += o.toNumber();
        break;
    case JSON_STRING:
        if (excepted_key_idx >= 0)
        {
            if (strcmp(excepted_key[excepted_key_idx], "condition_name") == 0)
            {
                paser_data.condition_name.push_back(win32::Utf8ToUtf16(string(o.toString())));
            }
            else if (strcmp(excepted_key[excepted_key_idx], "effect_name") == 0)
            {
                paser_data.effect_name.push_back(win32::Utf8ToUtf16(string(o.toString())));
            }
        }
        break;
    case JSON_ARRAY:
        for (auto i : o) {
            sum += json_read(i->value);
        }
        break;
    case JSON_OBJECT:
        for (auto i : o)
        {
            if ((excepted_key_idx + 1) < sizeof(excepted_key) / sizeof(char**) && strcmp(excepted_key[excepted_key_idx + 1], i->key) == 0)
            {
                ++excepted_key_idx;
            }
            sum += json_read(i->value);
        }
        break;
    case JSON_TRUE:
        break;
    case JSON_FALSE:
        break;
    case JSON_NULL:
        break;
    }
    return sum;
}
void AoEJsonRead::set_language(size_t lang_idx)
{
    excepted_key[0] = language_key[lang_idx];
}