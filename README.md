# bikeDisplay
This is a project to add a smart controller to a standard bicycle, using an Atmel SAMD21 controller
(known as Arduino Zero or Arduino M0). What it does:
* Uses GPS for speed, heading (direction), and simple navigation
* Uses an SSD1306 OLED to display a menu
* Controls front & back lights with both PWM and Neopixel control
* Uses radio (NRF24L01) to communicate with additional controllers

What it is not:
* An anti-theft device (although it might work)
* A controller for motorized bicycles

## Building & Uploading
This project uses [platformio](https://github.com/platformio/platformio-core) for automatic
dependency management, building, and uploading. Follow the instructions outlined
[here](http://docs.platformio.org/en/latest/installation.html) to install PlatformIO. You can
install the full IDE based on Atom, or just install the core version and use an external IDE
(I use CLion). Once platformIO is installed, update `platformio.ini` to use the correct port in
`upload_port = `. Then, run `pio run -t upload`. This will automatically install all dependencies
and upload to the selected board. If you'd like to use another IDE, guides are provided
[here](http://docs.platformio.org/en/latest/ide.html).