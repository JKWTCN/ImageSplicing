#ifndef ENUMERATE_H
#define ENUMERATE_H

#include <string>
#include <map>

// 图像保存格式
enum SaveType
{
    ST_PNG,
    ST_JPG
};

// 边缘填充类型
enum PaddingType
{
    PT_TRANSPARENT,
    PT_BLACK,
    PT_WHITE
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

// 枚举转换工具函数
namespace EnumConverter
{
    // SaveType 转换
    std::string saveTypeToString(SaveType type);
    SaveType stringToSaveType(const std::string &str);

    // SplicingType 转换
    std::string splicingTypeToString(SplicingType type);
    SplicingType stringToSplicingType(const std::string &str);

    // PaddingType 转换
    std::string paddingTypeToString(PaddingType type);
    PaddingType stringToPaddingType(const std::string &str);

    // InterpolationType 转换
    std::string interpolationTypeToString(InterpolationType type);
    InterpolationType stringToInterpolationType(const std::string &str);

    // FeatureExtraction 转换
    std::string featureExtractionToString(FeatureExtraction type);
    FeatureExtraction stringToFeatureExtraction(const std::string &str);
}

#endif // ENUMERATE_H
