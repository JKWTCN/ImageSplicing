#ifndef ENUMERATE_H
#define ENUMERATE_H

#include <string>
#include <map>

//图片拼接移动模式
enum Move_Type
{
    MV_V,
    MV_H
};
// 图像保存格式
enum SaveType
{
    ST_PNG,
    ST_JPG
};

// 边缘填充颜色类型
enum PaddingColorType
{
    PT_TRANSPARENT,
    PT_BLACK,
    PT_WHITE,
    PT_DARKGRAY,
    PT_GRAY,
    PT_LIGHTGRAY,
    PT_RED,
    PT_GREEN,
    PT_BLUE,
    PT_CYAN,
    PT_MAGENTA,
    PT_YELLOW,
    PT_DARKRED,
    PT_DARKGREEN,
    PT_DARKBLUE,
    PT_DARKCYAN,
    PT_DARKMAGENTA,
    PT_DARKYELLOW,
};

// 图像拼接类型
enum SplicingType
{
    ST_LOW2HIGH,
    ST_HIGH2LOW,
    ST_RAW
};

// 插值方法类型
enum InterpolationType
{
    IT_INTER_LINEAR,
    IT_INTER_NEAREST,
    IT_INTER_AREA,
    IT_INTER_CUBIC,
    IT_INTER_LANCZOS4
};

// 特征提取算法
enum FeatureExtraction
{
    FE_SIFT,
    FE_SURF,
    FE_ORB
};

// 文件排序方式
enum FileSortType
{
    FST_IMPORT_ORDER,  // 导入顺序
    FST_ASCENDING,     // 顺序（从小到大）
    FST_DESCENDING     // 倒序（从大到小）
};

// 目前的拼接状态
enum SplicingState
{
    SS_VERTICAL,
    SS_HORIZONTAL,
    SS_AUTO_VERTICAL,
    SS_AUTO_HORIZONTAL,
    SS_NONE
};

// 枚举转换工具函数
namespace EnumConverter
{
    // SaveType 转换
    std::string saveTypeToString(SaveType type);
    SaveType stringToSaveType(const std::string &str);

    // SplicingType 转换
    std::string splicingTypeToString(SplicingType type);
    SplicingType stringToSplicingType(const std::string &str);

    // PaddingColorType 转换
    std::string paddingColorTypeToString(PaddingColorType type);
    PaddingColorType stringToPaddingColorType(const std::string &str);

    // InterpolationType 转换
    std::string interpolationTypeToString(InterpolationType type);
    InterpolationType stringToInterpolationType(const std::string &str);

    // FeatureExtraction 转换
    std::string featureExtractionToString(FeatureExtraction type);
    FeatureExtraction stringToFeatureExtraction(const std::string &str);
}

#endif // ENUMERATE_H
