/**
 * @file BluetoothLogger.cpp
 * @author Jakub Borsukiewicz (borsukiewiczkuba12345@gmail.com)
 * @brief Tworzy oraz zarządza okienkiem "Bluetooth Logger" do wyświetlania wiadomości po łączności bluetooth
 * @version 1.0
 * @date 2026-03-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "BluetoothLogger.h"
#include "ui_BluetoothLogger.h"
#include <QDateTime>           
#include <QListWidgetItem>

/**
 * @brief Construct a new Bluetooth Logger:: Bluetooth Logger object
 * 
 * @param parent wskażnik na rodzica obiektu
 */
BluetoothLogger::BluetoothLogger(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BluetoothLogger)
{
    ui->setupUi(this);

    this->setWindowTitle("Bluetooth Logger");
}

/**
 * @brief Destroy the Bluetooth Logger:: Bluetooth Logger object
 * 
 */
BluetoothLogger::~BluetoothLogger()
{
    delete ui;
}

/**
 * @brief Funkcja odpowiada za resetowanie wyświetlanych logów po naciśnięciu przycisku RESET
 * 
 */
void BluetoothLogger::on_buttonReset_clicked()
{
    ui->listLog->clear();
}

/**
 * @brief Funkcja odpowiada za dodanie logów wysłanych do urządzenia w odpowiednim miejscu w oknie Bluetooth Logger 
 * 
 * @param log wysłana wiadomość do wyświetlenia
 */
void BluetoothLogger::addSendedLog(const QString &log) 
{
    QString time = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    QString label = QString("[%1] LOGGER >> %2").arg(time).arg(log);
    QListWidgetItem *item = new QListWidgetItem(label);
    item->setForeground(QColor(255, 165, 0)); // Orange
    ui->listLog->addItem(item);
    ui->listLog->scrollToBottom();
}

/**
 * @brief Funkcja odpowiada za dodanie logów odebranych od urządzenia w odpowiednim miejscu w oknie Bluetooth Logger 
 * 
 * @param log przysłana wiadomość do wyświetlenia
 */
void BluetoothLogger::addReceivedLog(const QString &log) 
{
    QString time = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    QString label = QString("[%1] LOGGER << %2").arg(time).arg(log);
    QListWidgetItem *item = new QListWidgetItem(label);
    item->setForeground(Qt::darkGreen); // Dark Green
    ui->listLog->addItem(item);
    ui->listLog->scrollToBottom();
}