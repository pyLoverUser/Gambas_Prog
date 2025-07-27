Beleg Linux:

sudo apt-get install mosquitto-clients 
Library "/lib/libwiringPi"



Regner:

Public RegnerCach[20, 3] As Integer     'x,0=Regner Nr., x,1 Beregnungsdauer, x,2=Einschaltverzögerung

TimerRegner_Timer

RegnerEin

OutByte[RegnerCach[i, 0], 0] = 1
RegnerRollo(0, clock0)

RegnerAus

OutByte[j, 0] = 0
RegnerRollo(0, clock0)

Row = stunde * 4 + minute / 15

hight_nr = Länge der Buchung in Anzahl 15 min (4 beim 60 min)
Platz_idx = PlatzNr * 100 + row beim start
