#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();

    //set up icon display (might be able to just do this on the ui)
    ui->TwentySession->setEnabled(false);
    ui->TwentySession->setStyleSheet("QPushButton {border: none}");
    ui->FortyFiveSession->setEnabled(false);
    ui->FortyFiveSession->setStyleSheet("QPushButton {border: none}");
    ui->UserDesignatedSession->setEnabled(false);
    ui->UserDesignatedSession->setStyleSheet("QPushButton {border: none}");

    ui->SessionType1->setEnabled(false);
    ui->SessionType1->setStyleSheet("QPushButton {border: none}");
    ui->SessionType2->setEnabled(false);
    ui->SessionType2->setStyleSheet("QPushButton {border: none}");
    ui->SessionType3->setEnabled(false);
    ui->SessionType3->setStyleSheet("QPushButton {border: none}");
    ui->SessionType4->setEnabled(false);
    ui->SessionType4->setStyleSheet("QPushButton {border: none}");

    //setup timers to facilitate holding buttons achieving different functionality from clicking
    powerButtonTimer = new QTimer(this);
    upIntensityTimer = new QTimer(this);
    downIntensityTimer = new QTimer(this);
    connectTestTimer = new QTimer(this);
    sessionTimer = new QTimer(this);

    connect(powerButtonTimer, SIGNAL(timeout()), this,SLOT(togglePower()));
    connect(upIntensityTimer, SIGNAL(timeout()), this,SLOT(increaseIntensity()));
    connect(downIntensityTimer, SIGNAL(timeout()), this,SLOT(decreaseIntensity()));
    connect(connectTestTimer, SIGNAL(timeout()), this,SLOT(checkConnection()));
    connect(sessionTimer, SIGNAL(timeout()), this, SLOT(updateTimer()));

    //connect buttons to their respective slots
    connect(ui->powerBtn, SIGNAL(pressed()), this,SLOT(startPowerTimer()));
    connect(ui->powerBtn, SIGNAL(released()), this,SLOT(handlePowerButton()));
    connect(ui->upBtn, SIGNAL(pressed()), this,SLOT(startupIntensityTimer()));
    connect(ui->upBtn,SIGNAL(released()),this,SLOT(selectUpSession()));
    connect(ui->downBtn, SIGNAL(pressed()), this,SLOT(startdownIntensityTimer()));
    connect(ui->downBtn,SIGNAL(released()),this,SLOT(selectDownSession()));
    connect(ui->checkBtn, SIGNAL(clicked()), this, SLOT(confirmTreatment()));
    connect(ui->saveBtn, SIGNAL(clicked()), this, SLOT(saveTreatment()));
    connect(ui->DampenEars, SIGNAL(clicked()),this,SLOT(dampenEar()));
    connect(ui->ConnectEarclips, SIGNAL(clicked()),this,SLOT(connectEarclips()));

    //block all buttons except the power button
    ui->checkBtn->blockSignals(true);
    ui->downBtn->blockSignals(true);
    ui->upBtn->blockSignals(true);
    ui->saveBtn->blockSignals(true);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::init() {
    Database = new dbManager();
    Group* twentyGrp = new Group(ui->TwentySession, "20", 20);
    Group* fortyFiveGrp = new Group(ui->FortyFiveSession, "45", 45);
    Group* userDesignatedGrp = new Group(ui->UserDesignatedSession, "UD", 10);

    //can change the name of these variables when we decide on the sessions we'll do
    session* one = new session(ui->SessionType1, "Session Type 1");
    session* two = new session(ui->SessionType2, "Session Type 2");
    session* three = new session(ui->SessionType3,  "Session Type 3");
    session* four = new session(ui->SessionType4,  "Session Type 4");

    // initialize MainWindow QVectors
    groupList = {twentyGrp,fortyFiveGrp,userDesignatedGrp};
    sessionList = {one,two,three,four};
    recordingList = Database->retrieveRecordings();
    intensityLevels = {ui->lvl1, ui->lvl2, ui->lvl3, ui->lvl4, ui->lvl5, ui->lvl6, ui->lvl7, ui->lvl8};
}

void MainWindow::startPowerTimer(){
    powerButtonTimer->start(3000);
}

void MainWindow::resetAppearance(){
    if (currSelectedGrp != -1){
        groupList.at(currSelectedGrp)->getBtnWidget()->setStyleSheet("QPushButton {border: none}");
    } if (currSelectedSess != -1) {
        sessionList.at(currSelectedSess)->getBtnWidget()->setStyleSheet("QPushButton{border: none}");
    }
    currSelectedGrp = -1;
    currSelectedSess = -1;
    displayOff_intensity();
    ui->powerIndicator->setStyleSheet("QLabel {background-color: rgb(255,255,255);}");
}

// turn on and off the power of the device, block/unblock signals from appropriate buttons
void MainWindow::togglePower(){
    powerButtonTimer->stop();
    if(powerStatus){
        qInfo("Recieved POWER OFF signal");
        endSession();
        resetAppearance();
        powerStatus = false;
        ui->checkBtn->blockSignals(true);
        ui->downBtn->blockSignals(true);
        ui->upBtn->blockSignals(true);
    }
    else{
        qInfo("Recieved POWER ON signal");
        powerStatus = true;
        displayOn_intensity();
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
    qInfo("Recieved END SESSION signal");
    powerButtonTimer->stop();
    sessionTimer->stop();
    currTimerCount = 0;
    currentIntensity = 0;
    ui->checkBtn->blockSignals(false);
    ui->saveBtn->blockSignals(true);
    therapyInProgress = false;
}


/*
    Function: selectGroup()
    Purpose: Select a group
    How it works:
        if nothing is currently selected, it will set the currSelected variable to 0 and change the border
        if something is selected
            increment the current selected value
            if it goes out of bounds, set it to 0
            set the border colour
 *
*/
void MainWindow::selectGroup(){
    qInfo("Recieved SELECT GROUP signal");
    if (currSelectedGrp == -1) {
        currSelectedGrp = 0;
        groupList.at(0)->getBtnWidget()->setStyleSheet("QPushButton {border: 3px solid red}");
        qInfo() << "Current selected group: " << groupList.at(currSelectedGrp)->getName();

    } else {
        groupList.at(currSelectedGrp)->getBtnWidget()->setStyleSheet("QPushButton {border: none}");
        currSelectedGrp++;
        if (currSelectedGrp >= groupList.size()) {
            currSelectedGrp = 0;
        }
        groupList.at(currSelectedGrp)->getBtnWidget()->setStyleSheet("QPushButton {border: 3px solid red}");
        qInfo() << "Current selected group: " << groupList.at(currSelectedGrp)->getName();
    }
}

void MainWindow::selectUpSession(){
    if(upIntensityTimer->isActive()){
        upIntensityTimer->stop();
        if(!therapyInProgress){
            qInfo("Recieved SELECT UP SESSION signal");
            if (currSelectedSess == -1) {
                currSelectedSess = 0;
                sessionList.at(0)->getBtnWidget()->setStyleSheet("QPushButton{border: 3px solid red}");
            } else if (currSelectedSess < sessionList.size()-1) {
                sessionList.at(currSelectedSess)->getBtnWidget()->setStyleSheet("QPushButton{border: none}");
                currSelectedSess++;
                sessionList.at(currSelectedSess)->getBtnWidget()->setStyleSheet("QPushButton{border: 3px solid red}");
            }
        }
    }
}

void MainWindow::selectDownSession(){
    if(downIntensityTimer->isActive()){
        downIntensityTimer->stop();
        if(!therapyInProgress){
            qInfo("Recieved SELECT DOWN SESSION signal");
            if (currSelectedSess == -1) {
                currSelectedSess = 0;
                sessionList.at(0)->getBtnWidget()->setStyleSheet("QPushButton{border: 3px solid red}");
            } else if (currSelectedSess > 0) {
                sessionList.at(currSelectedSess)->getBtnWidget()->setStyleSheet("QPushButton{border: none}");
                currSelectedSess--;
                sessionList.at(currSelectedSess)->getBtnWidget()->setStyleSheet("QPushButton{border: 3px solid red}");
            }
        }
    }
}

// Confirm the selected Group and Session and if selection is valid, check the connection before starting the therapy.
void MainWindow::confirmTreatment(){
    qInfo("Recieved CONFIRM TREATMENT signal");
    if (currSelectedGrp > -1 && currSelectedSess > -1) {
        ui->checkBtn->blockSignals(true);
        ui->saveBtn->blockSignals(false);
        qInfo() << "Confirmed Group: " << groupList.at(currSelectedGrp)->getName();
        confirmedGrp = groupList.at(currSelectedGrp)->getName();
        qInfo() << "Confirmed Session: " << sessionList.at(currSelectedSess)->getName();
        checkConnection();

        //should probably reset style here
    } else {
        qInfo("You must select both a Group and Session in order to confirm treatment");
    }

}

void MainWindow::saveTreatment(){
    qInfo("Recieved SAVE TREATMENT signal");
    recording *newRecording = new recording(recordingList.size(),currentIntensity,groupList.at(currSelectedGrp)->getDuration(),sessionList.at(currSelectedSess)->getName());
    recordingList.push_back(newRecording);
    if(Database->insertRecording(newRecording)){
        qInfo("Treatment saved");
    }
    else{
        qInfo("Treatment could not be saved");
    }
}

void MainWindow::startdownIntensityTimer(){
    downIntensityTimer->start(1500);
}

void MainWindow::startupIntensityTimer(){
    upIntensityTimer->start(1500);
}

void MainWindow::increaseIntensity(){
    if(currentIntensity < 8 && therapyInProgress){
        qInfo("Recieved INCREASE INTENSITY signal");
        currentIntensity++;
    }
}

void MainWindow::decreaseIntensity(){
    if(currentIntensity > 0 && therapyInProgress){
        qInfo("Recieved DECREASE INTENSITY signal");
        currentIntensity--;
    }
}

/*
 *Function: beginSession
 * Input: initialize the session timer  and call initTimer with it
 *
*/
void MainWindow::beginSession() {
    qInfo("Beginning session!");
    therapyInProgress = true;
    sessionTimer->start(1000);
}

/*
 * Function: updateTimer
 * Input: none
 * Purpose: update the timer count every second (increment by 1) with the help of its sessionTimer's initialization in initTimer.
 *          Cancel the timer when it reaches the confirmed group's corresponding time.
 *
*/
void MainWindow::updateTimer() {
//    qInfo() << "Elapsed: " << currTimerCount;
    currTimerCount++;
    int total = groupList.at(currSelectedGrp)->getDuration();
    int remaining = total-currTimerCount;

    //get seconds
    int seconds = remaining % 60;
    int temp = remaining-seconds;
    //minutes
    temp /=60;
    int minutes = temp%60;
    //get hours
    int hours = temp-minutes;
    QString timeRemaining = "Remaining Session Time: " + QString::number(hours) + ":" + QString::number(minutes) + ":" + QString::number(seconds);
//    qInfo() << "Remaining: " << timeRemaining;
    ui->remainingTimeLabel->setText(timeRemaining);
    if(currTimerCount == groupList.at(currSelectedGrp)->getDuration()){
        ui->remainingTimeLabel->setText("Remaining Session Time: 0:0:0");
        qInfo("Session Ending...");
        endSession();
    }
}

/*
 * Function: checkConnection()
 * Input: none
 * Purpose: check the connection between the device and the users ears depending on if the ear clips are connected and the ears are wet or not.
 *
*/
void MainWindow::checkConnection() {
    if(earClipsConnected){
        if(earsWet){
            qInfo("Strong Connection.");

            connectTestTimer->stop();
            beginSession();
        }
        else{
            qInfo("Okay Connection.");

            connectTestTimer->stop();
            beginSession();
        }
    }
    else {
        qInfo("No Connection");
        connectTestTimer->start(2000);
    }
}

void MainWindow::connectEarclips(){
    if(earClipsConnected){
        earClipsConnected=false;
        qInfo("Earclips Disconnected");
        ui->EarclipsConnected->setStyleSheet("QLabel {background-color: rgb(239, 41, 41);}");
        if(therapyInProgress){
            sessionTimer->stop();
            checkConnection();
        }
    }
    else{
        qInfo("Earclips Connected");
        earClipsConnected=true;
        ui->EarclipsConnected->setStyleSheet("QLabel {background-color: rgb(138, 226, 52);}");
    }
}

void MainWindow::dampenEar(){
    if(earsWet){
        earsWet=false;
        qInfo("Ears Dry");
        ui->EarsDampened->setStyleSheet("QLabel {background-color: rgb(239, 41, 41);}");
    }
    else{
        qInfo("Ears Damp");
        earsWet=true;
        ui->EarsDampened->setStyleSheet("QLabel {background-color: rgb(138, 226, 52);}");
    }
}

void MainWindow::displayOff_intensity() {
    for (int i = 0; i < 8; i++) {
        intensityLevels.at(i)->setStyleSheet("QLabel {background-color: rgb(255, 255, 255);}");
    }
}

void MainWindow::displayOn_intensity() {
    //power indicator
    ui->powerIndicator->setStyleSheet("QLabel {background-color: rgb(138, 226, 52);}");
    //levels 1-3
    for (int i = 0; i < 3; i++) {
        intensityLevels.at(i)->setStyleSheet("QLabel {background-color: rgb(138, 226, 52);}");
    }
    //levels 4-6
    for (int i = 3; i < 6; i++) {
        intensityLevels.at(i)->setStyleSheet("QLabel {background-color: rgb(252, 233, 79);}");
    }
    //levels 7 and 8
    for (int i = 6; i < 8; i++) {
        intensityLevels.at(i)->setStyleSheet("QLabel {background-color: rgb(239, 41, 41);}");
    }
}
