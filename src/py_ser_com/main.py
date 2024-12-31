import serial
import argparse

def parse_arguments():
    parser = argparse.ArgumentParser(description="Connect to ESP32 via serial and print the output.")
    parser.add_argument('-d', '--device', type=str, required=True, help="Serial port (e.g., /dev/ttyACM0 or COMx)")
    parser.add_argument('-b', '--baudrate', type=int, default=115200, help="Baud rate for the connection (default: 115200)")
    return parser.parse_args()

def connect_to_esp32(port, baud_rate):
    try:
        ser = serial.Serial(port, baud_rate, timeout=1)

        if ser.is_open:
            ser.close()

        ser.setDTR(False)
        ser.setRTS(False)

        ser.open()

        print(ser.readline().decode('utf-8', errors='ignore').strip())

        while True:
            data = ser.readline()
            if data:
                try:
                    decoded_data = data.decode('utf-8').strip()
                    if decoded_data:
                        print(decoded_data)
                except UnicodeDecodeError:
                    print(f"Raw bytes: {data}")
            else:
                # Skip empty lines
                continue

    except serial.SerialException as e:
        print(f"Error connecting to the serial port: {e}")
    except KeyboardInterrupt:
        print("Program interrupted. Closing the connection.")
    finally:
        if ser.is_open:
            ser.close()
            print(f"Connection to {port} closed.")

def main():
    args = parse_arguments()
    connect_to_esp32(args.device, args.baudrate)

if __name__ == "__main__":
    main()
