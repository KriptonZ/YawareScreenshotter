#include "ScreenshotManager.hpp"
#include <QScreen>
#include <QGuiApplication>
#include <QWindow>

ScreenshotManager::ScreenshotManager(QObject *parent)
	: QObject(parent)
{

}

QPixmap ScreenshotManager::takeScreenShot(const QWindow *winHandle) const
{
	auto screen = QGuiApplication::primaryScreen();
	if(winHandle)
	{
		screen = winHandle->screen();
	}

	return screen->grabWindow(0);
}

int ScreenshotManager::comparePixelByPixel(const QPixmap &prev, const QPixmap &curr) const
{
	auto prevImage = prev.toImage();
	auto currImage = curr.toImage();
	int unequalPixels = 0;
	for (int y = 0; y < prevImage.height(); ++y)
	{
		QRgb *prevLine = reinterpret_cast<QRgb*>(prevImage.scanLine(y));
		QRgb *currLine = reinterpret_cast<QRgb*>(currImage.scanLine(y));
		for (int x = 0; x < prevImage.width(); ++x)
		{
			if(prevLine[x] != currLine[x])
			{
				++unequalPixels;
			}
		}
	}
	double percent = (1.0 - (double)unequalPixels / (prevImage.height() * prevImage.width())) * 100.0;
	return (int)percent;
}
