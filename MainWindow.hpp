#pragma once

#include <QMainWindow>

class QPushButton;
class QLabel;
class QGridLayout;

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

	QPushButton* mStartStopButton = nullptr;
	QLabel* mSecondsLeftLabel = nullptr;
	QGridLayout* mScreensGrid = nullptr;
};
