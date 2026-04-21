#ifndef ROBOTDIAGRAM_H
#define ROBOTDIAGRAM_H

#include <QWidget>
#include <QLabel>
#include "TofConeWidget.h"
#include "MotorArrowWidget.h"

namespace Ui {
class RobotDiagram;
}

/**
 * @file RobotDiagram.h
 * @author Jakub Borsukiewicz
 * @brief Widget widoku robota MiniSumo — stożki ToF, czujniki linii, silniki
 * @version 2.0
 * @date 2026-03-29
 */
class RobotDiagram : public QWidget
{
    Q_OBJECT

public:
    explicit RobotDiagram(QWidget *parent = nullptr);
    ~RobotDiagram();

    /**
     * @brief Aktualizuje wszystkie dane widgetu
     * @param tof1    odległość czujnika ToF1 lewy skrajny [mm]
     * @param tof2    odległość czujnika ToF2 lewy środkowy [mm]
     * @param tof3    odległość czujnika ToF3 prawy środkowy [mm]
     * @param tof4    odległość czujnika ToF4 prawy skrajny [mm]
     * @param motorL  prędkość silnika lewego [-100..100]
     * @param motorR  prędkość silnika prawego [-100..100]
     * @param lineL   wartość czujnika linii lewego [0..4095]
     * @param lineR   wartość czujnika linii prawego [0..4095]
     */
    void updateData(int tof1, int tof2, int tof3, int tof4,
                    int motorL, int motorR,
                    int lineL, int lineR);

private:
    Ui::RobotDiagram *ui;

    // Stożki ToF (promoted widgets w Designer)
    TofConeWidget    *m_cone1 = nullptr;   // TOF1 lewy skrajny  (kąt -45°)
    TofConeWidget    *m_cone2 = nullptr;   // TOF2 lewy środkowy (kąt -15°)
    TofConeWidget    *m_cone3 = nullptr;   // TOF3 prawy środkowy(kąt +15°)
    TofConeWidget    *m_cone4 = nullptr;   // TOF4 prawy skrajny (kąt +45°)

    // Strzałki silników (promoted widgets w Designer)
    MotorArrowWidget *m_arrowL = nullptr;
    MotorArrowWidget *m_arrowR = nullptr;

    /**
     * @brief Aktualizuje kolor i tekst etykiety czujnika linii
     * @param label  wskaźnik na QLabel
     * @param value  wartość ADC [0..4095]; niska = wykrywa linię
     */
    void updateLineLabel(QLabel *label, int value);
};

#endif // ROBOTDIAGRAM_H