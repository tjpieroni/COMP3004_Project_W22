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

    //device starts powered off and not in a session.
    powerStatus = false;
    therapyInProgress = false;
    shuttingDown = false;
    earClipsConnected = false;
    earsWet = false;

    //setup timers to facilitate holding buttons achieving different functionality from clicking
    powerButtonTimer = new QTimer(this);
    upIntensityTimer = new QTimer(this);
    downIntensityTimer = new QTimer(this);
    connectTestTimer = new QTimer(this);

    connect(powerButtonTimer, SIGNAL(timeout()), this,SLOT(togglePower()));
    connect(upIntensityTimer, SIGNAL(timeout()), this,SLOT(increaseIntensity()));
    connect(downIntensityTimer, SIGNAL(timeout()), this,SLOT(decreaseIntensity()));
    connect(connectTestTimer, SIGNAL(timeout()), this,SLOT(checkConnection()));

    //connect buttons to their respective slots
    connect(ui->powerBtn, SIGNAL(pressed()), this,SLOT(startPowerTimer()));
    connect(ui->powerBtn, SIGNAL(released()), this,SLOT(handlePowerButton()));
    connect(ui->upBtn, SIGNAL(pressed()), this,SLOT(startupIntensityTimer()));
    connect(ui->upBtn,SIGNAL(released()),this,SLOT(selectUpSession()));
    connect(ui->downBtn, SIGNAL(pressed()), this,SLOT(startdownIntensityTimer()));
    connect(ui->downBtn,SIGNAL(released()),this,SLOT(selectDownSession()));
    connect(ui->checkBtn, SIGNAL(clicked()), this, SLOT(confirmTreatment()));
    connect(ui->saveBtn, SIGNAL(clicked()), this, SLOT(saveTreatment()));

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
    Group* twentyGrp = new Group(ui->TwentySession, "20");
    Group* fortyFiveGrp = new Group(ui->FortyFiveSession, "45");
    Group* userDesignatedGrp = new Group(ui->UserDesignatedSession, "UD");

    //can change the name of these variables when we decide on the sessions we'll do
    session* one = new session(ui->SessionType1, "Session Type 1");
    session* two = new session(ui->SessionType2, "Session Type 2");
    session* three = new session(ui->SessionType3,  "Session Type 3");
    session* four = new session(ui->SessionType4,  "Session Type 4");

    groupList.append(twentyGrp);
    groupList.append(fortyFiveGrp);
    groupList.append(userDesignatedGrp);

    sessionList.append(one);
    sessionList.append(two);
    sessionList.append(three);
    sessionList.append(four);

    recordingList = Database->retrieveRecordings();
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
        ui->saveBtn->blockSignals(true);
        therapyInProgress = false;
    }
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
    if(upIntensityTimer->isActive() && !therapyInProgress){
        upIntensityTimer->stop();
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

void MainWindow::selectDownSession(){
    if(downIntensityTimer->isActive() && !therapyInProgress){
        downIntensityTimer->stop();
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

void MainWindow::confirmTreatment(){
    qInfo("Recieved CONFIRM TREATMENT signal");
    if (currSelectedGrp > -1 && currSelectedSess > -1) {
        therapyInProgress = true;
        ui->checkBtn->blockSignals(true);
        ui->saveBtn->blockSignals(false);
        qInfo() << "Confirmed Group: " << groupList.at(currSelectedGrp)->getName();
        confirmedGrp = groupList.at(currSelectedGrp)->getName();
        qInfo() << "Confirmed Session: " << sessionList.at(currSelectedSess)->getName();

        if (groupList.at(currSelectedGrp)->getName() == "20") {
            //twenty session
            qInfo("Twenty session starting...");
            beginSession();
        } else if (groupList.at(currSelectedGrp)->getName() == "45") {
            qInfo("Forty-Five session starting...");
            beginSession();
        } else if (groupList.at(currSelectedGrp)->getName() == "UD") {
            qInfo("User-designated group starting...");
            beginSession();
        } else {
            qInfo("Incalid group");
        }
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

/*
 *Function: beginSession
 * Input: initialize the session timer  and call initTimer with it
 *
*/
void MainWindow::beginSession() {

    if(checkConnection() == 3){
        connectTestTimer->start(5000);
        return;
    }
    qInfo("Beginning session!");
    sessionTimer = new QTimer(this);
    initTimer(sessionTimer);
}
/*Function: initTimer
 * Input: QTimer object pointer
 * Purpose: initialize the session count to 0, connect the timer to the updateTimer function and make it run every second
 *
 *
*/
void MainWindow::initTimer(QTimer *timer) {
    currTimerCount = 0;
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
    timer->start(1000);
}

/*
 * Function: updateTimer
 * Input: none
 * Purpose: update the timer count every second (increment by 1) with the help of its sessionTimer's initialization in initTimer.
 *          Cancel the timer when it reaches the confirmed group's corresponding time.
 *
*/
void MainWindow::updateTimer() {
    currTimerCount++;
    qInfo() << "Elapsed: " << currTimerCount;
    if (confirmedGrp == "20") {
        if (currTimerCount == 20) {
            sessionTimer->stop();
            qInfo("End of 20 session.");
            //sessionTimer->disconnect();
            //Not sure if this is the right place to reset but gonna do it anyways for now, also not sure about disconnecting here
            currTimerCount = 0;
        }
    } else if (confirmedGrp == "45") {
        if (currTimerCount == 45) {
            sessionTimer->stop();
            qInfo("End of 45 session");
            //sessionTimer->disconnect();
            //Not sure if this is the right place to reset but gonna do it anyways for now, also not sure about disconnecting here
            currTimerCount = 0;
        }
    } else if (confirmedGrp == "UD") {
        if (currTimerCount == 10) {
            sessionTimer->stop();
            qInfo("End of user-designated session");
            //sessionTimer->disconnect();
            //Not sure if this is the right place to reset but gonna do it anyways for now, also not sure about disconnecting here
            currTimerCount = 0;
        }
    }
}

/*
 * Function: checkConnection()
 * Input: none
 * Purpose: check the connection between the device and the users ears depending on if the ear clips are connected and the ears are wet or not.
 *
*/
int MainWindow::checkConnection() {

    if(earClipsConnected){
        if(earsWet){
            qInfo("Strong Connection.");
            if(connectTestTimer->isActive()){
                connectTestTimer->stop();
                beginSession();
            }
            return 1;
        }
        else{
           qInfo("Okay Connection.");
           if(connectTestTimer->isActive()){
               connectTestTimer->stop();
               beginSession();
           }
           return 2;
        }
    }
    else {
        qInfo("No Connection");
        return 3;
    }
}
