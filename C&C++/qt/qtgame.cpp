#include "qtgame.h"
#include "./ui_qtgame.h"

qtgame::qtgame(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::qtgame)
{
    ui->setupUi(this);
}

qtgame::~qtgame()
{
    delete ui;
}

