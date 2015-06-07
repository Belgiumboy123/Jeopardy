#ifndef AUTOPLAYOPTIONSDIALOG_H
#define AUTOPLAYOPTIONSDIALOG_H

#include <QDialog>

typedef unsigned int UINT;

struct TimeIntervals
{
    UINT ClueQuestion; // 10000
    UINT FinalStart; // 3000
    UINT FinalCategory; // 7000
    UINT FinalQuestion; // 30000
    UINT FinalAnswer; // 5000
    UINT AutoPlayAnimation; //400
    UINT AutoPlayFinal; //1500
    UINT ClueTimeOut; // 2500
    UINT ClueAnswer; // 2000
    UINT GameOver; // 5000

    // this will set the defaults
    TimeIntervals();
};

namespace Ui {
class AutoPlayOptionsDialog;
}

class AutoPlayOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoPlayOptionsDialog(QWidget *parent = 0);
    ~AutoPlayOptionsDialog();

    void SetTimeIntervals( const TimeIntervals& timeIntervals );
    TimeIntervals GetTimeIntervals() const;

private:
    Ui::AutoPlayOptionsDialog *m_ui;

    TimeIntervals m_timeIntervals;

    void AddAutoPlayWidget( const QString& label, unsigned int& value);
};

#endif // AUTOPLAYOPTIONSDIALOG_H
