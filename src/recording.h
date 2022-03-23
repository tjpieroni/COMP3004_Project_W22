#ifndef RECORDING_H
#define RECORDING_H

#include <QString>

class recording
{
public:
    recording(int, int, int, QString);

    int getID();
    int getIntensity();
    int getDuration();
    QString getType();

private:
    int recordingID, intensityLevel, duration;
    QString sessionType;
};

#endif // RECORDING_H
