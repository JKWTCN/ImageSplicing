#include "enumerate.h"
#include <QSettings>

static bool GetFinishRAWPhictureConfig()
{
    QSettings settings("iCloudWar", "ImageSplicing");
    bool finishRAWPhicture = settings.value("FinishRAWPhicture", false).toBool();
    return finishRAWPhicture;
}
static void SetFinishRAWPhictureConfig(bool finishRAWPhicture)
{
    QSettings settings("iCloudWar", "ImageSplicing");
    settings.setValue("FinishRAWPhicture", finishRAWPhicture);
}

static bool GetOpenReverseConfig()
{
    QSettings settings("iCloudWar", "ImageSplicing");
    bool openReverse = settings.value("OpenReverse", false).toBool();
    return openReverse;
}
static void SetOpenReverseConfig(bool openReverse)
{
    QSettings settings("iCloudWar", "ImageSplicing");
    settings.setValue("OpenReverse", openReverse);
}

static SaveType GetSaveTypeConfig()
{
    QSettings settings("iCloudWar", "ImageSplicing");
    SaveType saveType = SaveType(settings.value("SaveType", ST_PNG).toInt());
    return saveType;
}

static void SetSaveTypeConfig(SaveType saveType)
{
    QSettings settings("iCloudWar", "ImageSplicing");
    settings.setValue("SaveType", saveType);
}

static PaddingColorType GetPaddingColorTypeConfig()
{
    QSettings settings("iCloudWar", "ImageSplicing");
    PaddingColorType paddingColorType = PaddingColorType(settings.value("PaddingColorType", PT_TRANSPARENT).toInt());
    return paddingColorType;
}

static void SetPaddingColorTypeConfig(PaddingColorType paddingColorType)
{
    QSettings settings("iCloudWar", "ImageSplicing");
    settings.setValue("PaddingColorType", paddingColorType);
}
static SplicingType GetSplicingTypeConfig()
{
    QSettings settings("iCloudWar", "ImageSplicing");
    SplicingType splicingType = SplicingType(settings.value("SplicingType", ST_LOW2HIGH).toInt());
    return splicingType;
}

static void SetSplicingTypeConfig(SplicingType splicingType)
{
    QSettings settings("iCloudWar", "ImageSplicing");
    settings.setValue("SplicingType", splicingType);
}

static InterpolationType GetInterpolationTypeConfig()
{
    QSettings settings("iCloudWar", "ImageSplicing");
    InterpolationType interpolationType = InterpolationType(settings.value("InterpolationType", IT_INTER_LINEAR).toInt());
    return interpolationType;
}

static void SetInterpolationTypeConfig(InterpolationType interpolationType)
{
    QSettings settings("iCloudWar", "ImageSplicing");
    settings.setValue("InterpolationType", interpolationType);
}
static FeatureExtraction GetFeatureExtractionConfig()
{
    QSettings settings("iCloudWar", "ImageSplicing");
    FeatureExtraction featureExtraction = FeatureExtraction(settings.value("FeatureExtraction", FE_ORB).toInt());
    return featureExtraction;
}

static void SetFeatureExtractionConfig(FeatureExtraction featureExtraction)
{
    QSettings settings("iCloudWar", "ImageSplicing");
    settings.setValue("FeatureExtraction", featureExtraction);
}

static QString GetLastOpenDirectoryConfig()
{
    QSettings settings("iCloudWar", "ImageSplicing");
    QString lastDir = settings.value("LastOpenDirectory", QDir::homePath()).toString();
    return lastDir;
}

static void SetLastOpenDirectoryConfig(const QString &lastDir)
{
    QSettings settings("iCloudWar", "ImageSplicing");
    settings.setValue("LastOpenDirectory", lastDir);
}

static InterpolationType GetShrinkTypeConfig()
{
    QSettings settings("iCloudWar", "ImageSplicing");
    InterpolationType shrinkType = InterpolationType(settings.value("ShrinkType", ST_LOW2HIGH).toInt());
    return shrinkType;
}

static void SetShrinkTypeConfig(InterpolationType shrinkType)
{
    QSettings settings("iCloudWar", "ImageSplicing");
    settings.setValue("ShrinkType", shrinkType);
}

static InterpolationType GetNarrowTypeConfig()
{
    QSettings settings("iCloudWar", "ImageSplicing");
    InterpolationType getNarrowType = InterpolationType(settings.value("GetNarrowType", ST_LOW2HIGH).toInt());
    return getNarrowType;
}
static void SetNarrowTypeConfig(InterpolationType getNarrowType)
{
    QSettings settings("iCloudWar", "ImageSplicing");
    settings.setValue("NarrowType", getNarrowType);
}
