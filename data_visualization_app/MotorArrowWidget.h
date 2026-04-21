#ifndef MOTORARROWWIDGET_H
#define MOTORARROWWIDGET_H

#include <QWidget>

/**
 * @file MotorArrowWidget.h
 * @brief Widget rysujący strzałkę kierunku i siły silnika
 */
class MotorArrowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MotorArrowWidget(QWidget *parent = nullptr);

    /**
     * @param speed prędkość -100..+100
     *              >0 = do przodu (strzałka w górę)
     *              <0 = do tyłu  (strzałka w dół)
     *               0 = brak strzałki
     */
    void setSpeed(int speed);

    QSize sizeHint() const override { return QSize(40, 70); }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_speed = 0;
};

#endif // MOTORARROWWIDGET_H