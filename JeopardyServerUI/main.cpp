#include "mainwindow.h"

#include "jeopardydatabase.h"
#include "jeopardyservernoui.h"

#include <QApplication>
#include <QDebug>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    const QString serverSettings = DatabaseUtils::GetFilePathAppResourcesFile("server_settings.data");
    QFile file(serverSettings);
    QString port;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!file.atEnd())
        {
            QString line = file.readLine();
            QStringList valuePair = line.split("=");
            if( valuePair.size() == 2)
            {
                if( valuePair.at(0).toUpper() == "PORT")
                {
                    port = valuePair.at(1);
                }
            }
        }
    }
    file.close();

    JeopardyServerNoUi server;
    if(server.Start(port))
    {
        // server started
        return app.exec();
    }

    if( argc == 2)
    {
        QString portNumberArg = QCoreApplication::arguments().at(1);

        JeopardyServerNoUi server;
        if( server.Start(portNumberArg) )
        {
            // server started
            return app.exec();
        }
    }

    // If there any problems with command line arguments
    // default to the UI interface
    MainWindow w;
    w.show();

    return app.exec();
}
