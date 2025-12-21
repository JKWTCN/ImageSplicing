#include <string>
#include <QString>
#include "enumerate.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <QMessageBox>
#include <memory>
#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QRegularExpression>
#include <algorithm>

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

static QStringList OpenImagePaths()
{
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
        nullptr,    // 父窗口
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

// 辅助函数：从文件名中提取所有数字
static QVector<int> ExtractNumbers(const QString &str)
{
    QVector<int> numbers;
    QRegularExpression re("(\\d+)");
    QRegularExpressionMatchIterator i = re.globalMatch(str);

    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        numbers.append(match.captured(1).toInt());
    }

    return numbers;
}

// 辅助函数：从文件名中提取最后一个数字（主要排序依据）
static int ExtractLastNumber(const QString &str)
{
    QVector<int> numbers = ExtractNumbers(str);
    return numbers.isEmpty() ? 0 : numbers.last();
}

// 文件排序函数
static QStringList SortFilePaths(const QStringList &filePaths, FileSortType sortType)
{
    if (sortType == FST_IMPORT_ORDER || filePaths.isEmpty())
    {
        return filePaths; // 导入顺序，不做排序
    }

    QStringList sortedPaths = filePaths;

    if (sortType == FST_ASCENDING)
    {
        // 顺序排序：从小到大 (59, 60, 61)
        std::sort(sortedPaths.begin(), sortedPaths.end(), [](const QString &a, const QString &b) {
            QFileInfo fileA(a);
            QFileInfo fileB(b);
            QString baseNameA = fileA.baseName(); // 不包含扩展名的文件名
            QString baseNameB = fileB.baseName();

            // 提取所有数字
            QVector<int> numbersA = ExtractNumbers(baseNameA);
            QVector<int> numbersB = ExtractNumbers(baseNameB);

            // 如果都有数字，逐个比较
            if (!numbersA.isEmpty() && !numbersB.isEmpty())
            {
                int compareCount = qMin(numbersA.size(), numbersB.size());
                for (int i = 0; i < compareCount; ++i)
                {
                    if (numbersA[i] != numbersB[i])
                    {
                        // 顺序：小的数字排前面
                        return numbersA[i] > numbersB[i];
                    }
                }
                // 如果前面的数字都相同，数字数量少的优先
                return numbersA.size() < numbersB.size();
            }
            else
            {
                // 如果没有找到数字或只有一个有数字，则按字符串比较
                return a.compare(b, Qt::CaseInsensitive) < 0;
            }
        });
    }
    else if (sortType == FST_DESCENDING)
    {
        // 倒序排序：从大到小 (61, 60, 59)
        std::sort(sortedPaths.begin(), sortedPaths.end(), [](const QString &a, const QString &b) {
            QFileInfo fileA(a);
            QFileInfo fileB(b);
            QString baseNameA = fileA.baseName();
            QString baseNameB = fileB.baseName();

            // 提取所有数字
            QVector<int> numbersA = ExtractNumbers(baseNameA);
            QVector<int> numbersB = ExtractNumbers(baseNameB);

            // 如果都有数字，逐个比较
            if (!numbersA.isEmpty() && !numbersB.isEmpty())
            {
                int compareCount = qMin(numbersA.size(), numbersB.size());
                for (int i = 0; i < compareCount; ++i)
                {
                    if (numbersA[i] != numbersB[i])
                    {
                        // 倒序：大的数字排前面
                        return numbersA[i] < numbersB[i];
                    }
                }
                // 如果前面的数字都相同，数字数量多的优先（倒序）
                return numbersA.size() > numbersB.size();
            }
            else
            {
                // 如果没有找到数字或只有一个有数字，则按字符串比较
                return a.compare(b, Qt::CaseInsensitive) > 0;
            }
        });
    }

    return sortedPaths;
}
