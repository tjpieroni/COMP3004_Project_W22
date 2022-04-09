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
    batteryTimer = new QTimer(this);
    upIntensityTimer = new QTimer(this);
    downIntensityTimer = new QTimer(this);
    connectTestTimer = new QTimer(this);
    sessionTimer = new QTimer(this);
    beginSessionTimer = new QTimer(this);
    displayBatteryTimer = new QTimer(this);

    connect(powerButtonTimer, SIGNAL(timeout()), this,SLOT(togglePower()));
    connect(upIntensityTimer, SIGNAL(timeout()), this,SLOT(increaseIntensity()));
    connect(downIntensityTimer, SIGNAL(timeout()), this,SLOT(decreaseIntensity()));
    connect(connectTestTimer, SIGNAL(timeout()), this,SLOT(checkConnection()));
    connect(sessionTimer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    connect(batteryTimer, SIGNAL(timeout()), this,SLOT(updateBattery()));
    connect(beginSessionTimer,SIGNAL(timeout()),this,SLOT(beginSession()));
    connect(displayBatteryTimer,SIGNAL(timeout()),this,SLOT(iniBattery()));

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
    connect(ui->previousRecording, SIGNAL(clicked()),this,SLOT(previousRecording()));
    connect(ui->nextRecording, SIGNAL(clicked()),this,SLOT(nextRecording()));
    connect(ui->chargeBtn,SIGNAL(clicked()),this,SLOT(chargeBattery()));


    //block all buttons except the power button
    ui->checkBtn->blockSignals(true);
    ui->downBtn->blockSignals(true);
    ui->upBtn->blockSignals(true);
    ui->saveBtn->blockSignals(true);
    ui->previousRecording->blockSignals(true);
    ui->nextRecording->blockSignals(true);
}

MainWindow::~MainWindow(){
    delete ui;
}

/*
 * Function: init
 * In: None
 * Out: None
 * Return: None
 * Purpose: Initializes the database, the groups and session objects corresponding to the ui widgets, the QVectors
 *          containing the newly-created groups and sessions, saved recordings and intensity level labels.
 */
void MainWindow::init() {
    Database = new dbManager();
    Group* twentyGrp = new Group(ui->TwentySession, "20", 20);
    Group* fortyFiveGrp = new Group(ui->FortyFiveSession, "45", 45);
    Group* userDesignatedGrp = new Group(ui->UserDesignatedSession, "UD", 10);

    //can change the name of these variables when we decide on the sessions we'll do
    session* one = new session(ui->SessionType1, "MET");
    session* two = new session(ui->SessionType2, "DELTA");
    session* three = new session(ui->SessionType3,  "ALPHA");
    session* four = new session(ui->SessionType4,  "BETA");

    // initialize MainWindow QVectors
    groupList = {twentyGrp,fortyFiveGrp,userDesignatedGrp};
    sessionList = {one,two,three,four};
    recordingList = Database->retrieveRecordings();
    qInfo()<<"Recordings: "<<recordingList.size();
    intensityLevels = {ui->lvl1, ui->lvl2, ui->lvl3, ui->lvl4, ui->lvl5, ui->lvl6, ui->lvl7, ui->lvl8};
    batteryLevel = Database->retrievePower();
}
/*
 * Function: startPowerTimer
 * In: None
 * Out: None
 * Return: None
 * Purpose: Starts the Timer that will manage events for the device power every 3 seconds
 */
void MainWindow::startPowerTimer(){
    powerButtonTimer->start(3000);
}

/*
 * Function: resetAppearance
 * In: None
 * Out: None
 * Return: None
 * Purpose: Resets the machine by removing current group and session selections and resetting the UI colours
 */
void MainWindow::resetAppearance(){
    if (currSelectedGrp != -1){
        groupList.at(currSelectedGrp)->getBtnWidget()->setStyleSheet("QPushButton {border: none}");
    } if (currSelectedSess != -1) {
        sessionList.at(currSelectedSess)->getBtnWidget()->setStyleSheet("QPushButton{border: none}");
    }
    currSelectedGrp = -1;
    currSelectedSess = -1;
    //displayOff_intensity();
    displayIntensity(0);
    ui->powerIndicator->setStyleSheet("QLabel {background-color: rgb(255,255,255);}");
}
/*
 * Function: togglePower
 * In: None
 * Out: None
 * Return: None
 * Purpose: Powers on/off the device and manages the blocking/unblocking of signals in the corresponding device state
 */
void MainWindow::togglePower(){
    powerButtonTimer->stop();
    if(powerStatus){
        qInfo("Recieved POWER OFF signal");
        batteryTimer->stop();
        displayBatteryTimer->stop();
        endSession();
        resetAppearance();
        powerStatus = false;
        ui->checkBtn->blockSignals(true);
        ui->downBtn->blockSignals(true);
        ui->upBtn->blockSignals(true);
        ui->previousRecording->blockSignals(true);
        ui->nextRecording->blockSignals(true);
        if(Database->updatePower(batteryLevel)){
            qInfo("Battery Level Saved");
        }
    }
    else{
        qInfo("Recieved POWER ON signal");
        if(batteryLevel <= 0){
            qInfo("Battery is dead: recharge");
            return;
        }
        powerStatus = true;
        ui->powerIndicator->setStyleSheet("QLabel {background-color: rgb(138, 226, 52);}");
        batteryTimer->start(5000);
        displayBatteryTimer->start(20000);
        displayIntensity(8);
        displayRecording(0);
        ui->checkBtn->blockSignals(false);
        ui->downBtn->blockSignals(false);
        ui->upBtn->blockSignals(false);
        ui->previousRecording->blockSignals(false);
        ui->nextRecording->blockSignals(false);
        iniBattery();
    }
}

/*
 * Function: handlePowerButton
 * In: None
 * Out: None
 * Return: None
 * Purpose: Handles all power button signals consisting of: group selection, ending an in-progress sesion or sending no signal if the timer has already timed out
 */
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
/*
 * Function:  endSession
 * In: None
 * Out: None
 * Return:  None
 * Purpose: ends a session by stopping the timers for the power button and the session, resetting the intensity level and blocking button signals
 */
void MainWindow::endSession(){
    qInfo("Recieved END SESSION signal");
    powerButtonTimer->stop();
    sessionTimer->stop();
    currTimerCount = 0;
    currentIntensity = 0;
    //displayOff_intensity();
    displayIntensity(0);
    ui->remainingTimeLabel->setText("Remaining Session Time: 0:0:0");
    ui->checkBtn->blockSignals(false);
    ui->saveBtn->blockSignals(true);
    therapyInProgress = false;
}



/*
 * Function: selectGroup
 * In: None
 * Out: None
 * Return: None
 * Purpose: Handles group selection by cycling through the groupList vector
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

/*
 * Function: selectUpSession
 * In: None
 * Out: None
 * Return: None
 * Purpose: Handles session selection on the UI when the up intensity button is pressed by incrementing currSelectedSess
 */
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

/*
 * Function: selectDownSession
 * In:  None
 * Out: None
 * Return: None
 * Purpose:Handles session selection on the UI when the up intensity button is pressed by decrementing currSelectedSess
 */
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

/*
 *Function: confirmTreatment
 * In: None
 * Out: None
 * Return: None
 * Purpose: Confirms the selected group and session type if the selection is valid and checks the connection before beginning the session.
 */
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
/*
 * Function: saveTreatment
 * In: None
 * Out: None
 * Return: None
 * Purpose: Save a treatment by creating a new recording with she treatment information and inserting it into the database
 */
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

/*
 *Function: startdownIntensityTimer
 * In: None
 * Out: None
 * Return: None
 * Purpose: Starts the timer for the down intensity button at a 1.5 second interval
 */
void MainWindow::startdownIntensityTimer(){
    downIntensityTimer->start(1500);
}

/*
 *Function: startupIntensityTimer
 * In: None
 * Out: None
 * Return: None
 * Purpose: Starts the timer for the up intensity button at a 1.5 second interval
 */
void MainWindow::startupIntensityTimer(){
    upIntensityTimer->start(1500);
}
/*
 *Function: increaseIntensity
 * In: None
 * Out: None
 * Return: None
 * Purpose: Increases the intensity by one level up to a maximum of 8
 */
void MainWindow::increaseIntensity(){
    if(currentIntensity < 8 && therapyInProgress){
        qInfo("Recieved INCREASE INTENSITY signal");
        currentIntensity++;
        if(currentIntensity<=3){
            intensityLevels.at(currentIntensity-1)->setStyleSheet("QLabel {background-color: rgb(138, 226, 52);}");
        }
        else if(currentIntensity<=6){
            intensityLevels.at(currentIntensity-1)->setStyleSheet("QLabel {background-color: rgb(252, 233, 79);}");
        }
        else{
            intensityLevels.at(currentIntensity-1)->setStyleSheet("QLabel {background-color: rgb(239, 41, 41);}");
        }
    }
}

/*
 *Function: decreaseIntensity
 * In: None
 * Out: None
 * Return: None
 * Purpose: Decreases the intensity by one level down to a minimum of 0
 */
void MainWindow::decreaseIntensity(){
    if(currentIntensity > 0 && therapyInProgress){
        qInfo("Recieved DECREASE INTENSITY signal");
        intensityLevels.at(currentIntensity-1)->setStyleSheet("QLabel {background-color: rgb(255, 255, 255);}");
        currentIntensity--;
    }
}

/*
 *Function: beginSession
 * In: None
 * Out: None
 * Return: None
 * Purpose: Increases the intensity by one level up to a maximum of 8
 */
void MainWindow::beginSession() {
    beginSessionTimer->stop();
    qInfo("Beginning session!");
    therapyInProgress = true;
    //displayOff_intensity();
    displayIntensity(0);
    sessionTimer->start(1000);

}

/*
 * Function: updateTimer
 * Input: none
 * Purpose: update the timer count every second (increment by 1) with the help of its sessionTimer's initialization in initTimer.
 *          Cancel the timer when it reaches the confirmed group's corresponding time.
 *
*/
/*
 *Function: updateTimer
 * In: None
 * Out: None
 * Return: None
 * Purpose: Update the timer count every second with the help of its sessionTimer's initialization in initTimer
 *          Cancels the timer when it reaches the confirmed group's correspondiong time. Displays the remaining time
 */
void MainWindow::updateTimer() {
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
    ui->remainingTimeLabel->setText(timeRemaining);
    if(currTimerCount == groupList.at(currSelectedGrp)->getDuration()){
        ui->remainingTimeLabel->setText("Remaining Session Time: 0:0:0");
        qInfo("Session Ending...");
        endSession();
    }
}

/*
 *Function: updateConnectionQuality
 * In: None
 * Out: None
 * Return: None
 * Purpose: Update the connection quality text on the UI
 */
void MainWindow::updateConnectionQuality() {
    QString connectionQuality = "Connection Quality: ";
    if(earClipsConnected){
        if(earsWet){
            connectionQuality = connectionQuality + "Strong Connection";
        }
        else{
            connectionQuality = connectionQuality + "Okay Connection";
        }
    }
    else{
        connectionQuality = connectionQuality + "No Connection";
    }
    ui->connectionQuality->setText(connectionQuality);
}

/*
 *Function: checkConnection
 * In: None
 * Out: None
 * Return: None
 * Purpose: Checks the conenction beween the device and the user;s ears depending on whether or not the ear clips are connected and whether or not the ears are wet
 */
void MainWindow::checkConnection() {
    //displayOff_intensity();
    displayIntensity(0);
    if(earClipsConnected){
        if(earsWet){
            qInfo("Strong Connection.");
            for (int i = 0; i < 3; i++) {
                intensityLevels.at(i)->setStyleSheet("QLabel {background-color: rgb(138, 226, 52);}");
            }
            connectTestTimer->stop();
            beginSessionTimer->start(3000);
        }
        else{
            qInfo("Okay Connection.");
            for (int i = 3; i < 6; i++) {
                intensityLevels.at(i)->setStyleSheet("QLabel {background-color: rgb(252, 233, 79);}");
            }
            connectTestTimer->stop();
            beginSessionTimer->start(3000);
        }
    }
    else {
        qInfo("No Connection");
        for (int i = 6; i < 8; i++) {
            intensityLevels.at(i)->setStyleSheet("QLabel {background-color: rgb(239, 41, 41);}");
        }
        connectTestTimer->start(2000);
    }
}
/*
 *Function: connectEarClips
 * In: None
 * Out: None
 * Return: None
 * Purpose: Connects the earclips and updates the UI
 */
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
    updateConnectionQuality();
}

/*
 *Function: dampenEar
 * In: None
 * Out: None
 * Return: None
 * Purpose: Simulates the dampening of the ear by changing the earsWet variable
 */
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
    updateConnectionQuality();
}

/*
 *Function: displayIntensity
 * In: None
 * Out: None
 * Return: None
 * Purpose: Updates the UI intensity colours as per a supplied int 0=OFF 8=ON
 */
void MainWindow::displayIntensity(int intensity){
    int counter=0;
    for(int i=counter;i<intensity;i++){
        if(i<3){
            intensityLevels.at(i)->setStyleSheet("QLabel {background-color: rgb(138, 226, 52);}");
        }
        else if(i<6){
            intensityLevels.at(i)->setStyleSheet("QLabel {background-color: rgb(252, 233, 79);}");
        }
        else{
            intensityLevels.at(i)->setStyleSheet("QLabel {background-color: rgb(239, 41, 41);}");
        }
        counter++;
    }
    for(int i=counter; i<8;i++){
        intensityLevels.at(i)->setStyleSheet("QLabel {background-color: rgb(255, 255, 255);}");
        counter++;
    }
}

/*
 * Function: previousRecording
 * In: None
 * Out: currRecording
 * Return: None
 * Purpose: Handles the previous recording button being clicked from the UI by decreasing currRecording.
 */
void MainWindow::previousRecording(){
    if(!therapyInProgress){
        qInfo("Received PREVIOUS RECORDING signal.");
        if(currRecording != 0){
            currRecording = currRecording - 1;
        }
        displayRecording(currRecording);
    }
}

/*
 * Function: nextRecording
 * In: None
 * Out: currRecording
 * Return: None
 * Purpose: Handles the next recording button being clicked from the UI by increasing currRecording.
 */
void MainWindow::nextRecording(){
    if(!therapyInProgress){
        qInfo("Received NEXT RECORDING signal.");
        if(currRecording < recordingList.size()){
            currRecording = currRecording + 1;
        }
        displayRecording(currRecording);
    }
}

/*
 * Function: displayRecording
 * In: None
 * Out: None
 * Return: None
 * Purpose: Changes text on UI to display the current recording..
 */
void MainWindow::displayRecording(int r){
    if(r < recordingList.size()){
        QString duration = "Duration: " + QString::number(recordingList.at(r)->getDuration());
        QString type = "Session Type: " + recordingList.at(r)->getType();
        QString intensity = "Intensity: " + QString::number(recordingList.at(r)->getIntensity());

        ui->recordingDuration->setText(duration);
        ui->recordingSession->setText(type);
        ui->recordingIntensity->setText(intensity);
    }
}

/*
 *Function: iniBattery
 * In: None
 * Out: None
 * Return: None
 * Purpose: Initializes the current battery level and displays on UI
 */
void MainWindow::iniBattery(){
    qInfo("displaying battery");
    displayIntensity(0);
    if(batteryLevel > 2){
        intensityLevels.at(batteryLevel-1)->setStyleSheet("QLabel {background-color: rgb(0, 0, 255);}");
    }
    else{
        intensityLevels.at(batteryLevel-1)->setStyleSheet("QLabel {background-color: rgb(239, 41, 41);}");
    }
    QTimer::singleShot(1000, [=](){
        if(therapyInProgress){
            displayIntensity(currentIntensity);
        }
        else if(powerStatus){
            displayIntensity(8);
        }
    });
}

/*
 *Function: updateBattery
 * In: None
 * Out: None
 * Return: None
 * Purpose: Updates the UI and current battery level based on length of therapy, intensity, and connection to skin
 */
void MainWindow::updateBattery(){
    //do for loop to check for current battery level and blink it
    //power on and power off stops/begins battery consumation,

    if(powerStatus == true){
       if(therapyInProgress == true){
           batteryLevel = (batteryLevel - (currentIntensity * 0.1) - 0.01);
           if (batteryLevel < 0){
               batteryLevel = 0;
               togglePower();
               return;
           }
       }
       else{
           batteryLevel = (batteryLevel - 0.01);
           if (batteryLevel < 0){
               batteryLevel = 0;
               togglePower();
               return;
           }
       }
    }
    qDebug() << "Idle: " << batteryLevel ;
    Database->updatePower(batteryLevel);
}

void MainWindow::chargeBattery(){
    batteryLevel = 8;
}

