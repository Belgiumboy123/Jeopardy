#include "pausedialog.h"
#include "ui_pausedialog.h"

#include <QApplication>
#include <QKeyEvent>

PauseDialog::PauseDialog(QWidget *parent, const QColor& textColor)
    : QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint)
    , m_ui(new Ui::PauseDialog)
{
    m_ui->setupUi(this);
    setWindowTitle("");

    auto dialogPal = palette();
    dialogPal.setColor(backgroundRole(), Qt::black);
    dialogPal.setColor(foregroundRole(), textColor);
    setPalette(dialogPal);

    auto headerFont = m_ui->header->font();
    headerFont.setPointSize(40);
    m_ui->header->setFont(headerFont);

    auto buttonFont(headerFont);
    buttonFont.setPointSize(14);
    m_ui->quitButton->setFont(buttonFont);
    m_ui->continueButton->setFont(buttonFont);

    connect( m_ui->quitButton, &QPushButton::clicked, this, &QDialog::reject);
    connect( m_ui->continueButton, &QPushButton::clicked, this, &QDialog::accept);
}

void
PauseDialog::keyPressEvent(QKeyEvent* event)
{
    if( event->key() == Qt::Key_Escape)
    {
        // stop escape from closing the dialog
        return;
    }

    QDialog::keyPressEvent(event);
}

PauseDialog::~PauseDialog()
{
    delete m_ui;
}
