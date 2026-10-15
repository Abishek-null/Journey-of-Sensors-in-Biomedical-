🔋 Battery Voltage Monitoring Using ESP32
📌 Project Overview

This project demonstrates how to measure battery voltage using an ESP32 microcontroller and its built-in analog-to-digital converter (ADC).

A resistor-based voltage divider scales the battery voltage to a suitable range for the ESP32 ADC. The microcontroller reads the analog signal, calculates the estimated input voltage, and displays the result through the Serial Monitor.

This project is part of my Journey of Sensors in Biomedical repository, where I document sensor interfacing, electronics experiments, microcontroller programming, and practical measurement techniques.

🎯 Objectives
Understand voltage measurement using an ESP32 ADC.
Learn how a resistor voltage divider works.
Read and calculate battery voltage.
Explore ADC conversion and calibration.
Compare measured voltage with a multimeter.
Understand basic battery monitoring for electronic systems.
🧰 Components Required
ESP32 development board
Battery or suitable DC voltage source
Two resistors for the voltage divider
Jumper wires
Breadboard
USB cable
Digital multimeter for testing and calibration
🔌 Example Pin Connections

This example uses GPIO 34 on a classic ESP32 development board. Check your board's pinout before wiring.

Component	Connection
Battery positive	R1 input
R1 output	ADC measurement node
R2	ADC node to GND
ADC measurement node	ESP32 GPIO 34
Battery negative	ESP32 GND

Example resistor values:

R1 = 100 kΩ
R2 = 100 kΩ

With equal resistor values, the voltage divider ideally reduces the input voltage by half.

Safety note: Never connect a battery directly to an ESP32 GPIO. Verify the maximum battery voltage, resistor ratings, ADC input limits, and common-ground connection before powering the circuit.

⚙️ Working Principle
The battery provides the voltage being measured.
The resistor voltage divider scales the voltage to a suitable level.
The ESP32 ADC reads the divided analog voltage.
The program estimates the voltage at the ADC input.
The voltage-divider ratio is used to calculate the original battery voltage.
The result is displayed in the Serial Monitor.

For a two-resistor divider:

Vout = Vin × R2 / (R1 + R2)

Therefore:

Vin = Vout × (R1 + R2) / R2

Where:

Vin is the original battery voltage.
Vout is the voltage at the ESP32 ADC pin.
R1 is the resistor between battery positive and the ADC node.
R2 is the resistor between the ADC node and ground.
💻 Source Code

The source code for reading and calculating battery voltage is stored in this project folder.

Suggested filename: battery_voltage.ino

The code should use the actual resistor values, ADC pin, and calibration settings from the implemented circuit.

🧪 Testing and Calibration
Measure the battery voltage using a multimeter.
Read the voltage reported by the ESP32.
Compare the two readings.
Check the resistor values and ADC configuration.
Apply calibration if needed and document the method.

ESP32 ADC readings may have errors due to ADC characteristics, resistor tolerances, wiring, and noise. Calibration can improve accuracy.

📚 Learning Outcomes
ESP32 ADC interfacing
Voltage-divider circuit design
Analog voltage measurement
Embedded C/C++ programming
Measurement calibration
Battery monitoring fundamentals
🚧 Project Status

Status: Learning and development project.

The source code, circuit diagram, testing photographs, and sample measurements can be updated as the project develops.

⚠️ Disclaimer

This is an educational electronics project, not a certified battery protection or battery-management system. It does not independently provide overvoltage protection, undervoltage protection, or battery safety monitoring.

🤝 Contributions

Suggestions, feedback, and improvements are welcome.

Learn • Experiment • Build • Share 🌱
