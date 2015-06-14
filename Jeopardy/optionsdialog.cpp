#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "autoplayoptionwidget.h"

#include <QPushButton>
#include <QVBoxLayout>

OptionsDialog::OptionsDialog(QWidget *parent, const OptionsData& options)
    : QDialog(parent)
    , m_ui(new Ui::OptionsDialog)
    , m_options(options)
{
    m_ui->setupUi(this);

    setModal(false);
    setWindowModality(Qt::ApplicationModal);

    connect( m_ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect( m_ui->okButton, &QPushButton::clicked, this, &QDialog::accept);

    QVBoxLayout* layout = new QVBoxLayout(m_ui->container);
    m_ui->container->setLayout( layout );

    TimeIntervals& timeIntervals( m_options.m_timeIntervals);
    AddAutoPlayWidget("Clue Question", timeIntervals.ClueQuestion);
    AddAutoPlayWidget("Clue Timeout", timeIntervals.ClueTimeOut);
    AddAutoPlayWidget("CLue Answer", timeIntervals.ClueAnswer);
    AddAutoPlayWidget("Final Jeopardy Start", timeIntervals.FinalStart);
    AddAutoPlayWidget("Final Jeopardy Category", timeIntervals.FinalCategory);
    AddAutoPlayWidget("Final Jeopardy Question", timeIntervals.FinalQuestion);
    AddAutoPlayWidget("Auto play animation", timeIntervals.AutoPlayAnimation);
    AddAutoPlayWidget("Auto play final clue", timeIntervals.AutoPlayFinal);
}

void
OptionsDialog::AddAutoPlayWidget( const QString& label, unsigned int& value)
{
    AutoPlayOptionWidget* widget = new AutoPlayOptionWidget(m_ui->container, label, value);
    m_ui->container->layout()->addWidget( widget );
}

OptionsData
OptionsDialog::GetOptions() const
{
    return m_options;
}

OptionsDialog::~OptionsDialog()
{
    delete m_ui;
}
