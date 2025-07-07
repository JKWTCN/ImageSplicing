#include <string>
#include <QString>
#include <setting.h>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <QMessageBox>
#include <memory>
#include <QSettings>
#include <QDir>
#include <QFileDialog>
using json = nlohmann::json;

using namespace std;

static string GetFileName(string path)
{
    // 取得最后一个路径分隔符的位置
    size_t position = path.find_last_of("/\\");
    // 截取最后一个路径分隔符后面的字符串即为文件名
    string filename = path.substr(position + 1, path.length() - position - 1);
    return filename;
}

static string Qstring2String(QString qstr)
{
    return qstr.toStdString();
}

static string ReadFileIntoString(const string &path)
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

static void WriteStringIntoFile(const string &text, const string &path)
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
static Setting GetSetting()
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

static void showInfoMessageBox(string title, string content)
{
    QMessageBox::information(NULL, QString::fromStdString(title), QString::fromStdString(content),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

static void showNoticeMessageBox(string title, string content)
{
    QMessageBox::information(NULL, QString::fromStdString(title), QString::fromStdString(content),
                             QMessageBox::Yes | QMessageBox::Yes);
}

static void showErrorMessageBox(string title, string content)
{
    QMessageBox::critical(NULL, QString::fromStdString(title), QString::fromStdString(content),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

static void showWarningMessageBox(string title, string content)
{
    QMessageBox::warning(NULL, QString::fromStdString(title), QString::fromStdString(content),
                         QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

static void showQuestionMessageBox(string title, string content)
{
    QMessageBox::question(NULL, QString::fromStdString(title), QString::fromStdString(content),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

static void SetSetting(Setting setting)
{
    WriteStringIntoFile(setting.toJson(), "setting.json");
}

static QStringList OpenImagePaths(){
    // 设置文件过滤器
    QString filter = "All Image Files (*.JPG *.jpeg *.PNG *.bmp *.gif *.tif *.tiff *.ico);;"
                     "JPEG Files (*.JPG *.jpeg);;"
                     "PNG Files (*.PNG);;"
                     "Bitmap Files (*.bmp);;"
                     "GIF Files (*.gif);;"
                     "TIFF Files (*.tif *.tiff);;"
                     "Icon Files (*.ico);;"
                     "All Files (*.*)";
    // 从设置中获取上次使用的目录
    QSettings settings("iCloudWar", "ImageSplicing");
    QString lastDir = settings.value("LastOpenDirectory", QDir::homePath()).toString();

    // 打开文件对话框，允许多选
    QStringList filePaths = QFileDialog::getOpenFileNames(
        nullptr,       // 父窗口
        "选择文件", // 对话框标题
        lastDir,    // 使用上次的目录作为初始目录
        filter      // 文件过滤器
    );

    // 如果选择了文件，保存当前目录
    if (!filePaths.isEmpty())
    {
        // 获取第一个文件的目录路径
        QString currentDir = QFileInfo(filePaths.first()).absolutePath();
        settings.setValue("LastOpenDirectory", currentDir);
    }

    return filePaths;
}
