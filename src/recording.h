#ifndef RECORDING_H
#define RECORDING_H

#include <QString>

class recording
{
public:
    recording();

private:
    int intensityLevel, duration;
    QString sessionType;
};

#endif // RECORDING_H
