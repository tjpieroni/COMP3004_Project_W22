#include "recording.h"

recording::recording(int recordingID, int intensityLevel, int duration, QString sessionType){
    this->recordingID = recordingID;
    this->intensityLevel = intensityLevel;
    this->duration = duration;
    this->sessionType = sessionType;
}

int recording::getID(){return this->recordingID;}
int recording::getIntensity(){return this->intensityLevel;}
int recording::getDuration(){return this->duration;}
QString recording::getType(){return this->sessionType;}
