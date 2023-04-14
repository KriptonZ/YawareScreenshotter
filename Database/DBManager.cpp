#include "DBManager.hpp"

#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

DBManager::DBManager(QObject *parent)
	: QObject(parent), mMutex(new QMutex())
{
	mDb = QSqlDatabase::addDatabase("QSQLITE", "screens");
	mDb.setConnectOptions("QSQLITE_ENABLE_REGEXP");
	mDb.setDatabaseName("screens.db");
}

bool DBManager::connect()
{
	if(!mDb.open())
	{
		qDebug() << "Can't open database: " << mDb.lastError().databaseText();
		return false;
	}

	QSqlQuery query(mDb);

	query.exec("PRAGMA foreign_keys=ON");

	if(mDb.tables().isEmpty()) //first run
	{
		QString queryText = "CREATE TABLE `screenshots_data` ("
						"`id`			INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
						"`hash`			INTEGER NOT NULL,"
						"`similarity`	INTEGER NOT NULL,"
						"`data`			BLOB NOT NULL"
						");";
		if(!query.exec(queryText))
		{
			qDebug() << "Can't create the table: " << query.lastError().text();
		}
	}

	return true;
}

bool DBManager::addScreenshot(const ScreenshotData &data) const
{
	QMutexLocker locker(mMutex);

	QSqlQuery query(mDb);
	query.prepare("INSERT INTO screenshots_data(hash, similarity, data) "
				  "VALUES(:hash, :similarity, :data)");
	query.bindValue(":hash", data.hash);
	query.bindValue(":similarity", data.similarity);
	query.bindValue(":data", data.rawData);

	if (!query.exec())
	{
		qDebug() << "Can't add screenshot data: " << query.lastError().text();
		return false;
	}

	return true;
}

QVector<ScreenshotData> DBManager::getAllScreenshots() const
{
	QSqlQuery query(mDb);
	query.prepare("SELECT hash, similarity, data FROM screenshots_data");
	query.setForwardOnly(true);

	if(!query.exec())
	{
		return QVector<ScreenshotData>();
	}

	QVector<ScreenshotData> screens;
	while(query.next())
	{
		auto hash = query.value(0).toInt();
		auto similarity = query.value(1).toInt();
		auto imageArray = query.value(2).toByteArray();

		screens.append(ScreenshotData(hash, similarity, imageArray));
	}

	return screens;
}
