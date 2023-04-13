#pragma once

#include <QByteArray>

struct ScreenshotData
{
	int hash = 0;
	int similarity = 100;
	QByteArray rawData;

	ScreenshotData() = default;
	ScreenshotData(int hash, int similarity, QByteArray rawData) :
		hash(hash), similarity(similarity), rawData(rawData)
	{}
};
