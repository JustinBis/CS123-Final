#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Setup the GL format
    QGLFormat qglFormat;
    qglFormat.setVersion(3, 2);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    QGLFormat::setDefaultFormat(qglFormat);
    qglFormat.setSampleBuffers(true);

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

