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
