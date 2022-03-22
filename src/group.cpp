#include "group.h"

Group::Group(QPushButton *btnParam, QString nameParam)
{
    this->btn = btnParam;
    this->groupName = nameParam;
}

QPushButton* Group::getBtnWidget() {
    return this->btn;
}

QString Group::getName() {
    return this->groupName;
}
