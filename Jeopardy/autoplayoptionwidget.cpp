#include "autoplayoptionwidget.h"
#include "ui_autoplayoptionwidget.h"

AutoPlayOptionWidget::AutoPlayOptionWidget(QWidget *parent, const QString& label, unsigned int& value)
    : QWidget(parent)
    , m_ui(new Ui::AutoPlayOptionWidget)
    , m_value(value)
{
    m_ui->setupUi(this);

    m_ui->label->setText(label);

    m_ui->spinBox->setMinimum(0);
    m_ui->spinBox->setMaximum(50000);
    m_ui->spinBox->setSingleStep(100);
    m_ui->spinBox->setValue(value);

    // There are two signals with the same name in QSpinBox
    // so we have to do this ugly casting for it to work
    connect( m_ui->spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &AutoPlayOptionWidget::onValueChanged);
}

void
AutoPlayOptionWidget::onValueChanged(int value)
{
    m_value = value;
}

AutoPlayOptionWidget::~AutoPlayOptionWidget()
{
    delete m_ui;
}
