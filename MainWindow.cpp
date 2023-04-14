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
#include <QScrollArea>
#include <QBuffer>
#include <QPainter>
#include <QTimer>
#include <QThread>


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

	mTimer = new QTimer(this);
	mTimer->setInterval(1000);
	connect(mTimer, &QTimer::timeout, this, &MainWindow::onTimerTimeout);
}

MainWindow::~MainWindow()
{
	if(mScreenshotManagerThread != nullptr)
	{
		mScreenshotManagerThread->quit();
		mScreenshotManagerThread->wait();
	}
}

void MainWindow::onStartStopButtonClicked()
{
	if(!mTimer->isActive())
	{
		mTimer->start();
		mStartStopButton->setText("Stop");
	}
	else
	{
		mTimer->stop();
		mStartStopButton->setText("Start");
	}
}

void MainWindow::onTimerTimeout()
{
	--mSecondsLeft;
	if(mSecondsLeft == 0)
	{
		mSecondsLeft = Interval;

		mScreenshotManagerThread = new QThread(this);
		mScreenshotManager = new ScreenshotManager();
		mScreenshotManager->moveToThread(mScreenshotManagerThread);

		connect(mScreenshotManagerThread, &QThread::started, this, [this](){
			auto screenShot = mScreenshotManager->takeScreenShot(windowHandle());

			int percent = 100;
			if(!mLastScreen.isNull())
			{
				percent = mScreenshotManager->comparePixelByPixel(mLastScreen, screenShot);
			}
			mLastScreen = screenShot;

			addScreenToGrid(screenShot, percent);
			mDBManager->addScreenshot(ScreenshotData(screenShot.cacheKey(), percent, pixmapToByteArray(screenShot)));

			emit screeshotAdded();;
		});
		connect(this, &MainWindow::screeshotAdded, mScreenshotManagerThread, &QThread::quit);
		connect(mScreenshotManagerThread, &QThread::finished, mScreenshotManager, &ScreenshotManager::deleteLater);

		mScreenshotManagerThread->start();
	}

	mSecondsLeftLabel->setText(QString("%1:%2").arg(mSecondsLeft / 60, 2 , 10, QLatin1Char('0'))
							   .arg(mSecondsLeft % 60, 2 , 10, QLatin1Char('0')));
}

void MainWindow::initLayout()
{
	mStartStopButton = new QPushButton("Start", this);

	mSecondsLeftLabel = new QLabel(this);
	mSecondsLeftLabel->setText(QString("%1:%2").arg(mSecondsLeft / 60, 2 , 10, QLatin1Char('0'))
							   .arg(mSecondsLeft % 60, 2 , 10, QLatin1Char('0')));

	auto hlayout = new QHBoxLayout();
	hlayout->addWidget(mStartStopButton);
	hlayout->addWidget(mSecondsLeftLabel);
	hlayout->addStretch();

	mScreensGrid = new QGridLayout();
	auto previewsWidget = new QWidget(this);
	previewsWidget->setLayout(mScreensGrid);
	auto scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(previewsWidget);

	auto vlayout = new QVBoxLayout();
	vlayout->addLayout(hlayout);
	vlayout->addWidget(scrollArea);

	auto centralWidget = new QWidget(this);
	centralWidget->setLayout(vlayout);

	setCentralWidget(centralWidget);
	setMinimumWidth(680);
	setMinimumHeight(360);

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
	const int w = scaledPixmap.width();
	const int h = scaledPixmap.height();
	const int r = 30;
	painter.drawEllipse(w - (r+1), h - (r+1), r, r);

	painter.save();
	painter.setPen(QPen(Qt::black, 5));
	auto f = painter.font();
	f.setPointSize(6);
	f.setBold(true);
	f.setStyleStrategy(QFont::PreferAntialias);
	painter.setFont(f);

	QString text = QString::number(percent) + "%";
	auto textSize = painter.fontMetrics().size(0, text);

	int x = w - r + (r - textSize.width()) / 2;
	int y = h - r + (r - textSize.height()) / 2;
	painter.drawText(QRect(x,y,r,r), text);
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

