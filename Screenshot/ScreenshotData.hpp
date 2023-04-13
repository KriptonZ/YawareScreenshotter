#pragma once

#include <QByteArray>

struct ScreenshotData
{
	qint64 hash = 0;
	int similarity = 100;
	QByteArray rawData;

	ScreenshotData() = default;
	ScreenshotData(qint64 hash, int similarity, QByteArray rawData) :
		hash(hash), similarity(similarity), rawData(rawData)
	{}
};
