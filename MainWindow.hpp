#pragma once

#include <QMainWindow>

class DBManager;
class QPushButton;
class QLabel;
class QGridLayout;
class QPixmap;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void onStartStopButtonClicked();

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
	QPixmap mLastScreen;

	DBManager* mDBManager = nullptr;
};
