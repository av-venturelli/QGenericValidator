#include "mainwindowpanel.h"
#include "ui_mainwindowpanel.h"

MainWindowPanel::MainWindowPanel(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowPanel)
{
    ui->setupUi(this);
}

MainWindowPanel::~MainWindowPanel()
{
    delete ui;
}
