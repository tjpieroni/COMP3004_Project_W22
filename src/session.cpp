#include "session.h"
/*
 * Function: session
 * In: A QPushButton pointer and a QString
 * Out: None
 * Return: None
 * Purpose: Constructor for the session object
 */
session::session(QPushButton* btnParam, QString nameParam)
{
    this->btn = btnParam;
    this->name = nameParam;
}
/*
 * Function: getbtnWidget
 * In: None
 * Out: None
 * Return: btn, a QPushButton pointer
 * Purpose: getter function for the session object's btn parameter
 */
QPushButton* session::getBtnWidget() {
    return this->btn;
}

/*
 * Function: getName
 * In: None
 * Out: None
 * Return: name, a QString
 * Purpose: getter function for the session's name parameter
 */
QString session::getName() {
    return this->name;
}
