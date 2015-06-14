#ifndef AUTOPLAYOPTIONSDIALOG_H
#define AUTOPLAYOPTIONSDIALOG_H

#include <QDialog>
#include "options.h"

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent, const OptionsData& timeIntervals);
    ~OptionsDialog();

    OptionsData GetOptions() const;

private:
    Ui::OptionsDialog *m_ui;

    OptionsData m_options;

    void AddAutoPlayWidget( const QString& label, unsigned int& value);
};

#endif // AUTOPLAYOPTIONSDIALOG_H
