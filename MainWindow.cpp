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

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	initLayout();

	mDBManager = new DBManager(this);
	mDBManager->connect();

	auto firstScreen = mDBManager->getAllScreenshots().first();
	addScreenToGrid(byteArrayToPixmap(firstScreen.rawData), 100);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onStartStopButtonClicked()
{
	ScreenshotManager scr;
	auto screenShot = scr.takeScreenShot(windowHandle());

	auto lastScreen = mDBManager->getAllScreenshots().last();

	auto percent = scr.comparePixelByPixel(byteArrayToPixmap(lastScreen.rawData), screenShot);

	addScreenToGrid(screenShot, percent);

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

void MainWindow::addScreenToGrid(const QPixmap &pixmap, const int percent)
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

