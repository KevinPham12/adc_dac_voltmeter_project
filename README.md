# ADC and DAC Voltmeter Project

This project demonstrates the use of ADC (Analog-to-Digital Converter) and DAC (Digital-to-Analog Converter) interfaces with the K60 microcontroller. The objective of the lab is to measure an analog voltage signal using the ADC and then output that signal using the DAC. The project also includes a voltmeter design that measures DC voltages between 0V and 3.3V and displays the result on a 7-segment display.

## Objectives
1. Understand and utilize ADC and DAC modules in the K60 microcontroller.
2. Implement a voltmeter design that measures DC voltages.
3. Output the measured signal using the DAC and observe the results on an oscilloscope.
4. Integrate ADC and DAC configurations and timing with PIT (Periodic Interrupt Timer) and PDB (Programmable Delay Block).

## Hardware Requirements
- **TWR-K60D100M** board (Kinetis K60 microcontroller).
- **Function Generator** for generating input signals.
- **Oscilloscope** for visualizing the ADC and DAC outputs.
- **7-Segment Display** for showing the voltage measurement.
- **Breadboard** and additional components like resistors and transistors for circuit setup.

## Software Requirements
- **Keil uVision 5** IDE for writing and debugging the embedded software.
- **PuTTY** terminal for SSH communications (if needed).

## Software Overview
The software for this project is written in C and ARM Assembly. The main components of the code include:
- **PIT Timer**: Used for timing control and cycling through digits on the 7-segment display.
- **PDB**: Used to trigger the ADC conversion.
- **ADC Configuration**: Used to sample the input voltage and convert it to a digital value.
- **DAC Configuration**: Used to output the digital value as an analog signal.
- **7-Segment Display**: Displays the measured voltage.

### Main Functions
- **PIT_Config()**: Configures the PIT timer for interrupt generation.
- **PIT0_IRQHandler()**: Handles the PIT timer interrupts to cycle through the 7-segment display.
- **ADC_Config()**: Configures the ADC for input voltage measurement.
- **ADC0_IRQHandler()**: Handles the ADC conversion and displays the result on the 7-segment display.
- **DAC Configuration**: Sends the ADC value to the DAC for analog signal output.

## Project Setup

### Step 1: Hardware Setup
1. Connect the **TWR-K60D100M** board to your computer.
2. Hook up the **7-segment display** to the K60 board via GPIO pins.
3. Connect the **function generator** to the ADC input on the board.
4. Attach the **oscilloscope** to monitor the DAC output.

### Step 2: Software Setup
1. Open **Keil uVision 5**.
2. Import the project files or create a new project.
3. Write the code for ADC, DAC, and display handling.
4. Build and flash the project to the K60 board.

### Step 3: Testing
1. Use the **function generator** to supply different DC voltage values to the ADC input.
2. Observe the output on the **7-segment display** for the voltage measurement.
3. Use the **oscilloscope** to verify the DAC output signal.

## Results and Observations
- The ADC successfully measures input voltages ranging from 0V to 3.3V.
- The DAC accurately reproduces the input signal, with a maximum frequency output of around 6-7 kHz.
- The measured voltage is displayed correctly on the 7-segment display.

## Conclusion
This project successfully demonstrates the use of the ADC and DAC in the K60 microcontroller. The voltmeter design was completed, and the input signal was accurately measured and displayed. The DAC was able to output the corresponding analog signal, and the results were validated using an oscilloscope.

## Appendix
For the full code, refer to `main.c` for configuration, handling, and interrupt routines for the ADC and DAC modules.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
