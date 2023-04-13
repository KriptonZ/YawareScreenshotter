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
#include <QPainter>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	initLayout();

	mDBManager = new DBManager(this);
	mDBManager->connect();

	auto allScreens = mDBManager->getAllScreenshots();
	for(int i = allScreens.count() - 1; i >= 0; i--)
	{
		addScreenToGrid(byteArrayToPixmap(allScreens[i].rawData), allScreens[i].similarity, false);
	}
	if(!allScreens.isEmpty())
	{
		mLastScreen = byteArrayToPixmap(allScreens.last().rawData);
	}
}

MainWindow::~MainWindow()
{
}

void MainWindow::onStartStopButtonClicked()
{
	ScreenshotManager scr;
	auto screenShot = scr.takeScreenShot(windowHandle());

	int percent = 100;
	if(!mLastScreen.isNull())
	{
		percent = scr.comparePixelByPixel(mLastScreen, screenShot);
	}
	mLastScreen = screenShot;

	addScreenToGrid(screenShot, percent);
	mDBManager->addScreenshot(ScreenshotData(screenShot.cacheKey(), percent, pixmapToByteArray(screenShot)));

	mStartStopButton->setText(mStartStopButton->text() == "Start" ? "Stop" : "Start");
}

void MainWindow::initLayout()
{
	mStartStopButton = new QPushButton("Start", this);

	mSecondsLeftLabel = new QLabel("01:00", this);

	auto hlayout = new QHBoxLayout();
	hlayout->addWidget(mStartStopButton);
	hlayout->addWidget(mSecondsLeftLabel);
	hlayout->addStretch();

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

void MainWindow::addScreenToGrid(const QPixmap &pixmap, const int percent, bool pushFront)
{
	auto screenLabel = new QLabel(this);
	auto scaledPixmap = pixmap.scaled(120, 90, Qt::KeepAspectRatio);
	QPainter painter(&scaledPixmap);

	painter.setBrush(Qt::white);
	QPen pen;
	pen.setColor(Qt::black);
	painter.setPen(pen);
	int w = scaledPixmap.width();
	int h = scaledPixmap.height();
	painter.drawEllipse(w - 31, h - 31, 30, 30);

	painter.save();
	painter.setPen(QPen(Qt::black, 5));
	auto f = painter.font();
	f.setPointSize(6);
	f.setBold(true);
	f.setStyleStrategy(QFont::PreferAntialias);
	painter.setFont(f);

	QString text = QString::number(percent) + "%";
	auto textSize = painter.fontMetrics().size(0, text);

	int x = w - 30 + (30 - textSize.width()) / 2;
	int y = h - 30 + (30 - textSize.height()) / 2;
	painter.drawText(QRect(x,y,30,30), text);
	painter.restore();

	screenLabel->setPixmap(scaledPixmap);

	if(pushFront)
	{
		pushFrontWidgetToGridLayout(screenLabel);
	}
	else
	{
		pushBackWidgetToGridLayout(screenLabel);
	}
}

void MainWindow::pushBackWidgetToGridLayout(QWidget *widget)
{
	int r = mScreensGrid->count() / ElementsPerRow;
	int c = mScreensGrid->count() % ElementsPerRow;
	mScreensGrid->addWidget(widget, r, c);
}

void MainWindow::pushFrontWidgetToGridLayout(QWidget *widget)
{
	for(int i = mScreensGrid->count() - 1; i >= 0; i--)
	{
		int r = i / ElementsPerRow;
		int c = i % ElementsPerRow;
		auto currItem = mScreensGrid->itemAtPosition(r, c);
		r = (i+1) / ElementsPerRow;
		c = (i+1) % ElementsPerRow;
		mScreensGrid->addWidget(currItem->widget(), r, c);
	}

	mScreensGrid->addWidget(widget, 0, 0);
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

