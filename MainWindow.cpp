#include "MainWindow.hpp"
#include "Screenshot/ScreenshotManager.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	initLayout();
}

MainWindow::~MainWindow()
{
}

void MainWindow::onStartStopButtonClicked()
{
	ScreenshotManager scr;
	auto screenShot = scr.takeScreenShot(windowHandle());

	auto screenLabel = new QLabel(this);
	screenLabel->setPixmap(screenShot.scaled(160, 90, Qt::KeepAspectRatio));

	mScreensGrid->addWidget(screenLabel, 0, 0);

	mStartStopButton->setText(mStartStopButton->text() == "Start" ? "Stop" : "Start");
}

void MainWindow::initLayout()
{
	mStartStopButton = new QPushButton("Start", this);

	mSecondsLeftLabel = new QLabel("01:00", this);

	auto hlayout = new QHBoxLayout();
	hlayout->addWidget(mStartStopButton);
	hlayout->addWidget(mSecondsLeftLabel);

	mScreensGrid = new QGridLayout();
	auto vlayout = new QVBoxLayout();
	vlayout->addLayout(hlayout);
	vlayout->addLayout(mScreensGrid);

	auto centralWidget = new QWidget(this);
	centralWidget->setLayout(vlayout);

	setCentralWidget(centralWidget);
	setMinimumHeight(300);
	setMinimumWidth(300);

	connect(mStartStopButton, &QPushButton::clicked, this, &MainWindow::onStartStopButtonClicked);
}

