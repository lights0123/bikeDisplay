# bikeDisplay
This is a project to add a smart controller to a standard bicycle, using an Atmel SAMD21 controller
(known as Arduino Zero or Arduino M0). What it does:
* Uses GPS for speed, heading (direction), and simple navigation
* Uses an OLED display to display a menu
* Controls front & back lights with both PWM and Neopixel control
* Uses radio (NRF24L01) to communicate with additional controllers

What it is not:
* An anti-theft device (although it might work as one)
* A controller for motorized bicycles

## Building & Uploading
This project uses [platformio](https://github.com/platformio/platformio-core) for automatic
dependency management, building, and uploading. Follow the instructions outlined
[here](http://docs.platformio.org/en/latest/installation.html) to install PlatformIO. You can
install the full IDE based on VS Code or Atom, or just install the core version and use an external IDE.
Once platformIO is installed, update `platformio.ini` to use the correct port in
`upload_port = `. Then, run `pio run -t upload`. This will automatically install all
dependencies and upload to the selected board. If you'd like to use another IDE, guides are
provided [here](http://docs.platformio.org/en/latest/ide.html).

## Libraries Used
| Name                                                                               | License                                                                       | Used for                                                                                                           |
|------------------------------------------------------------------------------------|-------------------|--------------------------------------------------------------------------------------------------------------------|
| [Adafruit NeoPixel ZeroDMA](https://github.com/adafruit/Adafruit_NeoPixel_ZeroDMA) | MIT               | Driving WS2812B (Neopixel) LED strips                                                                              |
| [Adafruit ZeroDMA](https://github.com/adafruit/Adafruit_ZeroDMA)                   | MIT               | Dependency to Adafruit NeoPixel ZeroDMA                                                                            |
| [Adafruit Neopixel](https://github.com/adafruit/Adafruit_NeoPixel)                 | LGPLv3            | Dependency to Adafruit NeoPixel ZeroDMA                                                                            |
| [FastLED](https://github.com/FastLED/FastLED)                                      | MIT               | Math for driving LED strips, and to replace the above three when using other LED chips (e.g. APA102, using SPI)    |
| [NeoGPS](https://github.com/SlashDevin/NeoGPS)                                     | GPLv3             | Parsing GPS data. Used instead of TinyGPS because it is smaller, faster, more customizable, and better documented. |
| [U8g2](https://github.com/olikraus/u8g2)                                           | 2-clause BSD      | Driving monochrome graphic displays                                                                                |
| [sunset](https://github.com/buelowp/sunset)                                        | MIT               | Calculating sunrise and sundown times for automatic lights (TODO: implement)                                       |
| [Timezone](https://github.com/JChristensen/Timezone)                               | GPLv3             | Calculating time offsets and daylight savings time                                                                 |
| [Time](https://github.com/PaulStoffregen/Time)                                     | LGPLv2.1 or later | Keeping track of current time                                                                                      |

`Time` and `Timezone` should be removed first, as they pollute the global namespace.