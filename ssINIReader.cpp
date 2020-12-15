//@ Project : ssTools
//@ Copyright : Non-unruly (Git)
//@ Git: https://github.com/Non-Unruly/ssTools
//@
//@ Launch Date : 2020-06-27
//@ Update: 2020-12-15

#include "ssINIReader.h"

ssINIReader *ssINIReader::inst = NULL;

ssINIReader *ssINIReader::instance()
{
    if (inst == NULL)
        inst = new ssINIReader();
    return inst;
}

ssINIReader::ssINIReader()
{
}

ssINIReader::~ssINIReader()
{
}

std::string ssINIReader::trim(std::string str)
{
    while (str.front() == ' ')
        str = str.substr(1, str.size() - 1);
    while (str.back() == ' ')
        str.pop_back();
    return str;
}

bool ssINIReader::Load(const char *_iniFilePath)
{
    if (_iniFilePath == NULL || strlen(_iniFilePath) <= 0)
    {
        errmsg = "ini file path is empty";
        return false;
    }

    ini.clear();

    FILE *f = fopen(_iniFilePath, "r");
    if (f == NULL)
    {
        errmsg = "file open error";
        return false;
    }

    char line[1024];
    memset(line, 0, 1024);
    std::string root = "NULL";
    while (fgets(line, 1024, f))
    {
        std::string data(line);
        memset(line, 0, 1024);

        int pos = 0;
        while (true)
        {
            if (data[pos] == '\n' || data[pos] == '\r')
                data = data.erase(pos);
            else
                pos++;
            if (pos >= data.size())
                break;
        }

        if (data.empty())
            continue;
        if (data[0] == ';' || data[0] == '#')
            continue;

        //判断是否为新的[root]结点
        if (data.front() == '[' && data.back() == ']')
        {
            std::string str = data.substr(1, data.size() - 2);
            if (str == root)
                continue;
            else
                root = str;
            continue;
        }

        int spacePos = data.find('=');
        std::string tKey = data.substr(0, spacePos);
        std::string tValue = data.substr(spacePos + 1, data.size());
        std::string key = trim(tKey);
        std::string value = trim(tValue);

        ini[root][key] = value;
    }
    return true;
}

std::string ssINIReader::GetStrValue(std::string _root, std::string _key)
{
    std::string res = "";
    res = ini[_root][_key];
    return res;
}

int ssINIReader::GetIntValue(std::string _root, std::string _key)
{
    std::string res = "";
    res = ini[_root][_key];
    return atoi(res.c_str());
}

float ssINIReader::GetFltValue(std::string _root, std::string _key)
{
    std::string res = "";
    res = ini[_root][_key];
    return atof(res.c_str());
}

bool ssINIReader::GetBoolValue(std::string _root, std::string _key)
{
    std::string res = "";
    res = ini[_root][_key];
    return (res == "true" ? true : false);
}
