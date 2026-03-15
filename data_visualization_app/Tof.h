#ifndef TOF_H
#define TOF_H

#include <QWidget>

namespace Ui {
class TOF;
}

class TOF : public QWidget
{
    Q_OBJECT

public:
    explicit TOF(QWidget *parent = nullptr);
    ~TOF();

private:
    Ui::TOF *ui;
};

#endif // TOF_H
