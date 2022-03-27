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


private:
    Ui::MainWindow *ui;
    QTimer *powerButtonTimer,*upIntensityTimer,*downIntensityTimer, *sessionTimer;
    QVector<Group*> groupList;
    QVector<session*> sessionList;
    QVector<recording*> recordingList;
    dbManager *Database;
    bool powerStatus,therapyInProgress,shuttingDown;
    int currSelectedGrp = -1;
    int currSelectedSess = -1;
    int currentIntensity = 1;

    int currTimerCount = -1;
    QString confirmedGrp = "";

    bool earClipsConnected, earsWet;
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
    void updateTimer();
    void initTimer(QTimer*);
    void beginSession();
};
#endif // MAINWINDOW_H
