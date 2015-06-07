#ifndef AUTOPLAYOPTIONWIDGET_H
#define AUTOPLAYOPTIONWIDGET_H

#include <QWidget>

namespace Ui {
class AutoPlayOptionWidget;
}

class AutoPlayOptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AutoPlayOptionWidget(QWidget *parent, const QString& label, unsigned int& value);
    ~AutoPlayOptionWidget();

    void SetValue();

private:
    Ui::AutoPlayOptionWidget *m_ui;

    unsigned int& m_value;

    Q_SLOT void onValueChanged(int value);
};

#endif // AUTOPLAYOPTIONWIDGET_H
