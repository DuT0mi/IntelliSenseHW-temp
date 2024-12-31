# IntelliSense Assessment

>> WiFi Channel Quality Scanner

This document provides an overview of the solution for the IntelliSense assessment. The solution involves creating a WiFi channel quality scanner using the ESP32c6 microcontroller and the ESP-IDF framework.

## Environment

- **Hardware**: ESP32c6
- **Host OS**: Ubuntu LTS 22.04
- **Framework**: ESP-IDF
- **Programming Languages**: C (std=c99), Python
- **Tested On**: Linux

## Configuration

### ESP-Side Setup

Follow these steps to configure and deploy the solution:

1. **Clone the Repository**:
   Navigate to the `esp-side` folder after cloning the repository.
   ```bash
   git clone <repository-url>
   cd esp-side
   ```

2. **Set Up the ESP-IDF Environment**:
   Initialize the ESP-IDF environment.
   ```bash
   . $HOME/esp/esp-idf/export.sh
   ```

3. **Set the Target**:
   Configure the build target to match the hardware (e.g., ESP32c6). This step generates the `build` folder and the `sdkconfig` file.
   ```bash
   idf.py set-target esp32c6
   ```

4. **Connect the Hardware**:
   Connect the ESP32c6 hardware to your host system and identify the serial port. For example:
   ```bash
   sudo dmesg | grep tty
   ```
<img src="https://github.com/DuT0mi/IntelliSenseHW-temp/blob/main/img/hw_setup.jpeg" />
5. **Build and Flash**:
   Build the project, set up mandatory parameters, and flash the firmware. Replace the port (e.g. `/dev/ttyACM0`) with the detected one if different.
   ```bash
   idf.py -p /path/to/port -b 115200 build flash monitor
   ```
   If successful, you should see output similar to the following in the terminal:
   <img src="https://github.com/DuT0mi/IntelliSenseHW-temp/blob/main/img/esp-idf-serial.png" />

6. **Alternate Monitoring Tools**:
   After closing the monitor tool provided by ESP-IDF (ctrl+x &rarr; ctrl+t &rarr; ctrl+x), you can use alternate tools like `minicom` for monitoring. Refer to [this guide](https://wiki.emacinc.com/wiki/Getting_Started_With_Minicom) for setup instructions.
6. **Alternate Monitoring Tools**:
   After closing the monitor tool provided by ESP-IDF, you can use alternate tools like `minicom` for monitoring. Refer to [this guide](https://wiki.emacinc.com/wiki/Getting_Started_With_Minicom) for setup instructions.

### Python-Based "Serial Monitor"

To use a Python-based serial monitor, follow these steps:

1. **Install Required Packages**:
   Ensure that the following Python packages are installed:
   ```bash
   pip install pyserial argparse
   ```

2. **Connect the Hardware**:
   Connect the ESP32c6 hardware to your host system and identify the serial port as described earlier.

3. **Close Active Sessions**:
   Ensure that no other sessions are using the serial port (e.g., `/dev/ttyACMX`). Close any such sessions if necessary.

4. **Run the Python Program**:
   Use the provided Python script to monitor the serial output. Replace the used port (e.g. `/dev/ttyACMX`) with your actual port and set the baud rate to match the ESP configuration.
   ```bash
   python3 main.py -p /path/to/port -b <baud rate>
   ```

5. **Monitor Output**:
   The console output should be identical (after you reset the uC if it has been running and finsihed its single job) to what you would see using the ESP-IDF monitor (or similar if the ambient dependencies has changed) tool or other serial monitoring tools like `minicom`.
   <img src="https://github.com/DuT0mi/IntelliSenseHW-temp/blob/main/img/python-serial.png"/>
   
## Literature Used

The following resources were referenced during development:

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/index.html)
- [PyParsing Documentation](https://pyparsing-docs.readthedocs.io/en/latest/)
