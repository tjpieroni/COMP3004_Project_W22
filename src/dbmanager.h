#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "recording.h"

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

private:
    static const QString DB_PATH;
    QSqlDatabase oasisDB;
    QVector<recording*> recordings;

};

#endif // DBMANAGER_H
