#pragma once

#include "Screenshot/ScreenshotData.hpp"
#include <QObject>
#include <QSqlDatabase>

class DBManager : public QObject
{
	Q_OBJECT
public:
	explicit DBManager(QObject *parent = nullptr);

	bool connect();

	bool addScreenshot(const ScreenshotData& data) const;
	QVector<ScreenshotData> getAllScreenshots() const;

signals:

private:
	QSqlDatabase mDb;
};

