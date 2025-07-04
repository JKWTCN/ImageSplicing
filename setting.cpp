#include "setting.h"
#include "enumerate.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Setting::Setting() : openReverse(false),
                     finishRAWPhicture(false),
                     saveType(ST_PNG),
                     splicingType(ST_LOW2HIGH),
                     paddingType(PT_TRANSPARENT),
                     narrowType(IT_INTER_AREA),
                     shrinkType(IT_INTER_NEAREST),
                     featureExtraction(FE_SIFT)
{
}

// Getter
bool Setting::getopenReverse() const { return openReverse; }
bool Setting::getfinishRAWPhicture() const { return finishRAWPhicture; }
SaveType Setting::getSaveType() const { return saveType; }
SplicingType Setting::getSplicingType() const { return splicingType; }
PaddingType Setting::getPaddingType() const { return paddingType; }
InterpolationType Setting::getNarrowType() const { return narrowType; }
InterpolationType Setting::getShrinkType() const { return shrinkType; }
FeatureExtraction Setting::getFeatureExtraction() const { return featureExtraction; }

// Setter
void Setting::setopenReverse(bool value) { openReverse = value; }
void Setting::setfinishRAWPhicture(bool value) { finishRAWPhicture = value; }
void Setting::setSaveType(SaveType value) { saveType = value; }
void Setting::setSplicingType(SplicingType value) { splicingType = value; }
void Setting::setPaddingType(PaddingType value) { paddingType = value; }
void Setting::setNarrowType(InterpolationType value) { narrowType = value; }
void Setting::setShrinkType(InterpolationType value) { shrinkType = value; }
void Setting::setFeatureExtraction(FeatureExtraction value) { featureExtraction = value; }

//  JSON 序列化
std::string Setting::toJson() const
{
    json j;
    j["openReverse"] = openReverse;
    j["finishRAWPhicture"] = finishRAWPhicture;
    j["saveType"] = EnumConverter::saveTypeToString(saveType);
    j["splicingType"] = EnumConverter::splicingTypeToString(splicingType);
    j["paddingType"] = EnumConverter::paddingTypeToString(paddingType);
    j["narrowType"] = EnumConverter::interpolationTypeToString(narrowType);
    j["shrinkType"] = EnumConverter::interpolationTypeToString(shrinkType);
    j["featureExtraction"] = EnumConverter::featureExtractionToString(featureExtraction);

    return j.dump(4); // Pretty print with 4 spaces indentation
}

void Setting::fromJson(const std::string &jsonStr)
{
    try
    {
        json j = json::parse(jsonStr);

        openReverse = j.value("openReverse", false);
        finishRAWPhicture = j.value("finishRAWPhicture", false);

        saveType = EnumConverter::stringToSaveType(j.value("saveType", "ST_PNG"));
        splicingType = EnumConverter::stringToSplicingType(j.value("splicingType", "ST_LOW2HIGH"));
        paddingType = EnumConverter::stringToPaddingType(j.value("paddingType", "PT_TRANSPARENT"));
        narrowType = EnumConverter::stringToInterpolationType(j.value("narrowType", "IT_INTER_AREA"));
        shrinkType = EnumConverter::stringToInterpolationType(j.value("shrinkType", "IT_INTER_NEAREST"));
        featureExtraction = EnumConverter::stringToFeatureExtraction(j.value("featureExtraction", "FE_SIFT"));
    }
    catch (const json::parse_error &e)
    {
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }
    catch (const std::invalid_argument &e)
    {
        throw std::runtime_error("Invalid enum value: " + std::string(e.what()));
    }
}
