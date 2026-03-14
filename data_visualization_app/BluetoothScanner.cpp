#include "BluetoothScanner.h"
#include "ui_BluetoothScanner.h"

BluetoothScanner::BluetoothScanner(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BluetoothScanner)
{
    ui->setupUi(this);
}

BluetoothScanner::~BluetoothScanner()
{
    delete ui;
}
