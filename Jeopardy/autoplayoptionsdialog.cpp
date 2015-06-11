#include "autoplayoptionsdialog.h"
#include "ui_autoplayoptionsdialog.h"
#include "autoplayoptionwidget.h"

#include <QVBoxLayout>

TimeIntervals::TimeIntervals()
    : ClueQuestion(10000)
    , FinalStart(3000)
    , FinalCategory(7000)
    , FinalQuestion(30000)
    , FinalAnswer(5000)
    , AutoPlayAnimation(400)
    , AutoPlayFinal(1500)
    , ClueTimeOut(2500)
    , ClueAnswer(2000)
    , GameOver(5000)
{
}

AutoPlayOptionsDialog::AutoPlayOptionsDialog(QWidget *parent, const TimeIntervals& timeIntervals)
    : QDialog(parent)
    , m_ui(new Ui::AutoPlayOptionsDialog)
    , m_timeIntervals(timeIntervals)
{
    m_ui->setupUi(this);

    setModal(false);
    setWindowModality(Qt::ApplicationModal);

    QVBoxLayout* layout = new QVBoxLayout(m_ui->container);
    m_ui->container->setLayout( layout );

    AddAutoPlayWidget("Clue Question", m_timeIntervals.ClueQuestion);
    AddAutoPlayWidget("Clue Timeout", m_timeIntervals.ClueTimeOut);
    AddAutoPlayWidget("CLue Answer", m_timeIntervals.ClueAnswer);
    AddAutoPlayWidget("Final Jeopardy Start", m_timeIntervals.FinalStart);
    AddAutoPlayWidget("Final Jeopardy Category", m_timeIntervals.FinalCategory);
    AddAutoPlayWidget("Final Jeopardy Question", m_timeIntervals.FinalQuestion);
    AddAutoPlayWidget("Auto play animation", m_timeIntervals.AutoPlayAnimation);
    AddAutoPlayWidget("Auto play final clue", m_timeIntervals.AutoPlayFinal);
}

void
AutoPlayOptionsDialog::AddAutoPlayWidget( const QString& label, unsigned int& value)
{
    AutoPlayOptionWidget* widget = new AutoPlayOptionWidget(m_ui->container, label, value);
    m_ui->container->layout()->addWidget( widget );
}

TimeIntervals
AutoPlayOptionsDialog::GetTimeIntervals() const
{
    return m_timeIntervals;
}

AutoPlayOptionsDialog::~AutoPlayOptionsDialog()
{
    delete m_ui;
}
