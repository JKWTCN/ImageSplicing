#ifndef SETTING_H
#define SETTING_H

#include "enumerate.h"
#include <nlohmann/json.hpp>
#include <string>
#include <stdexcept>

class Setting
{
public:
    Setting();

    // Get
    bool getopenReverse() const;
    bool getfinishRAWPhicture() const;
    SaveType getSaveType() const;
    SplicingType getSplicingType() const;
    PaddingType getPaddingType() const;
    InterpolationType getNarrowType() const;
    InterpolationType getShrinkType() const;
    FeatureExtraction getFeatureExtraction() const;

    // Set
    void setopenReverse(bool value);
    void setfinishRAWPhicture(bool value);
    void setSaveType(SaveType value);
    void setSplicingType(SplicingType value);
    void setPaddingType(PaddingType value);
    void setNarrowType(InterpolationType value);
    void setShrinkType(InterpolationType value);
    void setFeatureExtraction(FeatureExtraction value);

    // JSON 序列化
    std::string toJson() const;
    void fromJson(const std::string &jsonStr);

private:
    bool openReverse;
    bool finishRAWPhicture;
    SaveType saveType;
    SplicingType splicingType;
    PaddingType paddingType;
    InterpolationType narrowType;
    InterpolationType shrinkType;
    FeatureExtraction featureExtraction;
};

#endif // SETTING_H
