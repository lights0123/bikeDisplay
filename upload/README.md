#What this is
Previously, before [platformio/platform-atmelsam#20](https://github.com/platformio/platform-atmelsam/issues/20) was fixed, uploading to
an Arduino M0 with the Arduino.org bootloader would fail because avrdude
would attempt to validate fuses like it would with an AVR device. This is a nodejs
script to upload to this board with automatic port detection.
