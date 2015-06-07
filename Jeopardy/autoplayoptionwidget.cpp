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

    // For some reason the following line does not compile
    // Which is why the ugly SetValue workaround is needed
    //connect( m_ui->spinBox, &QSpinBox::valueChanged, this, &AutoPlayOptionWidget::onValueChanged);
}

void
AutoPlayOptionWidget::onValueChanged(int value)
{
    m_value = value;
}

void
AutoPlayOptionWidget::SetValue()
{
    m_value = m_ui->spinBox->value();
}

AutoPlayOptionWidget::~AutoPlayOptionWidget()
{
    delete m_ui;
}
