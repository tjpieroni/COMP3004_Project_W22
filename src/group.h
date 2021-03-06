#ifndef GROUP_H
#define GROUP_H

#include "session.h"
#include <QString>
#include <QVector>
#include <QPushButton>

class Group
{
public:
    Group(QPushButton*, QString, int);
    QPushButton* getBtnWidget();
    QString getName();
    int getDuration();

private:
    int duration;
    QString groupName;
    //added this to be able to change the stylesheet
    QPushButton* btn;
};

#endif // GROUP_H
