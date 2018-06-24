#include "rephelperqt.h"
#include "rephelperqt_p.h"

#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QDebug>

RepHelperQtPrivate::RepHelperQtPrivate(RepHelperQt *q)
    :q_ptr(q)
{
    QSettings settings(
                "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Blizzard Entertainment\\Starcraft\\",
                QSettings::NativeFormat);
    m_scInstallDir = settings.value("InstallPath").toString();
#ifndef F_NO_DEBUG
    qDebug() << QObject::tr("path = %1").arg(m_scInstallDir);
#endif
    if (m_scInstallDir.isEmpty())
        return;

    m_timer.setInterval(5*1000);
    //m_timer.setSingleShot(true);
    QObject::connect(&m_timer, &QTimer::timeout, this, &RepHelperQtPrivate::updateReplay);
    m_timer.start();
}

RepHelperQtPrivate::~RepHelperQtPrivate()
{

}

void RepHelperQtPrivate::updateReplay()
{
    QString repRootDir = m_scInstallDir + "\\maps\\replays\\autoReplay";
    QString repTmpFile = m_scInstallDir + "\\maps\\replays\\LastReplay.rep";

    QFileInfo fiTmpFile = QFileInfo(repTmpFile);
    QDateTime lmd = fiTmpFile.lastModified();
    if ( lmd != m_lastRepTime) {
        QString repDirName = repRootDir + "\\" + QLocale(QLocale::C).toString(lmd, "yyyyMMdd");
        QString repFileName = repDirName + "\\" +
                QLocale(QLocale::C).toString(lmd, "yyyyMMddhhmmss") +
                ".rep";
#ifndef F_NO_DEBUG
        qDebug() << QObject::tr("%1, %2").arg(repDirName).arg(repFileName);
#endif

        //QFile file(repTmpFile);
        QDir targetDir(repDirName);
        if(!targetDir.exists()) {
            if(!targetDir.mkpath(targetDir.absolutePath()))
                return;
        }

        QFile::copy(repTmpFile, repFileName);
        m_lastRepTime = lmd;
    }

}

RepHelperQt::RepHelperQt()
    : d_ptr(new RepHelperQtPrivate(this))
{
#ifndef F_NO_DEBUG
    qDebug() << "RepHelperQt contract ...";
#endif
}

RepHelperQt::~RepHelperQt()
{
    delete d_ptr;
}
