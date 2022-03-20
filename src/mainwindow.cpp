#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->TwentySession->setEnabled(false);
    ui->FortyFiveSession->setEnabled(false);
    ui->UserDesignatedSession->setEnabled(false);

    ui->TwentySession->setStyleSheet("QPushButton {border: 3px solid red}");

    //device starts powered off and not in a session.
    powerStatus = false;
    therapyInProgress = false;
    shuttingDown = false;

    //setup timers to facilitate holding buttons achieving different functionality from clicking
    powerButtonTimer = new QTimer(this);
    upIntensityTimer = new QTimer(this);
    downIntensityTimer = new QTimer(this);

    connect(powerButtonTimer, SIGNAL(timeout()), this,SLOT(togglePower()));
    connect(upIntensityTimer, SIGNAL(timeout()), this,SLOT(increaseIntensity()));
    connect(downIntensityTimer, SIGNAL(timeout()), this,SLOT(decreaseIntensity()));

    //connect buttons to their respective slots
    connect(ui->powerBtn, SIGNAL(pressed()), this,SLOT(startPowerTimer()));
    connect(ui->powerBtn, SIGNAL(released()), this,SLOT(handlePowerButton()));
    connect(ui->upBtn, SIGNAL(pressed()), this,SLOT(startupIntensityTimer()));
    connect(ui->upBtn,SIGNAL(released()),this,SLOT(selectUpSession()));
    connect(ui->downBtn, SIGNAL(pressed()), this,SLOT(startdownIntensityTimer()));
    connect(ui->downBtn,SIGNAL(released()),this,SLOT(selectDownSession()));
    connect(ui->checkBtn, SIGNAL(clicked()), this, SLOT(confirmTreatment()));

    //block all buttons except the power button
    ui->checkBtn->blockSignals(true);
    ui->downBtn->blockSignals(true);
    ui->upBtn->blockSignals(true);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::startPowerTimer(){
    powerButtonTimer->start(3000);
}

// turn on and off the power of the device, block/unblock signals from appropriate buttons
void MainWindow::togglePower(){
    powerButtonTimer->stop();
    if(powerStatus){
        qInfo("Recieved POWER OFF signal");
        //if power off is called during a session, end the session (TODO: turn this into just a function call)
        if(therapyInProgress){
            shuttingDown = true;
            endSession();
        }
        shuttingDown = false;
        powerStatus = false;
        ui->checkBtn->blockSignals(true);
        ui->downBtn->blockSignals(true);
        ui->upBtn->blockSignals(true);
    }
    else{
        qInfo("Recieved POWER ON signal");
        powerStatus = true;
        ui->checkBtn->blockSignals(false);
        ui->downBtn->blockSignals(false);
        ui->upBtn->blockSignals(false);
    }
}

/*slot handles all potention signals from power button, to either select a group, end an in progress session, or if the user has held the button for more than 3 seconds,
 * send no signal for the timer has already timed out.*/
void MainWindow::handlePowerButton(){
    if(powerButtonTimer->isActive() && powerStatus){
        powerButtonTimer->stop();
        if(therapyInProgress){
            endSession();
        }
        else{
            selectGroup();
        }
    }
}

void MainWindow::endSession(){
    if(powerButtonTimer->isActive() || shuttingDown){
        powerButtonTimer->stop();
        qInfo("Recieved END SESSION signal");
        ui->checkBtn->blockSignals(false);
        therapyInProgress = false;
    }
}

void MainWindow::selectGroup(){
    qInfo("Recieved SELECT GROUP signal");
}

void MainWindow::selectUpSession(){
    if(upIntensityTimer->isActive() && !therapyInProgress){
        upIntensityTimer->stop();
        qInfo("Recieved SELECT UP SESSION signal");
    }
}

void MainWindow::selectDownSession(){
    if(downIntensityTimer->isActive() && !therapyInProgress){
        downIntensityTimer->stop();
        qInfo("Recieved SELECT DOWN SESSION signal");
    }
}

void MainWindow::confirmTreatment(){
    qInfo("Recieved CONFIRM TREATMENT signal");
    therapyInProgress = true;
    ui->checkBtn->blockSignals(true);
}

void MainWindow::startdownIntensityTimer(){
    downIntensityTimer->start(2000);
}

void MainWindow::startupIntensityTimer(){
    upIntensityTimer->start(2000);
}

void MainWindow::increaseIntensity(){
    upIntensityTimer->stop();
    qInfo("Recieved INCREASE INTENSITY signal");
}

void MainWindow::decreaseIntensity(){
    downIntensityTimer->stop();
    qInfo("Recieved DECREASE INTENSITY signal");
}

