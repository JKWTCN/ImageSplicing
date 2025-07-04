#include "settingwindow.h"
#include "ui_settingwindow.h"

SettingWindow::SettingWindow(QWidget *parent) : QDialog(parent),
                                                ui(new Ui::SettingWindow)
{
    ui->setupUi(this);
    Setting setting = tools::GetSetting();
    ui->checkBox_openReverse->setChecked(setting.getopenReverse());
    ui->checkBox_finishRAWPhicture->setChecked(setting.getfinishRAWPhicture());
    ui->comboBox_saveType->setCurrentIndex(static_cast<int>(setting.getSaveType()));
    ui->comboBox_splicingType->setCurrentIndex(static_cast<int>(setting.getSplicingType()));
    ui->comboBox_paddingType->setCurrentIndex(static_cast<int>(setting.getPaddingType()));
    ui->comboBox_narrowType->setCurrentIndex(static_cast<int>(setting.getNarrowType()));
    ui->comboBox_shrinkType->setCurrentIndex(static_cast<int>(setting.getShrinkType()));
    ui->comboBox_featureExtraction->setCurrentIndex(static_cast<int>(setting.getFeatureExtraction()));
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]()
            {
        Setting setting;
        setting.setopenReverse(ui->checkBox_openReverse->isChecked());
        setting.setfinishRAWPhicture(ui->checkBox_finishRAWPhicture->isChecked());
        setting.setSaveType(static_cast<SaveType>(ui->comboBox_saveType->currentIndex()));
        setting.setSplicingType(static_cast<SplicingType>(ui->comboBox_splicingType->currentIndex()));
        setting.setPaddingType(static_cast<PaddingType>(ui->comboBox_paddingType->currentIndex()));
        setting.setNarrowType(static_cast<InterpolationType>(ui->comboBox_narrowType->currentIndex()));
        setting.setShrinkType(static_cast<InterpolationType>(ui->comboBox_shrinkType->currentIndex()));
        setting.setFeatureExtraction(static_cast<FeatureExtraction>(ui->comboBox_featureExtraction->currentIndex()));
        tools::SetSetting(setting);
        this->close(); });
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SettingWindow::close);
}
SettingWindow::~SettingWindow()
{
    delete ui;
}
