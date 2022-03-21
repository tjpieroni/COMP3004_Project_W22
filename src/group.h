#ifndef GROUP_H
#define GROUP_H

#include "session.h"
#include <QString>
#include <QVector>

class Group
{
public:
    Group();

private:
    int duration;
    QString groupName;
    QVector<*session> sessions;
};

#endif // GROUP_H
