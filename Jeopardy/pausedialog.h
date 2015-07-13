#ifndef PAUSEDDIALOG_H
#define PAUSEDDIALOG_H

#include <QDialog>
#include "options.h"

namespace Ui {
class PauseDialog;
}

class PauseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PauseDialog(QWidget *parent, const QColor& textColor, const OptionsData& options);
    ~PauseDialog();

    OptionsData GetOptions() const;

protected:
    virtual void keyPressEvent(QKeyEvent* event);

private:
    void LaunchOptionsDialog();

    Ui::PauseDialog* m_ui;

    OptionsData m_options;
};

#endif // PAUSEDDIALOG_H

