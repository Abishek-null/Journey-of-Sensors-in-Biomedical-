🤏 Flex Sensor Interfacing with ESP32
📌 Project Overview

This project demonstrates interfacing a flex sensor with an ESP32 microcontroller to detect bending through changes in electrical resistance.

A voltage-divider circuit converts the resistance variation into a measurable voltage. The ESP32 reads the analog signal and can use the readings to monitor changes in bending.

This project is part of my Journey of Sensors in Biomedical repository, where I document my sensor experiments, code, and practical learning in electronics and biomedical instrumentation.

🎯 Objectives
Understand the working principle of a flex sensor.
Interface a resistive sensor with an ESP32.
Read analog voltage using the ESP32 ADC.
Observe changes in sensor readings as the sensor bends.
Explore sensor calibration and data analysis.
Investigate potential applications in biomedical instrumentation and motion monitoring.
🧰 Components Required
ESP32 development board
Flex sensor
Fixed resistor, such as 10 kΩ
Breadboard
Jumper wires
USB cable

The appropriate resistor value depends on the flex sensor's resistance range and the desired measurement sensitivity.

🔌 ESP32 Pin Connections
Component	Connection
Flex sensor terminal 1	3.3V
Flex sensor terminal 2	ADC measurement node
Fixed resistor	ADC node to GND
ADC measurement node	GPIO 34 on a classic ESP32
Ground	ESP32 GND

Note: The wiring above is an example. Verify the ADC pin availability for your specific ESP32 board.

⚙️ Working Principle
The flex sensor changes resistance when bent.
The sensor and fixed resistor form a voltage-divider circuit.
The voltage at the divider output changes as the sensor bends.
The ESP32 ADC reads the output voltage.
The program records or displays the readings.
Calibration can be used to relate the readings to specific bending positions or angles.

The relationship between sensor voltage and bending angle is not necessarily linear and depends on the sensor and mechanical setup.

💻 Source Code

The source code for reading flex sensor values is stored in this folder.

Suggested filename: flex_sensor.ino

The code should use the ADC pin and any calibration values corresponding to the actual circuit.

🧪 Testing and Calibration
Record the sensor reading when it is straight.
Bend it gradually within its recommended mechanical limits.
Record the readings at different positions.
Repeat the measurements to check consistency.
Develop a calibration relationship if estimating bending angle is required.

Avoid sharp folds, excessive force, or bending beyond the sensor's specified limits.

📚 Learning Outcomes
Resistive sensor interfacing
Voltage-divider circuits
ESP32 analog-to-digital conversion
Sensor calibration and data acquisition
Motion and bending measurement concepts
Biomedical instrumentation applications
🏥 Potential Applications

Flex sensors can be explored in experimental systems for finger movement monitoring, hand gesture recognition, rehabilitation research, and wearable motion-sensing prototypes.

These are potential applications, not validated clinical functions.

🚧 Project Status

Status: Learning and development project.

Source code, circuit diagrams, hardware photographs, and test results can be updated as the project progresses.

⚠️ Disclaimer

This is an educational project and is not a certified medical device. It should not be used for diagnosis or clinical decisions without appropriate validation.

🤝 Contributions

Suggestions, feedback, and improvements are welcome.

Learn • Experiment • Build • Share 🌱
