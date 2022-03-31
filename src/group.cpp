#include "group.h"

Group::Group(QPushButton *btnParam, QString nameParam, int duration)
{
    this->btn = btnParam;
    this->groupName = nameParam;
    this->duration = duration;
}

QPushButton* Group::getBtnWidget() {return this->btn;}

QString Group::getName() {return this->groupName;}

int Group::getDuration(){return this->duration;}
