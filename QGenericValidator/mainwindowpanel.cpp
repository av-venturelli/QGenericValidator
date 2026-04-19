#include "mainwindowpanel.h"
#include "ui_mainwindowpanel.h"

#include "validationManager.h"

MainWindowPanel::MainWindowPanel(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowPanel)
    , saveBtn_validator( new ValidationManager(this) )
{
    ui->setupUi(this);
    ui->btn_save->setEnabled(false);

    saveBtn_validator->registerQObject(
        ui->chb_agree, &QCheckBox::checkStateChanged,
        [](QCheckBox* chbox) -> ValidationResult {
            if( chbox->checkState() != Qt::CheckState::Unchecked ) return ValidationResult::Success(); return ValidationResult::Failure("The policy agreement must be checked");});


    ui->hsl_slider->setRange(0, 100);
    saveBtn_validator->registerQObject(
        ui->hsl_slider, &QSlider::valueChanged,
        [](QSlider* slider) -> ValidationResult {
            if( slider->value() < 50)
                return ValidationResult::Failure("Must be over 50 the value");
            return ValidationResult::Success();
        });

    ui->sbx_age->setRange(1,80);
    saveBtn_validator->registerQObject(
        ui->sbx_age, &QSpinBox::valueChanged,
        [](QSpinBox* age) -> ValidationResult {
            if( age->value() < 18)
                return ValidationResult::Failure("Must be 18yo");
            return ValidationResult::Success();
        });

    saveBtn_validator->registerQObject( ui->le_mail );

    QObject::connect(
        saveBtn_validator,
        &ValidationManager::formValidityChanged,
        ui->btn_save, [this](bool isValid){ this->ui->btn_save->setEnabled(isValid); } );

}

MainWindowPanel::~MainWindowPanel()
{
    delete ui;
}