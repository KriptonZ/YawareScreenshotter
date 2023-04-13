#pragma once

#include <QObject>

class QWindow;
class QPixmap;

class ScreenshotManager : public QObject
{
	Q_OBJECT
public:
	explicit ScreenshotManager(QObject *parent = nullptr);

	QPixmap takeScreenShot(const QWindow* winHandle) const;

	int comparePixelByPixel(const QPixmap& prev, const QPixmap& curr) const; // Result - similarity percent

signals:

};

