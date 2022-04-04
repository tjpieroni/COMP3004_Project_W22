#include "group.h"
/*
 * Function: Group
 * In: A QPushButton pointer, a QString and an integer
 * Out: None
 * Return: None
 * Purpose: A constructor for the Group object
 */
Group::Group(QPushButton *btnParam, QString nameParam, int duration)
{
    this->btn = btnParam;
    this->groupName = nameParam;
    this->duration = duration;
}
/*
 * Function: getBtnWidget
 * In: None
 * Out: None
 * Return: btn, a QPushButton pointer
 * Purpose: Getter function for the group object's btn parameter
 */
QPushButton* Group::getBtnWidget() {return this->btn;}
/*
 * Function: getName
 * In: None
 * Out: None
 * Return: groupName, a QString
 * Purpose: Getter function for the group object's groupName parameter
 */
QString Group::getName() {return this->groupName;}

/*
 * Function: getDuration
 * In: None
 * Out: None
 * Return: duration, an integer
 * Purpose: Getter function for the group object's duration parameter
 */
int Group::getDuration(){return this->duration;}
