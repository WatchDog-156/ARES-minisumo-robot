/**
 * @file RobotPicture.cpp
 * @author Jakub Borsukiewicz (borsukiewiczkuba12345@gmail.com)
 * @brief Klasa do zarządzania widget'em odpowiedzialnym za wyświetlanie zdjęcia robota MiniSumo
 * @version 1.0
 * @date 2026-03-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "RobotPicture.h"
#include "ui_RobotPicture.h"

/**
 * @brief Construct a new Robot Picture:: Robot Picture object
 * 
 * @param parent wskażnik na rodzica obiektu
 */
RobotPicture::RobotPicture(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RobotPicture)
{
    ui->setupUi(this);
}

/**
 * @brief Destroy the Robot Picture:: Robot Picture object
 * 
 */
RobotPicture::~RobotPicture()
{
    delete ui;
}
