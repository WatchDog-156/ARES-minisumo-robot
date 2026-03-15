#include "RobotPicture.h"
#include "ui_RobotPicture.h"

RobotPicture::RobotPicture(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RobotPicture)
{
    ui->setupUi(this);
}

RobotPicture::~RobotPicture()
{
    delete ui;
}
