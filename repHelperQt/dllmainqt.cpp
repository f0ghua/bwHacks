#include "qmfcapp.h"
#include "rephelperqt.h"

#include <windows.h>

#ifndef F_NO_DEBUG
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QDebug>

/**
 * Function registed for log to file with qDebug/qWarning/qCritical/qFatal
 * @param type	[description]
 * @param context [description]
 * @param msg	[description]
 */
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
        case QtInfoMsg:
            text = QString("Info:");
            break;

        case QtDebugMsg:
            text = QString("Debug:");
            break;

        case QtWarningMsg:
            text = QString("Warning:");
            break;

        case QtCriticalMsg:
            text = QString("Critical:");
            break;

        case QtFatalMsg:
            text = QString("Fatal:");
            break;
    }

    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString message = QString("[%1] %2 %3").arg(current_date_time).arg(text).arg(msg);

    QFile file("c:\\traceLog.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message.toUtf8().constData() << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

static void initMessageHandler()
{
    qInstallMessageHandler(outputMessage);
#ifndef F_NO_DEBUG
    qDebug() << "busInsight is initializing ...";
#endif
}
#else
#define initMessageHandler()
#endif

BOOL WINAPI DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpvReserved*/ )
{
    static bool ownApplication = FALSE;
    RepHelperQt *rhq = NULL;

    if ( dwReason == DLL_PROCESS_ATTACH ) {
        ownApplication = QMfcApp::pluginInstance( hInstance );
        initMessageHandler();
        rhq = new RepHelperQt();
    }
    if ( dwReason == DLL_PROCESS_DETACH && ownApplication ) {
        if (rhq) delete rhq;
        delete qApp;
    }

    return TRUE;
}
