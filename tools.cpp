#include "tools.h"

tools::tools()
{

}

string tools::GetFileName(string path)
{
    //取得最后一个路径分隔符的位置
    size_t position = path.find_last_of("/\\");
    //截取最后一个路径分隔符后面的字符串即为文件名
    string filename = path.substr(position+1, path.length()-position-1);
    return filename;
}

string tools::Qstring2String(QString qstr){
    return qstr.toStdString();
}
