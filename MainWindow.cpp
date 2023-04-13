#include "MainWindow.hpp"
#include "Screenshot/ScreenshotData.hpp"
#include "Screenshot/ScreenshotManager.hpp"
#include "Database/DBManager.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QBuffer>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	initLayout();

	mDBManager = new DBManager(this);
	mDBManager->connect();

	auto firstScreen = mDBManager->getAllScreenshots().first();
	addScreenToGrid(byteArrayToPixmap(firstScreen.rawData));
}

MainWindow::~MainWindow()
{
}

void MainWindow::onStartStopButtonClicked()
{
	ScreenshotManager scr;
	auto screenShot = scr.takeScreenShot(windowHandle());

	addScreenToGrid(screenShot);

	mDBManager->addScreenshot(ScreenshotData(111, 100, pixmapToByteArray(screenShot)));

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
	vlayout->addStretch();

	auto centralWidget = new QWidget(this);
	centralWidget->setLayout(vlayout);

	setCentralWidget(centralWidget);
	setMinimumHeight(300);
	setMinimumWidth(300);

	connect(mStartStopButton, &QPushButton::clicked, this, &MainWindow::onStartStopButtonClicked);
}

void MainWindow::addScreenToGrid(const QPixmap &pixmap)
{
	auto screenLabel = new QLabel(this);
	screenLabel->setPixmap(pixmap.scaled(120, 90, Qt::KeepAspectRatio));

	mScreensGrid->addWidget(screenLabel, 0, 0);
}

QByteArray MainWindow::pixmapToByteArray(const QPixmap &pixmap)
{
	QByteArray array;
	QBuffer buffer(&array);
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "PNG");
	return array;
}

QPixmap MainWindow::byteArrayToPixmap(const QByteArray &byteArray)
{
	QPixmap pixmap;
	pixmap.loadFromData(byteArray, "PNG");
	return pixmap;
}

