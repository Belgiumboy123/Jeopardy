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
    explicit OptionsDialog(QWidget *parent, const OptionsData& options);
    virtual ~OptionsDialog();

    OptionsData GetOptions() const;

private:
    void OnDefaultButtonClicked();

    std::unique_ptr<Ui::OptionsDialog> m_ui;

    OptionsData m_options;
};

#endif // AUTOPLAYOPTIONSDIALOG_H
