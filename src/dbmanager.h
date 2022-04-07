#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "recording.h"
#include "group.h"
#include "session.h"

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>
#include <QVariant>

class dbManager
{
public:
    dbManager();
    bool initDB();
    QVector<recording*> retrieveRecordings();
    bool insertRecording(recording*);
    int retrievePower();
    bool updatePower(int);

private:
    static const QString DB_PATH;
    QSqlDatabase oasisDB;
    QVector<recording*> recordings;
    int power;
};

#endif // DBMANAGER_H
