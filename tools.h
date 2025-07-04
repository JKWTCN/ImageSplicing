#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <QString>
#include <setting.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
using json = nlohmann::json;

using namespace std;

class tools
{
public:
    tools();
    static string GetFileName(std::string path);
    static string Qstring2String(QString qstr);
    static Setting GetSetting();
    static void SetSetting(Setting setting);
    static string ReadFileIntoString(const string& path);
    static void WriteStringIntoFile(const string& text,const string& path);
};

#endif // TOOLS_H
