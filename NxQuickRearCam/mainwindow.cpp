#include <QGraphicsDropShadowEffect>
#include <QGraphicsColorizeEffect>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	//	Application Name Style
	ui->overlayLabel->setStyleSheet("QLabel {"
									"color : rgba(1,  1, 1, 128);"
//									"outline-color : black;"
//									"outline-offset : black;"
									"}");
#if 0
//	QFont font = ui->overlayLabel->font();
//	font.setBold(true);
//	ui->overlayLabel->setFont(font);
//	QGraphicsColorizeEffect *ce = new QGraphicsColorizeEffect();
//	ce->setEnabled(true);
//	ce->setColor(QColor(0,0,0,128));
//	ui->overlayLabel->setGraphicsEffect(ce);
#endif
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::GetGraphicViewRect(QRect &rect)
{
	rect = ui->dspGraphicsView->geometry();
}


void MainWindow::setHideEvent( bool isHide )
{
	if( isHide )
		hide();
	else
		show();
}
