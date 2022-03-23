#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPushButton>
#include <QPixmap>
#include <QDebug>

#include "group.h"
#include "session.h"
#include "dbmanager.h"
#include "recording.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //Check connection when session is started, true = good connection false = no connection
    bool checkConnection();
    void init();
private slots:


private:
    Ui::MainWindow *ui;
    QTimer *powerButtonTimer,*upIntensityTimer,*downIntensityTimer;
    QVector<Group*> groupList;
    QVector<session*> sessionList;
    QVector<recording*> recordingList;
    dbManager *Database;
    bool powerStatus,therapyInProgress,shuttingDown;
    int currSelectedGrp = -1;
    int currSelectedSess = -1;
    int currentIntensity = 1;

private slots:
    void startPowerTimer();
    void handlePowerButton();
    void togglePower();
    void endSession();
    void selectGroup();
    void confirmTreatment();
    void saveTreatment();

    void selectUpSession();
    void selectDownSession();
    void startupIntensityTimer();
    void startdownIntensityTimer();
    void increaseIntensity();
    void decreaseIntensity();
};
#endif // MAINWINDOW_H
