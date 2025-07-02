#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <QString>
using namespace std;

class tools
{
public:
    tools();
    static string GetFileName(std::string path);
    static string Qstring2String(QString qstr);
};

#endif // TOOLS_H
