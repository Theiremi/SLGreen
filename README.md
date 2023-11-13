# SLGreen

SLGreen turns your Arduino equipped with a MCP2515 into a Serial to CAN bridge using the SLCan / CAN232 / Lawicel protocol.
This project facilitates usage of CAN bus on your computer by allowing you to make cheap CAN interface with your Arduino and those [chinese CAN boards](https://www.aliexpress.com/w/wholesale-MCP2515-arduino.html).

## Dependencies
- **MCP_CAN Library**: This project uses the `mcp_can` library by coryjfowler for communication with the MCP2515 CAN controller. It can be found here: [MCP_CAN_lib](https://github.com/coryjfowler/MCP_CAN_lib).

## Hardware Installation
To set up the hardware for SLGreen, follow these steps:

1. **Required Hardware**: You will need an Arduino and an MCP2515 CAN Bus module.
2. **Hardware Setup**: Connect the MCP2515 CAN Bus module to your Arduino as per the technical documentation.

## Software Installation
To install and use the SLGreen software, follow these steps:

1. **Code Download**: Clone this repository using `git clone` or download it directly from the GitHub page.
2. **Dependencies Installation**: Install the dependencies using the Library Manager of the Arduino IDE.
3. **Script Deployment**: Open `SLGreen.ino` with the Arduino IDE and upload it to your Arduino.



## Configuration
Several configuration options are available on top of the SLGreen.ino file.
Depending on your hardware and your needs, it may be useful to modify them.
**In any case, you should take a look at the table below to check if the default values are correct for your usage.**

### Parameters Table

| Parameter           | Default Value     | Recommended/Available Values                                      |
|---------------------|-------------------|-------------------------------------------------------------------|
| `SERIAL_SPEED`      | `115200`          | `115200`, `150000`, `250000`, `500000`, `1000000`, `2000000`      |
| `MCP_FREQUENCY`     | `MCP_8MHZ`        | `MCP_8MHZ`, `MCP_16MHZ`, `MCP_20MHZ`                              |
| `DEFAULT_CAN_SPEED` | `CAN_500KBPS`     | `CAN_10KBPS`, `CAN_20KBPS`, `CAN_50KBPS`, `CAN_100KBPS`, `CAN_125KBPS`, `CAN_250KBPS`, `CAN_500KBPS`, `CAN_1000KBPS` |

### Details

- **`SERIAL_SPEED`**: Defines the baud rate for serial communication. Adjust this value based on your project requirements and the capabilities of your hardware.
- **`MCP_FREQUENCY`**: Sets the clock frequency of the MCP2515 CAN controller. Choose the frequency that matches your MCP2515 module. Using a frequency different from your module's specification may lead to communication issues.
- **`DEFAULT_CAN_SPEED`**: Sets the default speed for the CAN bus. This value is used if the computer doesn't choose a CAN speed. Select the speed that best suits your network environment and the specifications of your CAN devices.


## SLCAN (Lawicel) Implementation
SLGreen implements the SLCAN (Lawicel) protocol to allow interactive communication with the CAN bus via a serial port. Here are some key details of the implementation:
- Implemented commands: `O` (open CAN bus), `L` (close CAN bus), `C` (configure CAN bus), `V` (software version), `N` (software name), and `S` (configure CAN bus speed).
- Commands for sending CAN frames: `t` and `T` are available for sending standard and extended frames, respectively. The commands for receiving CAN frames `r` and `R` are not implemented but are open for implementation by the community.
- CAN frames are continuously sent; the `X` command for automatic mode control is not active.
- Commands `F`, `W`, `M`, `m`, `U`, `Q` for various frame management and filtering functionalities are not implemented.

For more details on the SLCAN (Lawicel) implementation and protocol specifications, please refer to the documentation provided in the repository.

## Usage
After uploading the script to your Arduino, you can interact with the CAN bus using the provided functions. For specific usage examples, please refer to the comments in the code.

## Contribution
Contributions are welcome. If you wish to contribute, please fork the repository, create a new branch for your changes, and submit a pull request for review.

## License
This project is licensed under the MIT License. For more information, please consult the `LICENSE` file or visit https://opensource.org/licenses/MIT.

## Contact
For any questions or suggestions, please feel free to contact Theirémi:
- Website: [theiremi.fr](https://www.theiremi.fr/#contact)
- Email: [contact@theiremi.fr](mailto:contact@theiremi.fr)