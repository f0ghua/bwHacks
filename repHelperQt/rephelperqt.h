#ifndef REPHELPERQT_H
#define REPHELPERQT_H

#include "rephelperqt_global.h"

class RepHelperQtPrivate;

class REPHELPERQTSHARED_EXPORT RepHelperQt
{
    Q_DECLARE_PRIVATE(RepHelperQt)
public:
    RepHelperQt();
    ~RepHelperQt();

private:
    RepHelperQtPrivate * const d_ptr;
};

#endif // REPHELPERQT_H
