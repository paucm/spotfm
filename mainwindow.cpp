#include "mainwindow.h"

MainWindow::MainWindow(QWidget *widget, Qt::WFlags fl)
  : QMainWindow(widget, fl)
  , Ui::MainWindow()

{
    setupUi(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::slotPlay()
{
}

void MainWindow::slotStop()
{
}

void MainWindow::slotPause()
{
}

void MainWindow::slotSkip()
{
}
