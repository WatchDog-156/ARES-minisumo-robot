#ifndef ROTATEDLABEL_HPP
#define ROTATEDLABEL_HPP

#include <QWidget>
#include <QString>

class RotatedLabel : public QWidget
{
    Q_OBJECT

public:
    explicit RotatedLabel(QWidget *parent = nullptr);

    void setText(const QString &text);
    void setAngle(double deg);

    QString text()  const { return m_text; }
    double  angle() const { return m_angle; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_text;
    double  m_angle = 0.0;
};

#endif // ROTATEDLABEL_HPP