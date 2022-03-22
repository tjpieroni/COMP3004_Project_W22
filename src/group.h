#ifndef GROUP_H
#define GROUP_H

#include "session.h"
#include <QString>
#include <QVector>
#include <QPushButton>

class Group
{
public:
    Group(QPushButton*, QString);
    QPushButton* getBtnWidget();
    QString getName();
private:
    int duration;
    QString groupName;
    //added this to be able to change the stylesheet
    QPushButton* btn;
};

#endif // GROUP_H
