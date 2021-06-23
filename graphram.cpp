#include "graphram.h"
#include "ui_graphram.h"

graphRAM::graphRAM(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::graphRAM)
{
    ui->setupUi(this);
}

graphRAM::~graphRAM()
{
    delete ui;
}
