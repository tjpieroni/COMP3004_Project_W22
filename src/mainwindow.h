#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPushButton>
#include <QPixmap>
#include <QDebug>
#include <QLabel>
#include <cmath>
#include <QVariant>
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
    // reset device to appearance when powered off
    void resetAppearance();
    void displayIntensity(int);
    void displayRecording(int);

private:
    Ui::MainWindow *ui;
    QTimer *powerButtonTimer, *batteryTimer, *upIntensityTimer,*downIntensityTimer, *sessionTimer, *connectTestTimer, *beginSessionTimer, *displayBatteryTimer;
    QVector<Group*> groupList;
    QVector<session*> sessionList;
    QVector<recording*> recordingList;
    QVector<QLabel*> intensityLevels;
    dbManager *Database;
    bool powerStatus = false,therapyInProgress = false;
    int currSelectedGrp = -1;
    int currSelectedSess = -1;
    int currentIntensity = 0;
    float batteryLevel;
    int currRecording = 0;
    int currTimerCount = 0;
    QString confirmedGrp = "";

    bool earClipsConnected = false, earsWet = false;
private slots:
    void startPowerTimer();
    void handlePowerButton();
    void togglePower();
    void confirmTreatment();
    void saveTreatment();
    void beginSession();
    void selectUpSession();
    void selectDownSession();
    void startupIntensityTimer();
    void startdownIntensityTimer();
    void increaseIntensity();
    void decreaseIntensity();
    void updateTimer();
    void nextRecording();
    void previousRecording();
    void updateConnectionQuality();
    void checkConnection();
    void connectEarclips();
    void dampenEar();
    void chargeBattery();

    void iniBattery();
    void updateBattery();
};
#endif // MAINWINDOW_H
