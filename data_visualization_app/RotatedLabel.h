#ifndef ROTATEDLABEL_H
#define ROTATEDLABEL_H

#include <QLabel>
#include <QPainter>


class RotatedLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(double angle READ angle WRITE setAngle)

public:
    explicit RotatedLabel(QWidget *parent = nullptr)
        : QLabel(parent) {}

    double angle() const { return m_angle; }
    void setAngle(double deg) { m_angle = deg; update(); }

    QSize sizeHint() const override
    {
        QSize s = QLabel::sizeHint();
        if (qAbs(m_angle) > 5.0)
            return QSize(s.height(), s.width());
        return s;
    }

    QSize minimumSizeHint() const override
    {
        return sizeHint();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.translate(width() / 2.0, height() / 2.0);
        p.rotate(m_angle);
        p.translate(-width() / 2.0, -height() / 2.0);

        p.setBrush(QColor(255, 255, 255));
        p.setPen(QPen(QColor(85, 85, 204), 2.0));
        p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 5, 5);

        // Tekst
        p.setPen(Qt::black);
        p.setFont(font());
        p.drawText(rect(), Qt::AlignCenter, text());
    }

private:
    double m_angle = 0.0;
};

#endif // ROTATEDLABEL_H