#include "mainwindow.h"
#include <QApplication>

#include "jeopardyservernoui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if( argc == 2)
    {
        QString portNumberArg = QCoreApplication::arguments().at(1);
        bool ok;
        int port = portNumberArg.toInt(&ok);

        if( ok )
        {
            JeopardyServerNoUi server;
            if( server.Start(port) )
            {
                // server started
                return a.exec();
            }
        }
    }

    // If there any problems with command line arguments
    // default to the UI interface
    MainWindow w;
    w.show();

    return a.exec();
}
