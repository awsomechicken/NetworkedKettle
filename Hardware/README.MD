# Hardware
This folder will contain any hardware pertaining to the Project, including formware.

## KettleController
This folder has the hardware needed to build my prototype of the kettle, more specifics to come.
- 3D models (STLs and CAD files)
- Board Designs (KiCad)
- Controller Firmware (Arduino Framework, Initial revision)

For Connections see the [Schematic](./KettleController/Controller_Schematic.pdf).

## To Do
- [x] Publish the Project
- [ ] Migrate firmware to ESP-IDF
- [ ] Eliminate _The Ring of Power_.


## Bill Of Materials

| Part   | Use | Qty | Origin |
|--------|-----|-----|--------|
| 1.7L Farberware Electronically controlled Kettle | Vessel | 1 | Purchase|
| ESP32 WEMOS Dev Kit            | Controller | 1 | Purchase |
| 128x32 SSD1306 I2C OLED Module | Display | 1 | Purchase |
| Touch Sensor | Physical User input | 1 | Purchase |
| [Ring of Power](./KettleController/The_Ring_of_Power) | Supplemental power input | 1 | Fabricate |
| 3mmx1mm Pogo Pin | Supplemental Power input | 2 | Purchase |
| [Controller Housings](./KettleController/3D_Models) | House and Mount components | Varies | Fabricate |
