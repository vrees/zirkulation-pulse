# Wlan Einschalter für die Zirkulationspumpe

Das verwendete Board sendet via WLAN 'Buehl-private' ein HTTP Post Request an Home Assistant. Der zugehöriges EspHome Client, basierednde auf Sonoff Basic schaltet über ein Relais für 2 Sekunden das Zeitrelais in der Zirkulations-Abzweigdose. Die Zirkulation wird dann für dei eingestellte Zeit von 4 Minuten eingeschaltet.



## ESP-01 Anschlüsse
| ESP-01   | Farbe    | Anschluss      |
| -------- | -------- | -------------- |
| GND      | scharz   | Taster Common  |
| VCC      | rot      | LED-Anode      |
| RST      | grün     | Reset-Taster   |
| GPIO2    | gelb     | LED-Kathode    |


