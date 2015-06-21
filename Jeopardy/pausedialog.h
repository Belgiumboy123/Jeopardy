#ifndef PAUSEDDIALOG_H
#define PAUSEDDIALOG_H

#include <QDialog>

namespace Ui {
class PauseDialog;
}

class PauseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PauseDialog(QWidget *parent, const QColor& textColor);
    ~PauseDialog();

protected:
    virtual void keyPressEvent(QKeyEvent* event);

private:
    Ui::PauseDialog* m_ui;
};

#endif // PAUSEDDIALOG_H

