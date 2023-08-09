import serial

def create_new_file(filename):
    with open(filename, 'w') as file:
        pass

def main():
    serial_port = "/dev/ttyUSB1"  # Replace this with the appropriate port for your system
    baud_rate = 115200

    try:
        ser = serial.Serial(serial_port, baud_rate)
        print(f"Connected to {serial_port} at {baud_rate} baud.")

        current_file = None

        while True:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            print("Received:", line)

            if line.startswith("Message_"):
                message_num = line.split("_")[1]

                if current_file:
                    current_file.close()

                filename = f"Message_{message_num}.txt"
                create_new_file(filename)
                current_file = open(filename, 'a')

            elif current_file:
                current_file.write(line + "\n")

    except serial.SerialException as e:
        print(f"Error: {e}")
    finally:
        if current_file:
            current_file.close()
        ser.close()

if __name__ == "__main__":
    main()

