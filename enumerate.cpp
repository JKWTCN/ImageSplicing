#include "enumerate.h"
#include <stdexcept>

using namespace std;

namespace EnumConverter
{
    // SaveType 转换实现
    string saveTypeToString(SaveType type)
    {
        static const map<SaveType, string> names = {
            {ST_PNG, "ST_PNG"},
            {ST_JPG, "ST_JPG"}};
        auto it = names.find(type);
        return it != names.end() ? it->second : "UNKNOWN";
    }

    SaveType stringToSaveType(const string &str)
    {
        static const map<string, SaveType> types = {
            {"ST_PNG", ST_PNG},
            {"ST_JPG", ST_JPG}};
        auto it = types.find(str);
        if (it != types.end())
        {
            return it->second;
        }
        throw invalid_argument("Invalid SaveType string: " + str);
    }

    // SplicingType 转换实现
    string splicingTypeToString(SplicingType type)
    {
        static const map<SplicingType, string> names = {
            {ST_LOW2HIGH, "ST_LOW2HIGH"},
            {ST_HIGH2LOW, "ST_HIGH2LOW"},
            {ST_RAW, "ST_RAW"}};
        auto it = names.find(type);
        return it != names.end() ? it->second : "UNKNOWN";
    }

    SplicingType stringToSplicingType(const string &str)
    {
        static const map<string, SplicingType> types = {
            {"ST_LOW2HIGH", ST_LOW2HIGH},
            {"ST_HIGH2LOW", ST_HIGH2LOW},
            {"ST_RAW", ST_RAW}};
        auto it = types.find(str);
        if (it != types.end())
        {
            return it->second;
        }
        throw invalid_argument("Invalid SplicingType string: " + str);
    }

    // PaddingColorType 转换实现
    string paddingColorTypeToString(PaddingColorType type)
    {
        static const map<PaddingColorType, string> names = {
            {PT_TRANSPARENT, "PT_TRANSPARENT"},
            {PT_BLACK, "PT_BLACK"},
            {PT_WHITE, "PT_WHITE"}};
        auto it = names.find(type);
        return it != names.end() ? it->second : "UNKNOWN";
    }

    PaddingColorType stringToPaddingType(const string &str)
    {
        static const map<string, PaddingColorType> types = {
            {"PT_TRANSPARENT", PT_TRANSPARENT},
            {"PT_BLACK", PT_BLACK},
            {"PT_WHITE", PT_WHITE}};
        auto it = types.find(str);
        if (it != types.end())
        {
            return it->second;
        }
        throw invalid_argument("Invalid PaddingType string: " + str);
    }

    // InterpolationType 转换实现
    string interpolationTypeToString(InterpolationType type)
    {
        static const map<InterpolationType, string> names = {
            {IT_INTER_NEAREST, "IT_INTER_NEAREST"},
            {IT_INTER_LINEAR, "IT_INTER_LINEAR"},
            {IT_INTER_AREA, "IT_INTER_AREA"},
            {IT_INTER_CUBIC, "IT_INTER_CUBIC"},
            {IT_INTER_LANCZOS4, "IT_INTER_LANCZOS4"}};
        auto it = names.find(type);
        return it != names.end() ? it->second : "UNKNOWN";
    }

    InterpolationType stringToInterpolationType(const string &str)
    {
        static const map<string, InterpolationType> types = {
            {"IT_INTER_NEAREST", IT_INTER_NEAREST},
            {"IT_INTER_LINEAR", IT_INTER_LINEAR},
            {"IT_INTER_AREA", IT_INTER_AREA},
            {"IT_INTER_CUBIC", IT_INTER_CUBIC},
            {"IT_INTER_LANCZOS4", IT_INTER_LANCZOS4}};
        auto it = types.find(str);
        if (it != types.end())
        {
            return it->second;
        }
        throw invalid_argument("Invalid InterpolationType string: " + str);
    }

    // FeatureExtraction 转换实现
    string featureExtractionToString(FeatureExtraction type)
    {
        static const map<FeatureExtraction, string> names = {
            {FE_SIFT, "FE_SIFT"},
            {FE_SURF, "FE_SURF"},
            {FE_ORB, "FE_ORB"}};
        auto it = names.find(type);
        return it != names.end() ? it->second : "UNKNOWN";
    }

    FeatureExtraction stringToFeatureExtraction(const string &str)
    {
        static const map<string, FeatureExtraction> types = {
            {"FE_SIFT", FE_SIFT},
            {"FE_SURF", FE_SURF},
            {"FE_ORB", FE_ORB}};
        auto it = types.find(str);
        if (it != types.end())
        {
            return it->second;
        }
        throw invalid_argument("Invalid FeatureExtraction string: " + str);
    }
}
