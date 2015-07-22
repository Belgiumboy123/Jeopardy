#include "optionsdialog.h"
#include "ui_optionsdialog.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

#include <QDebug>

/**
 * class OptionsSpinBox
 */
class OptionsSpinBox : public QSpinBox
{
public:
    OptionsSpinBox(QWidget* parent, int& value)
        : QSpinBox(parent)
        , m_value(value)
    {
        // There are two signals with the same name in QSpinBox
        // so we have to do this ugly casting for it to work
        connect( this, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &OptionsSpinBox::onValueChanged);
    }

    virtual ~OptionsSpinBox() = default;

private:
    void onValueChanged(int value)
    {
        m_value = value;
    }

    int& m_value;
};

/**
 * class AutoPlayTab
 */
class AutoPlayTab : public QWidget
{
public:
    AutoPlayTab(QWidget* parent, TimeIntervals& timeIntervals)
        : QWidget(parent)
    {
        m_layout = new QGridLayout(this);
        setLayout( m_layout );

        AddAutoPlayWidgets("Clue Question", timeIntervals.ClueQuestion, 0);
        AddAutoPlayWidgets("Clue Timeout", timeIntervals.ClueTimeOut, 1);
        AddAutoPlayWidgets("Clue Answer", timeIntervals.ClueAnswer, 2);
        AddAutoPlayWidgets("Final Jeopardy Start", timeIntervals.FinalStart, 3);
        AddAutoPlayWidgets("Final Jeopardy Category", timeIntervals.FinalCategory, 4);
        AddAutoPlayWidgets("Final Jeopardy Question", timeIntervals.FinalQuestion, 5);
        AddAutoPlayWidgets("Auto play animation", timeIntervals.AutoPlayAnimation, 6);
        AddAutoPlayWidgets("Auto play final clue", timeIntervals.AutoPlayFinal, 7);
    }

    void SetValues(TimeIntervals& timeIntervals)
    {
        setValue( timeIntervals.ClueQuestion, 0);
        setValue( timeIntervals.ClueTimeOut, 1);
        setValue( timeIntervals.ClueAnswer, 2);
        setValue( timeIntervals.FinalStart, 3);
        setValue( timeIntervals.FinalCategory, 4);
        setValue( timeIntervals.FinalQuestion, 5);
        setValue( timeIntervals.AutoPlayAnimation, 6);
        setValue( timeIntervals.AutoPlayFinal, 7);
    }

    virtual ~AutoPlayTab() = default;

private:

    void AddAutoPlayWidgets( const QString& text, int& value, int row)
    {
        QLabel* label = new QLabel(text, this);
        OptionsSpinBox* spinBox = new OptionsSpinBox(this, value);
        spinBox->setMinimum(0);
        spinBox->setMaximum(50000);
        spinBox->setSingleStep(100);
        spinBox->setValue(value);

        m_layout->addWidget(label, row, 0);
        m_layout->addWidget(spinBox, row, 1);
    }

    void setValue( int& value, int row)
    {
        auto spinBox = dynamic_cast<QSpinBox*>(m_layout->itemAtPosition(row,1)->widget());
        if( spinBox)
        {
            spinBox->setValue(value);
        }
    }

    QGridLayout* m_layout;
};

/**
  * Class SoundTab
  */
class SoundTab : public QWidget
{
public:
    SoundTab(QWidget* parent, SoundOptions& musicOptions)
        : QWidget(parent)
        , m_playMusic(musicOptions.playFinalJeopardy)
        , m_volume(musicOptions.volume)
    {
        QGridLayout* layout = new QGridLayout(this);
        setLayout( layout );

        QLabel* label = new QLabel("Play Final Jeopardy Tune:", this);
        m_playMusicCheckBox = new QCheckBox(this);
        m_playMusicCheckBox->setChecked(m_playMusic);
        connect( m_playMusicCheckBox, &QCheckBox::toggled, this, &SoundTab::OnMusicToggled);

        layout->addWidget( label, 0, 0);
        layout->addWidget( m_playMusicCheckBox, 0, 1);

        m_sliderLabel = new QLabel("Volume: " + QString::number(m_volume), this);
        m_sliderLabel->setEnabled(m_playMusic);
        m_slider = new QSlider(Qt::Horizontal, this);
        m_slider->setMinimum(0);
        m_slider->setMaximum(100);
        m_slider->setEnabled(m_playMusic);
        m_slider->setValue(m_volume);
        connect( m_slider, &QAbstractSlider::valueChanged, this, &SoundTab::OnVolumeChanged);

        layout->addWidget( m_sliderLabel, 1, 0);
        layout->addWidget( m_slider, 1, 1);
    }

    void SetValues(SoundOptions& musicOptions)
    {
        m_playMusicCheckBox->setChecked(musicOptions.playFinalJeopardy);
        m_slider->setValue(musicOptions.volume);
    }

    virtual ~SoundTab() = default;

private:

    void OnMusicToggled(bool checked)
    {
        m_playMusic = checked;

        m_slider->setEnabled(m_playMusic);
        m_sliderLabel->setEnabled(m_playMusic);
    }

    void OnVolumeChanged(int value)
    {
        m_volume = value;
        m_sliderLabel->setText("Volume: " + QString::number(m_volume));
    }

    bool& m_playMusic;
    int& m_volume;

    QCheckBox* m_playMusicCheckBox;
    QSlider* m_slider;
    QLabel* m_sliderLabel;
};

/**
  * Class NextClueTab
  */
class NextClueTab : public QWidget
{
public:
    NextClueTab(QWidget* parent, NextClueOptions& nextClueOptions)
        : QWidget(parent)
    {
        m_layout = new QGridLayout(this);
        setLayout( m_layout );

        AddWidgets("Chance of switching category", nextClueOptions.NewColumnChance, 0 );
        AddWidgets("Chance of proceeding to next row when switching category", nextClueOptions.NextRowNewColumnChance, 1);
        AddWidgets("Chance of proceeding to next row when *not* switching category", nextClueOptions.NextRowSameColumnChance, 2);
    }

    void SetValues(NextClueOptions& nextClueOptions)
    {
        setValue( nextClueOptions.NewColumnChance, 0);
        setValue( nextClueOptions.NextRowNewColumnChance, 1);
        setValue( nextClueOptions.NextRowSameColumnChance, 2);
    }

    virtual ~NextClueTab() = default;

private:

    void AddWidgets( const QString& text, int& value, int row)
    {
        QLabel* label = new QLabel(text,this);
        label->setWordWrap(true);
        OptionsSpinBox* spinBox = new OptionsSpinBox(this, value);
        spinBox->setMinimum(0);
        spinBox->setMaximum(100);
        spinBox->setSingleStep(1);
        spinBox->setSuffix("%");
        spinBox->setValue(value);

        m_layout->addWidget(label, row, 0);
        m_layout->addWidget(spinBox, row, 1);
    }

    void setValue( int& value, int row)
    {
        auto spinBox = dynamic_cast<QSpinBox*>(m_layout->itemAtPosition(row,1)->widget());
        if( spinBox)
        {
            spinBox->setValue(value);
        }
    }

    QGridLayout* m_layout;
};

/**
 * Class OptionsDialog
 */
OptionsDialog::OptionsDialog(QWidget *parent, const OptionsData& options)
    : QDialog(parent)
    , m_ui(new Ui::OptionsDialog)
    , m_options(options)
{
    m_ui->setupUi(this);

    setModal(false);
    setWindowModality(Qt::ApplicationModal);

    connect( m_ui->defaultButton, &QPushButton::clicked, this, &OptionsDialog::OnDefaultButtonClicked);
    connect( m_ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect( m_ui->okButton, &QPushButton::clicked, this, &QDialog::accept);

    m_ui->tabWidget->addTab(new AutoPlayTab(this, m_options.m_timeIntervals), "Auto Play Timers");
    m_ui->tabWidget->addTab(new SoundTab(this, m_options.m_music), "Sound");
    m_ui->tabWidget->addTab(new NextClueTab(this, m_options.m_nextClueOptions), "Next Clue Chances");
}

void
OptionsDialog::OnDefaultButtonClicked()
{
    m_options = OptionsData();

    auto autoPlayTab = dynamic_cast<AutoPlayTab*>(m_ui->tabWidget->widget(0));
    if( autoPlayTab )
    {
        autoPlayTab->SetValues(m_options.m_timeIntervals);
    }

    auto soundTab = dynamic_cast<SoundTab*>(m_ui->tabWidget->widget(1));
    if( soundTab )
    {
        soundTab->SetValues(m_options.m_music);
    }

    auto nextClueTab = dynamic_cast<NextClueTab*>(m_ui->tabWidget->widget(2));
    if( nextClueTab )
    {
        nextClueTab->SetValues(m_options.m_nextClueOptions);
    }
}

OptionsData
OptionsDialog::GetOptions() const
{
    return m_options;
}

OptionsDialog::~OptionsDialog() {}
