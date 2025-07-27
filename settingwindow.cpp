#include "settingwindow.h"
#include "ui_settingwindow.h"
#include <tools.hpp>
#include <config.hpp>
SettingWindow::SettingWindow(QWidget *parent) : QDialog(parent),
                                                ui(new Ui::SettingWindow)
{
    ui->setupUi(this);
    ui->checkBox_openReverse->setChecked(GetOpenReverseConfig());
    ui->checkBox_finishRAWPhicture->setChecked(GetFinishRAWPhictureConfig());
    ui->comboBox_saveType->setCurrentIndex(static_cast<int>(GetSaveTypeConfig()));
    ui->comboBox_splicingType->setCurrentIndex(static_cast<int>(GetSplicingTypeConfig()));
    ui->comboBox_paddingType->setCurrentIndex(static_cast<int>(GetPaddingColorTypeConfig()));
    ui->comboBox_narrowType->setCurrentIndex(static_cast<int>(GetNarrowTypeConfig()));
    ui->comboBox_shrinkType->setCurrentIndex(static_cast<int>(GetShrinkTypeConfig()));
    ui->comboBox_featureExtraction->setCurrentIndex(static_cast<int>(GetFeatureExtractionConfig()));
    ui->spinBox_jpg_quality->setValue(GetJpgQualityConfig());
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]()
            {
        SetOpenReverseConfig(ui->checkBox_openReverse->isChecked());
        SetFinishRAWPhictureConfig(ui->checkBox_finishRAWPhicture->isChecked());
        SetSaveTypeConfig(static_cast<SaveType>(ui->comboBox_saveType->currentIndex()));
        SetSplicingTypeConfig(static_cast<SplicingType>(ui->comboBox_splicingType->currentIndex()));
        SetPaddingColorTypeConfig(static_cast<PaddingColorType>(ui->comboBox_paddingType->currentIndex()));
        SetNarrowTypeConfig(static_cast<InterpolationType>(ui->comboBox_narrowType->currentIndex()));
        SetShrinkTypeConfig(static_cast<InterpolationType>(ui->comboBox_shrinkType->currentIndex()));
        SetFeatureExtractionConfig(static_cast<FeatureExtraction>(ui->comboBox_featureExtraction->currentIndex()));
        SetJpgQualityConfig(int(ui->spinBox_jpg_quality->value()));

        this->close(); });
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SettingWindow::close);
    QListView *view = qobject_cast<QListView *>(ui->comboBox_splicingType->view());
    view->setRowHidden(ST_RAW, true);
    if (ui->comboBox_paddingType->currentIndex() != ST_RAW)
    {
        ui->comboBox_paddingType->setVisible(false);
        ui->label_paddingType->setVisible(false);
    }
    if (ui->comboBox_splicingType->currentIndex() == ST_RAW && ui->comboBox_saveType->currentIndex() != ST_PNG)
    {
        QListView *view = qobject_cast<QListView *>(ui->comboBox_paddingType->view());
        view->setRowHidden(PT_TRANSPARENT, true);
    }
}
SettingWindow::~SettingWindow()
{
    delete ui;
}

void SettingWindow::on_comboBox_splicingType_currentIndexChanged(int index)
{
    if (index != ST_RAW)
    {
        ui->comboBox_paddingType->setVisible(false);
        ui->label_paddingType->setVisible(false);
    }
    else
    {
        ui->comboBox_paddingType->setVisible(true);
        ui->label_paddingType->setVisible(true);
    }
    if (ui->comboBox_saveType->currentIndex() == ST_JPG)
    {
        ui->spinBox_jpg_quality->setVisible(true);
        ui->label_jpg_quality->setVisible(true);
    }
    else
    {
        ui->spinBox_jpg_quality->setVisible(false);
        ui->label_jpg_quality->setVisible(false);
    }
}

void SettingWindow::on_comboBox_saveType_currentIndexChanged(int index)
{
    if (ui->comboBox_splicingType->currentIndex() == ST_RAW && index == ST_PNG)
    {
        QListView *view = qobject_cast<QListView *>(ui->comboBox_paddingType->view());
        view->setRowHidden(PT_TRANSPARENT, false);
    }
    else
    {
        if (ui->comboBox_paddingType->currentIndex() == PT_TRANSPARENT)
            ui->comboBox_paddingType->setCurrentIndex(PT_BLACK);
        QListView *view = qobject_cast<QListView *>(ui->comboBox_paddingType->view());
        view->setRowHidden(PT_TRANSPARENT, true);
    }
    if (ui->comboBox_saveType->currentIndex() == ST_JPG)
    {
        ui->spinBox_jpg_quality->setVisible(true);
        ui->label_jpg_quality->setVisible(true);
    }
    else
    {
        ui->spinBox_jpg_quality->setVisible(false);
        ui->label_jpg_quality->setVisible(false);
    }
}

void SettingWindow::on_spinBox_jpg_quality_valueChanged(int arg1)
{
    QSettings settings("iCloudWar", "ImageSplicing");
    settings.setValue("JpgQuality", arg1);
}
