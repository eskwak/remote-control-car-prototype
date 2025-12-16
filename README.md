# ESP32 Semi-Autonomous Vehicle
The semi-autonomous vehicle navigates through obstacles using readings from ultrasonic sensors. The vehicle can be manually controlled using 
the web-based controller in */Vehicle Controller* but this was mainly developed for movement testing prior to integrating the ultrasonic sensors The final product is expected to be fully autonomous without support for manual control.

**Note: This project is currently in development. The final goal is to create a Roomba-style cleaning device.**

## Hardware 
| Component | Description |
| :--- | :--- |
| **ESP32** | Basically any ESP32 will work. I am using a WROOM |
| **DC Motors** | For movement |
| **TB6612FNG Motor Driver** | For motor control |
| **HR-S04 Ultrasonic Sensors** | For obstacle detection and vehicle navigation |
| **Chassis** | 3-D printed chassis using Bambu Studio and Onshape for design |

## Software & Toolchain
* The embedded software written in */Vehicle Firmware* was written entirely in C++ using PlatformIO on VSCode.
* The web-based controller consists of HTML, CSS, and JavaScript. *This was created for movement testing.*

### Build Environment
* **Compiler:** `[GCC xtensa-esp32-elf-g++]`
* **Build System:** `[Scons]` *(auto managed by PlatformIO)*
* **IDE:** `[VSCode, PlatformIO Extension]`