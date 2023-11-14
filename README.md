<img src="https://cdn.theiremi.fr/images/slgreen/banner.png" />

![Arduino IDE](https://img.shields.io/badge/Arduino_IDE-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![Espressif](https://img.shields.io/badge/espressif-E7352C?style=for-the-badge&logo=espressif&logoColor=white)


SLGreen turns your Arduino equipped with a MCP2515 into a Serial to CAN bridge using the SLCan / CAN232 / Lawicel protocol.
This project facilitates usage of CAN bus on your computer by allowing you to make cheap CAN interface with your Arduino and those [chinese CAN boards](https://www.aliexpress.com/w/wholesale-MCP2515-arduino.html).

# Hardware

### Components needed
SLGreen require two compoments to function properly :
- An MCU compatible with Arduino (Any Arduino board, ESP32...)
- An MCP2515 controller board, chosen according with the MCU logic voltage

### Wiring

The wiring of the MCP2515 module is basically the same as any SPI slave device :
- MOSI, MISO, SCK : Check your board documentation
- SS : Pin 10

**The INT pin doesn't need to be connected**

Here is a wiring example for an Arduino Uno :
![Arduino UNO wiring](https://dimitarmk.com/wp-content/uploads/2016/12/wiringarduinomcp2515.png)


# Installation instructions
**To install and use the SLGreen software, follow these steps:**

1. Clone this repository using `git clone` or download it directly as ZIP.
2. Install the dependencies listed below using the Library Manager of the Arduino IDE.
3. Check the configuration options to make sure your SLGreen board will work.
3. Open `SLGreen.ino` with the Arduino IDE and upload it to your Arduino.


### Dependencies
- **MCP_CAN Library**: This project uses the `mcp_can` library by coryjfowler for communication with the MCP2515 CAN controller. It can be found here: [MCP_CAN_lib](https://github.com/coryjfowler/MCP_CAN_lib).


## Configuration
Several configuration options are available at the top of the SLGreen.ino file. Depending on your hardware and your needs, it may be useful to modify them.

**⚠ In any case, you should take a look at the table below to check if the default values are correct for your usage ⚠**

### Parameters Table

| Parameter           | Default Value     | Recommended/Available Values |
|---------------------|-------------------|------------------------------|
| `MCP_CS`            | `10`              | Any value making your hardware work |
| `MCP_FREQUENCY`     | `MCP_8MHZ`        | `MCP_8MHZ`, `MCP_16MHZ`, `MCP_20MHZ` |
| `SERIAL_SPEED`      | `115200`          | `115200`, `150000`, `250000`, `500000`, `1000000`, `2000000` |
| `DEFAULT_CAN_SPEED` | `CAN_500KBPS`     | `CAN_10KBPS`, `CAN_20KBPS`, `CAN_50KBPS`, `CAN_100KBPS`, `CAN_125KBPS`, `CAN_250KBPS`, `CAN_500KBPS`, `CAN_1000KBPS` |

### Details

- **`MCP_CS`**: Define the Chip Select pin of the MCP2515
- **`MCP_FREQUENCY`**: Sets the clock frequency of the MCP2515 CAN controller
- **`SERIAL_SPEED`**: Defines the baud rate for serial communication
- **`DEFAULT_CAN_SPEED`**: Sets the default speed for the CAN bus, in case the computer doesn't choose one


## SLCAN (Lawicel) Implementation
SLGreen implements the SLCAN (Lawicel) protocol to allow interactive communication with the CAN bus via a serial port. To get more details about this protocol, [check this link](https://www.canusb.com/docs/can232_v3.pdf)

### Implementation status
- [x] `S` : Configure CAN speed (speed 7 / 800Kbps not implemented)
- [ ] `s` : Alternative configuration of CAN speed
- [x] `O` : Open CAN bus
- [x] `L` : Open CAN bus in listen-only mode
- [x] `C` : Close CAN bus
- [x] `t` : Send standard CAN frame
- [x] `T` : Send extended CAN frame
- [ ] `r` : Send standard RTR frame
- [ ] `R` : Send extended RTR frame
- [ ] `P` : Retrieve received frame
- [ ] `A` : Retrieve all received frames
- [ ] `F` : Retrieve status flags
- [ ] `X` : Configure if frames are directly send when received (forced ON)
- [ ] `W` : Filter mode setting
- [ ] `M` : Acceptance Code Register
- [ ] `m` : Acceptance Mask Register
- [ ] `U` : Define a new serial baud rate
- [x] `V` : Return software version
- [x] `N` : Return serial number
- [ ] `Z` : Configure if timestamp is included when transmitting received frames (forced OFF)
- [ ] `Q` : Save the configuration for the next startup



## Contribution
**Contributions are welcome !**

A lot of features are yet to be integrated into this project, so any help doing this will be greatly appreciated !

If you wish to contribute, please fork the repository, create a new branch for your changes, and submit a pull request for review.

### License
This project is licensed under the MIT License. For more information, please consult the `LICENSE` file or visit https://opensource.org/licenses/MIT.

### Contact
For any questions or suggestions, please feel free to contact me :
- Website: [theiremi.fr](https://www.theiremi.fr/#contact)
- Email: [contact@theiremi.fr](mailto:contact@theiremi.fr)