# Dcar
Dcar Controller Project. It receive command from bluetooth then send it to uart.

## Prerequisite
- Target: ESP32-WROOM
- Framework: ESPIDF version 4.4.2
- Environment: Ubuntu

## Install
- ESPIDF: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html

## Clone 

    git clone https://github.com/thodoxuan392/dcar

## Build
*Export ESPIDF to Project Workspace*

    cd dcar
    . ~/esp/esp-idf/export.sh


*Build Project*

    idf.py build

## Flash Image

    idf.py flash -p /dev/ttyUSB0

## Monitor Device

    idf.py monitor -p /dev/ttyUSB0
