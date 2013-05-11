#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(FLMSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    mSettings = settings;
    RenderConfig *renderConfig = settings->getActiveConfig();

    ui->spinBoxDXYLaserOn->setValue(renderConfig->mDelays.mDXY_LaserOn);
    ui->spinBoxDXYLaserOff->setValue(renderConfig->mDelays.mDXY_LaserOff);
    ui->spinBoxLaserOnDelay->setValue(renderConfig->mDelays.mLaserOnDelay);
    ui->spinBoxLaserOffDelay->setValue(renderConfig->mDelays.mLaserOffDelay);
    ui->spinBoxPointDelay->setValue(renderConfig->mDelays.mPointDelay);
    ui->spinBoxQSWPeriod->setValue(renderConfig->mDelays.mQSWPeriod);
    ui->spinBoxQSWDuration->setValue(renderConfig->mDelays.mQSWDuration);
    ui->checkBoxDrawIdleMotion->setChecked(renderConfig->mDrawIdleMotion);

    switch (renderConfig->mRenderMode)
    {
    case FAST:
        ui->radioFastDraw->setChecked(true);
        ui->radioLaserEmulation->setChecked(false);
        ui->radioLines->setChecked(true);
        ui->radioSteps->setChecked(false);
        ui->groupLaserEmulationWay->setEnabled(false);
        break;
    case LINES:
        ui->radioFastDraw->setChecked(false);
        ui->radioLaserEmulation->setChecked(true);
        ui->radioLines->setChecked(true);
        ui->radioSteps->setChecked(false);
        ui->groupLaserEmulationWay->setEnabled(true);
        break;
    case STEPS:
        ui->radioFastDraw->setChecked(false);
        ui->radioLaserEmulation->setChecked(true);
        ui->radioLines->setChecked(false);
        ui->radioSteps->setChecked(true);
        ui->groupLaserEmulationWay->setEnabled(true);
        break;
    }

    ui->checkCorrTurnOn->setChecked(mSettings->getActiveConfig()->mCorrTurnedOn);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void OptionsDialog::on_buttonBox_accepted()
{
    RenderConfig *renderConfig = mSettings->getActiveConfig();

    renderConfig->mDelays.mDXY_LaserOn = ui->spinBoxDXYLaserOn->value();
    renderConfig->mDelays.mDXY_LaserOff = ui->spinBoxDXYLaserOff->value();
    renderConfig->mDelays.mLaserOnDelay = ui->spinBoxLaserOnDelay->value();
    renderConfig->mDelays.mLaserOffDelay = ui->spinBoxLaserOffDelay->value();
    renderConfig->mDelays.mPointDelay = ui->spinBoxPointDelay->value();
    renderConfig->mDelays.mQSWPeriod = ui->spinBoxQSWPeriod->value();
    renderConfig->mDelays.mQSWDuration = ui->spinBoxQSWDuration->value();
    renderConfig->mDrawIdleMotion = ui->checkBoxDrawIdleMotion->isChecked();

    if (ui->radioFastDraw->isChecked())
        renderConfig->mRenderMode = FAST;
    else
    {
        if (ui->radioLines->isChecked())
            renderConfig->mRenderMode = LINES;
        else
            renderConfig->mRenderMode = STEPS;
    }

    mSettings->getActiveConfig()->mCorrTurnedOn = ui->checkCorrTurnOn->isChecked();
    done(1);
}

void OptionsDialog::on_buttonBox_rejected()
{
    done(0);
}

void OptionsDialog::on_radioFastDraw_toggled(bool checked)
{
    if (checked)
        ui->groupLaserEmulationWay->setEnabled(false);
}

void OptionsDialog::on_radioLaserEmulation_toggled(bool checked)
{
    if (checked)
        ui->groupLaserEmulationWay->setEnabled(true);
}
