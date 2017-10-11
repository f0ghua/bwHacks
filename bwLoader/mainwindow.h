#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    bool AdjustPrivileges();
    bool InjectDll();

    Ui::MainWindow *ui;
    QString m_className;
};

#endif // MAINWINDOW_H
