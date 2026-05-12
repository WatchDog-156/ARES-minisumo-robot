#ifndef ROTATEDLABEL_H
#define ROTATEDLABEL_H

#include <QWidget>
#include <QPainter>

class RotatedLabel : public QWidget
{
    Q_OBJECT
 
public:
    explicit RotatedLabel(QWidget *parent = nullptr) : QWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setFixedSize(100, 100);
    }
 
    void setText(const QString &text) { m_text = text; update(); }
    void setAngle(double deg)         { m_angle = deg;  update(); }
 
    QString text()  const { return m_text;  }
    double  angle() const { return m_angle; }
 
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
 
        p.translate(width() / 2.0, height() / 2.0);
        p.rotate(m_angle);
 
        QRect r(-38, -13, 76, 26);
 
        p.setBrush(Qt::white);
        p.setPen(QPen(QColor(85, 85, 204), 2.0));
        p.drawRoundedRect(r, 5, 5);
 
        p.setPen(Qt::black);
        QFont f;
        f.setPointSize(8);
        p.setFont(f);
        p.drawText(r, Qt::AlignCenter, m_text);
    }
 
private:
    QString m_text;
    double  m_angle = 0.0;
};


#endif // ROTATEDLABEL_H