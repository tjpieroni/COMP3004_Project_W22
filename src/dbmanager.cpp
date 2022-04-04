#include "dbmanager.h"

const QString dbManager::DB_PATH = "/database/oasis.db";
/*
 * Function: dbManager
 * In: None
 * Out: None
 * Return: None
 * Purpose: creates a new QSql database
 */
dbManager::dbManager()
{
    oasisDB = QSqlDatabase::addDatabase("QSQLITE");
    oasisDB.setDatabaseName("oasis.db");

    if (!oasisDB.open()) {
        throw "Error: Database could not be opened";
    }

    if (!initDB()) {
        throw "Error: Database could not be initialized";
    }
}

/*
 * Function: initDB
 * In:  None
 * Out: None
 * Return: a boolean
 * Purpose: Initializes the database with recordings table and returns whether or not the operation was successful
 */
bool dbManager::initDB(){
    oasisDB.transaction();

    QSqlQuery query;

    query.exec("CREATE TABLE IF NOT EXISTS recordings (recID INTEGER NOT NULL PRIMARY KEY, intLevel INTEGER NOT NULL, duration INTEGER NOT NULL, type TEXT NOT NULL);");

    return oasisDB.commit();
}

/*
 * Function: retrieveRecordings
 * In: None
 * Out: None
 * Return: A QVector containing recording pointers
 * Purpose: To retrieve recordings from the database
 */
QVector<recording*> dbManager::retrieveRecordings(){
    oasisDB.transaction();
    QSqlQuery query;
    query.exec("SELECT * FROM recordings;");
    while(query.next()){
        recording* newRecording = new recording(query.value(0).toInt(),query.value(1).toInt(),query.value(2).toInt(),query.value(3).toString());
        recordings.push_back(newRecording);
    }
    return recordings;
}

/*
 * Function: insertRecording
 * In: a recording pointer
 * Out: None
 * Return: a boolean value
 * Purpose: Inserts a new recording to the database and returns whether or not the operation was successful
 */
bool dbManager::insertRecording(recording *newRecording){
    oasisDB.transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO recordings (recID, intLevel, duration, type) VALUES (:recID, :intLevel, :duration, :type);");
    query.bindValue(":recID", newRecording->getID());
    query.bindValue(":intLevel", newRecording->getIntensity());
    query.bindValue(":duration", newRecording->getDuration());
    query.bindValue(":type", newRecording->getType());
    query.exec();

    return oasisDB.commit();
}
