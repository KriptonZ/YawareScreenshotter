#pragma once

#include <QMainWindow>

class ScreenshotManager;
class DBManager;
class QPushButton;
class QLabel;
class QGridLayout;
class QPixmap;
class QTimer;
class QThread;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

signals:
	void screeshotAdded();

private slots:
	void onStartStopButtonClicked();
	void onTimerTimeout();

private:
	void initLayout();
	void addScreenToGrid(const QPixmap& pixmap, const int percent, bool pushFront = true);
	void pushBackWidgetToGridLayout(QWidget* widget);
	void pushFrontWidgetToGridLayout(QWidget* widget);
	QByteArray pixmapToByteArray(const QPixmap& pixmap);
	QPixmap byteArrayToPixmap(const QByteArray& byteArray);

	QPushButton* mStartStopButton = nullptr;
	QLabel* mSecondsLeftLabel = nullptr;
	QGridLayout* mScreensGrid = nullptr;

	const int ElementsPerRow = 5;
	const int Interval = 60; // seconds
	int mSecondsLeft = Interval;
	QPixmap mLastScreen;

	QTimer* mTimer = nullptr;
	QThread* mScreenshotManagerThread = nullptr;

	ScreenshotManager* mScreenshotManager = nullptr;
	DBManager* mDBManager = nullptr;
};
