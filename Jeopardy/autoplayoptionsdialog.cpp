#include "autoplayoptionsdialog.h"
#include "ui_autoplayoptionsdialog.h"
#include "autoplayoptionwidget.h"

#include <QVBoxLayout>

TimeIntervals::TimeIntervals()
    : ClueQuestion(10000)
    , FinalStart(3000)
    , FinalCategory(7000)
    , FinalQuestion(30000)
    , AutoPlayAnimation(400)
    , AutoPlayFinal(1500)
    , ClueTimeOut(2500)
{
}

AutoPlayOptionsDialog::AutoPlayOptionsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::AutoPlayOptionsDialog)
{
    m_ui->setupUi(this);

    setModal(false);
    setWindowModality(Qt::ApplicationModal);

    QVBoxLayout* layout = new QVBoxLayout(m_ui->container);
    m_ui->container->setLayout( layout );
}

void
AutoPlayOptionsDialog::AddAutoPlayWidget( const QString& label, unsigned int& value)
{
    AutoPlayOptionWidget* widget = new AutoPlayOptionWidget(m_ui->container, label, value);
    m_ui->container->layout()->addWidget( widget );
}

void
AutoPlayOptionsDialog::SetTimeIntervals( const TimeIntervals& timeIntervals )
{
    m_timeIntervals = timeIntervals;

    AddAutoPlayWidget("Clue Question", m_timeIntervals.ClueQuestion);
    AddAutoPlayWidget("Clue Timeout", m_timeIntervals.ClueTimeOut);
    AddAutoPlayWidget("Final Jeopardy Start", m_timeIntervals.FinalStart);
    AddAutoPlayWidget("Final Jeopardy Category", m_timeIntervals.FinalCategory);
    AddAutoPlayWidget("Final Jeopardy Question", m_timeIntervals.FinalQuestion);
    AddAutoPlayWidget("Auto play animation", m_timeIntervals.AutoPlayAnimation);
    AddAutoPlayWidget("Auto play final clue", m_timeIntervals.AutoPlayFinal);
}

TimeIntervals
AutoPlayOptionsDialog::GetTimeIntervals() const
{
    // Until the individual widgets can figure out how to do this on their own
    for( auto child : m_ui->container->children())
    {
        auto autoPlayWidget = dynamic_cast<AutoPlayOptionWidget*>(child);
        if( autoPlayWidget)
        {
            autoPlayWidget->SetValue();
        }
    }

    return m_timeIntervals;
}

AutoPlayOptionsDialog::~AutoPlayOptionsDialog()
{
    delete m_ui;
}
