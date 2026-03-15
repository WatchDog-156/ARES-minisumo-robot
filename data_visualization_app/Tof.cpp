#include "Tof.h"
#include "ui_Tof.h"

TOF::TOF(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TOF)
{
    ui->setupUi(this);
}

TOF::~TOF()
{
    delete ui;
}
