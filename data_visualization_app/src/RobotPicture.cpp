/**
 * @file RobotPicture.cpp
 * @author Jakub Borsukiewicz & Jan Farbotko
 * @brief Klasa do zarządzania widget'em odpowiedzialnym za wyświetlanie zdjęcia robota MiniSumo
 * @version 1.0
 * @date 2026-03-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "RobotPicture.hpp"
#include "ui_RobotPicture.h"

/**
 * @brief Konstruktor nowego obiektu Robot Picture
 * 
 * @param[in] parent - wskażnik na rodzica obiektu
 */
RobotPicture::RobotPicture(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RobotPicture)
{
    ui->setupUi(this);
}

/**
 * @brief Destruktor obiektu Robot Picture
 * 
 */
RobotPicture::~RobotPicture()
{
    delete ui;
}
