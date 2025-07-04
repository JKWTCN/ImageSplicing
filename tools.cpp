#include "tools.h"

tools::tools()
{
}

string tools::GetFileName(string path)
{
    // 取得最后一个路径分隔符的位置
    size_t position = path.find_last_of("/\\");
    // 截取最后一个路径分隔符后面的字符串即为文件名
    string filename = path.substr(position + 1, path.length() - position - 1);
    return filename;
}

string tools::Qstring2String(QString qstr)
{
    return qstr.toStdString();
}

string tools::ReadFileIntoString(const string &path)
{
    auto ss = ostringstream{};
    ifstream input_file(path);
    if (!input_file.is_open())
    {
        cerr << "Could not open the file - '" << path << "'" << endl;
        exit(EXIT_FAILURE);
    }
    ss << input_file.rdbuf();
    return ss.str();
}

void tools::WriteStringIntoFile(const string &text, const string &path)
{
    ofstream output_file(path);
    if (!output_file.is_open())
    {
        cerr << "Could not open the file for writing - '" << path << "'" << endl;
        exit(EXIT_FAILURE);
    }
    output_file << text;
    if (output_file.fail())
    {
        cerr << "Failed to write to file - '" << path << "'" << endl;
        exit(EXIT_FAILURE);
    }
}
Setting tools::GetSetting()
{
    Setting setting = Setting();
    ifstream settingJson("setting.json");
    if (settingJson.good())
    {
        setting.fromJson(ReadFileIntoString("setting.json"));
    }
    else
    {
        WriteStringIntoFile(setting.toJson(), "setting.json");
    }

    return setting;
}

void tools::SetSetting(Setting setting)
{
    WriteStringIntoFile(setting.toJson(), "setting.json");
}
