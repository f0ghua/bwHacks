#ifndef REPHELPERQT_P_H
#define REPHELPERQT_P_H

#include "rephelperqt.h"

#include <QObject>
#include <QTimer>
#include <QDateTime>

class RepHelperQtPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(RepHelperQt)
public:
    RepHelperQtPrivate(RepHelperQt *q);
    ~RepHelperQtPrivate();

    RepHelperQt *q_ptr;
    QTimer m_timer;
    QString m_scInstallDir;
    QDateTime m_lastRepTime;

public slots:
    void updateReplay();
};

#endif // REPHELPERQT_P_H
