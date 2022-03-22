#ifndef SESSION_H
#define SESSION_H

#include <QString>
#include <QPushButton>
class session
{
public:
    session(QPushButton*, QString);
    QPushButton* getBtnWidget();
    QString getName();
private:
    QString name;
    double freqMin, freqMax;
    QPushButton* btn;
};

#endif // SESSION_H
