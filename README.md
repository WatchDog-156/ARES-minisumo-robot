# ARES – Autonomiczny Robot Elektroniczny Sumo

Projekt autonomicznego robota minisumo, budowanego w ramach zajęć z przedmiotu Sterowniki Robotów. Robot przeznaczony jest do rywalizacji na arenie zgodnej z regulaminem Robotic Arena.

---

## Spis treści
- [Opis projektu](#opis-projektu)
- [Architektura systemu](#architektura-systemu)
- [Konfiguracja mikrokontrolera](#konfiguracja-mikrokontrolera)
- [Urządzenia zewnętrzne](#urządzenia-zewnętrzne)
- [Projekt elektroniki](#projekt-elektroniki)
- [Konstrukcja mechaniczna](#konstrukcja-mechaniczna)
- [Oprogramowanie](#oprogramowanie)

---

## Opis projektu

Głównym celem projektu jest zbudowanie w pełni autonomicznego robota mobilnego klasy **MiniSumo**, którego podstawowym zadaniem jest wykrycie przeciwnika na arenie, szybkie dotarcie do niego i wypchnięcie poza białą linię ograniczającą ring. Robot jednocześnie musi samodzielnie unikać wyjechania poza obszar walki.

Robot wykorzystuje system czujników:
- **Czujniki białej linii** – do detekcji krawędzi areny
- **Czujniki odległości IR (ToF)** – do lokalizacji przeciwnika
- **System ASS (Anti-Sumo System)** – serwomechanizmy z białymi ekranami, wprowadzające przeciwnika w błąd

---

## Architektura systemu

System składa się z trzech głównych warstw:

1. **Warstwa sprzętowa** – mikrokontroler, czujniki, silniki, sterowniki, zasilanie
2. **Warstwa sterowania** – oprogramowanie na Raspberry Pi Pico
3. **Warstwa komunikacji** – USART (Bluetooth) do debugowania i monitorowania

![Architektura systemu](images/architektura_systemu.png "Architektura systemu")

---

## Konfiguracja mikrokontrolera

W projekcie wykorzystano **Raspberry Pi Pico 2 WH** z układem RP2350 (ARM Cortex M33), który oferuje:
- Dwa rdzenie obliczeniowe
- Wbudowany moduł WiFi/Bluetooth (CYW43439)
- Wystarczającą liczbę pinów GPIO
- Niskie zużycie energii

### Konfiguracja pinówdotarcie

| Pin | Tryb pracy        | Funkcja                                     |
|-----|-------------------|---------------------------------------------|
| 1   | USART0 TX         | Wysyłanie danych Bluetooth                  |
| 2   | USART0 RX         | Odbieranie danych Bluetooth                 |
| 10  | GPIO_Output       | Silnik lewy – kierunek "do przodu"          |
| 11  | GPIO_Output       | Silnik lewy – kierunek "do tyłu"            |
| 12  | PWM               | Silnik lewy – sygnał PWM                    |
| 14  | PWM               | Silnik prawy – sygnał PWM                   |
| 15  | GPIO_Output       | Silnik prawy – kierunek "do tyłu"           |
| 16  | GPIO_Output       | Silnik prawy – kierunek "do przodu"         |
| 19  | GPIO_Input        | Start walki (przycisk/modul startowy)       |
| 20  | GPIO_Input        | Kill (koniec walki)                         |
| 21  | GPIO_Output       | XSHUT 1 – włącznik czujnika odległości 1    |
| 22  | GPIO_Output       | XSHUT 2 – włącznik czujnika odległości 2    |
| 24  | GPIO_Output       | XSHUT 3 – włącznik czujnika odległości 3    |
| 25  | GPIO_Output       | XSHUT 4 – włącznik czujnika odległości 4    |
| 26  | I2C0 SDA          | Dane z czujników odległości IR              |
| 27  | I2C0 SCL          | Zegar czujników odległości IR               |
| 31  | ADC_IN            | Prawy czujnik linii (analogowy)             |
| 32  | ADC_IN            | Lewy czujnik linii (analogowy)              |

![Raspberry Pi Pico 2 WH pinout](images/pinout.png "Raspberry Pi Pico 2 WH pinout")

### Konfiguracja peryferiów

#### USART (Bluetooth)
- Baud Rate: 115200
- Word Length: 8 bitów
- Parity: Brak
- Stop Bits: 1

#### I2C (czujniki VL53L0X)
- Tryb: Standard Mode
- Częstotliwość: 100 kHz
- Adresowanie: 7-bitowe

#### ADC (czujniki linii)
- Rozdzielczość: 12 bitów
- Tryb: Single Conversion
- Częstotliwość próbkowania: 96 cykli

---

## Urządzenia zewnętrzne

### 3.1 Czujniki białej linii – KTIRO711S
- **Zasilanie:** 5V
- **Wyjście:** analogowe
- **Rodzaj:** odbiciowy fototranzystorowy
- **Rozdzielczość:** 1 mm
- **Zastosowanie:** detekcja białej linii areny

### 3.2 Czujniki odległości IR – VL53L0X
- **Zasilanie:** 3.3 – 5V
- **Zakres pomiarowy:** 2 – 200 cm
- **Częstotliwość odczytu:** do 50 Hz
- **Interfejs:** I2C
- **Kąt widzenia:** 25° stożkowy
- **Ilość:** 4 szt. (2 przednie, 2 skośne)

### 3.3 Sterownik silników – TB6612FNG
- **Napięcie silników:** do 15V
- **Napięcie logiczne:** 2.7 – 5.5V
- **Prąd ciągły:** 1.2A na kanał
- **Prąd szczytowy:** 3.2A
- **Tryby:** CW, CCW, hamulec, stop

### 3.4 Silniki DC – N20-BT37 210:1 140RPM
- **Napięcie:** 3 – 9V
- **Przekładnia:** 210:1
- **Prędkość:** 140 RPM
- **Moment obrotowy:** 3.6 kg·cm

### 3.5 Serwomechanizmy – MG90S Mikro (ASS)
- **Napięcie:** 4.8V
- **Kąt obrotu:** 180°
- **Moment:** 2 kg·cm
- **Zastosowanie:** system wprowadzania w błąd przeciwnika

### 3.6 Moduł startowy MiniSumo
- **Funkcja:** detekcja startu walki
- **Zapobiega falstartom**
- **Wymagany na zawodach**

![Moduł startowy](images/mod_startowy.png "Moduł startowy")

---

## Projekt elektroniki

Zaprojektowano dedykowaną płytkę PCB w formie shielda dla Raspberry Pi Pico, która integruje wszystkie komponenty:

### Główne cechy projektu:
- Zasilanie z baterii LiPo 7.4V
- Stabilizator napięcia 5V dla logiki i czujników
- Filtracja napięć i zabezpieczenia przeciwprzepięciowe
- Wyprowadzenia dla wszystkich czujników i silników
- Łatwy montaż na robocie

### Schematy i projekty:

![Schemat blokowy PCB](images/Schemat_block.jpg "Schemat blokowy PCB")

![Schemat elektroniczny](images/Schematics.jpg "Schemat elektroniczny")

![Projekt PCB – warstwa górna](images/PCB_front.jpg "Projekt PCB – warstwa górna")

![Projekt PCB – warstwa dolna](images/PCB_bottom.jpg "Projekt PCB – warstwa dolna")

### Montaż i lutowanie:

![PCB po lutowaniu – przód](images/Front.jpg "PCB po lutowaniu – przód")

![PCB po lutowaniu – tył](images/Bottom.jpg "PCB po lutowaniu – tył")

---

## Konstrukcja mechaniczna

### Projekt CAD
Konstrukcja zaprojektowana w **Autodesk Inventor**, składa się z:
- Głównej obudowy (korpus)
- Pokrywy górnej
- Dwóch części plugów
- Kół z felgami 3D i oponami silikonowymi

![Model 3D robota](images/mechanika.png "Model 3D robota")

### Produkcja
- Wydruk 3D z filamentu PET-G
- Wstawki mosiężne do mocowania
- Śruby metalowe M3 oraz M2.5
- Opony z silikonu formierskiego

![Gotowy robot](images/robot.jpeg "Gotowy robot")

---

## Oprogramowanie

### Diagram przepływu programu

![Diagram przepływu](images/flowchart.png "Diagram przepływu")

### Główne moduły programu:
1. **Inicjalizacja** – konfiguracja peryferiów, czujników, silników
2. **Pętla główna** – odczyt czujników, podejmowanie decyzji
3. **Algorytm walki** – strategia ataku i obrony
4. **System ASS** – sterowanie serwomechanizmami
5. **Komunikacja** – wysyłanie danych diagnostycznych przez Bluetooth

### Przykładowy fragment kodu:

```c
void main() {
    init_sensors();
    init_motors();
    init_servos();
    
    while(1) {
        read_line_sensors();
        read_tof_sensors();
        
        if (opponent_detected()) {
            attack_strategy();
        } else {
            search_strategy();
        }
        
        avoid_line();
        update_ass_system();
    }
}
```

---

**Autorzy:** Jakub BORSUKIEWICZ, Jan FARBOTKO