This repository contains the code for a custom lighting control circuit for a van conversion. The system allows for dimming, changing colors to different preset values, and consumes no power when off.

**Key Features:**
- Double tap brigtness up or down bring lights full on or full dim.
- Press and hold brighness up/down for fine tune dimming adjustment
- Single tap color change button to toggle between white light and red light.
- Double tap color button to toggle color fade mode (rainbow effect like a hot tub).
- Brightness of all three color modes saved independently to eeprom and restored when system powered on.
- No power consumption when off, very low controller power when running.

*Note:* if you're copying the circuit, be sure to pick pins with similar pwm timers to control the LED power transistors. The Pro Mini has three different clocks, two pwm pins per clock. Only two of these clocks can run at the same frequency, so if you pick the wrong pins, you'll get a flicker when LEDs aren't at full power.

The software runs on an Arduino Pro Mini. Here's a schematic of the circuit:
![Schematic](https://github.com/lekjos/VanLED/blob/fade/schematic.jpeg)
