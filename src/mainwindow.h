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
    // initialize environment
    void init();
    // end in progress session
    void endSession();
    // cycle through available groups
    void selectGroup();
    // begin selected session after connection verification
    void beginSession();
    // reset device to appearance when powered off
    void resetAppearance();

private:
    Ui::MainWindow *ui;
    QTimer *powerButtonTimer,*upIntensityTimer,*downIntensityTimer, *sessionTimer, *connectTestTimer;
    QVector<Group*> groupList;
    QVector<session*> sessionList;
    QVector<recording*> recordingList;
    dbManager *Database;
    bool powerStatus = false,therapyInProgress = false;
    int currSelectedGrp = -1;
    int currSelectedSess = -1;
    int currentIntensity = 0;

    int currTimerCount = 0;
    QString confirmedGrp = "";

    bool earClipsConnected = false, earsWet = false;
private slots:
    void startPowerTimer();
    void handlePowerButton();
    void togglePower();
    void confirmTreatment();
    void saveTreatment();

    void selectUpSession();
    void selectDownSession();
    void startupIntensityTimer();
    void startdownIntensityTimer();
    void increaseIntensity();
    void decreaseIntensity();
    void updateTimer();

    void checkConnection();

    void connectEarclips();
    void dampenEar();
};
#endif // MAINWINDOW_H
