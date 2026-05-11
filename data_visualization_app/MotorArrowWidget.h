#ifndef MOTORARROWWIDGET_H
#define MOTORARROWWIDGET_H

#include <QWidget>

class MotorArrowWidget : public QWidget{
    Q_OBJECT
public:
    explicit MotorArrowWidget(QWidget *parent = nullptr);
    void setSpeed(int speed);
    QSize sizeHint() const override{
        return QSize(40, 60);
    }
protected:
    void paintEvent(QPaintEvent *) override;
private:
    int m_speed = 0;
};

#endif