#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->TwentySession->setEnabled(false);
    ui->FortyFiveSession->setEnabled(false);
    ui->UserDesignatedSession->setEnabled(false);

    ui->TwentySession->setStyleSheet("QPushButton {border: 3px solid red}");
}

MainWindow::~MainWindow()
{
    delete ui;
}
