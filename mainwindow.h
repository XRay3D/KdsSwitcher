#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QLineEdit>
#include <QTimer>

#include "pointedit.h"
#include "kds.h"

#include <QFileSystemWatcher> /////////////////////////
#include <QFile> ////////////////////

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    KDS m_kds;
    PointEdit::Point m_points[12];
    int m_numPoint;
    int m_lastPointType;

    QList<QPushButton*> m_pushButtonList;
    QList<QLineEdit*> m_lineEditList;

    void saveSettings();
    void loadSettings();

    QFileSystemWatcher m_watcher; ///////////////////////////
    QTimer m_tmr;
    QSignalMapper m_signalMapper;

private slots:
    void sl();
    void Ping();
    void setAppStyleSheet(); ///////////////////
    void SwitchSlot(int pointNum);
    void CbTypeCurrentIndexChanged(int index);

protected:
    bool eventFilter(QObject* obj, QEvent* event);
};

#endif // MAINWINDOW_H
