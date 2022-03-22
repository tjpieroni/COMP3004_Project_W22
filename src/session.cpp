#include "session.h"

session::session(QPushButton* btnParam, QString nameParam)
{
    this->btn = btnParam;
    this->name = nameParam;
}

QPushButton* session::getBtnWidget() {
    return this->btn;
}

QString session::getName() {
    return this->name;
}
