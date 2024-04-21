# Temperature-Control-and-Energy-Monitoring-System
This repo focuses on presenting the results of a Project  aimed at creating a thermally controlled environment in  a small, insulated box. The temperature of the box is set  remotely using a website and the internal temperature is  raised or lowered until the desired temperature is  obtained
# OEL Temperature Control System

## A. Idea

The design philosophy behind our approach to the OEL (Online Environment Logger) was to provide users with the ability to set and control temperature parameters remotely through a web interface accessible from any device with an internet connection. To achieve this versatility, we employed an ESP32 Microcontroller as the central processing unit. This microcontroller facilitates temperature control operations and communicates with various components of the system.

We utilize a Piezo-electric cooler with reversible polarity to regulate the temperature within the environment. Additionally, cooling fans are integrated to assist in maintaining optimal conditions. The ESP32 is connected to four relays, enabling independent control over the polarity of the piezo-electric coolers and the operation of cooling fans.

## B. Applications

The design of the OEL Temperature Control System is suitable for various applications, including:

1. **Pharmaceutical Industry**: Precise temperature control is essential in pharmaceutical manufacturing processes.
   
2. **Air Conditioners**: The system's functionality aligns with the requirements of air conditioning units used in residential, commercial, and industrial settings.

3. **Food Industry**: Maintaining specific temperature levels is crucial for preserving food quality and safety in food processing and storage facilities.

4. **Automotive Industry**: Temperature control systems find applications in vehicles to regulate cabin temperature and ensure passenger comfort.

## C. Parts Used

1. **ESP32 Microcontroller**: An Arduino-based Wi-Fi microcontroller serves as the central processing unit for the system, managing relay operations and sensor data analysis.

2. **SSD1306 0.96in OLED Display**: A compact display with a resolution of 128x64 pixels, compatible with the I2C communication protocol, providing visual feedback.

3. **TEC1-12706 Piezo-electric Cooler**: Utilized for both heating and cooling purposes, employing the Peltier effect to regulate temperature by switching polarity.

4. **Heatsinks**: Enhance heat dissipation from the Peltier modules, optimizing their efficiency and performance.

5. **Cooling Fans**: Two 30mm fans facilitate air circulation within the environment, aiding in heating through convection.

6. **4-Relay Module**: Controls the polarity of Peltier modules and operation of cooling fans independently.

7. **CJMCU-75 Temperature Sensor**: Offers temperature readings in degrees Celsius via I2C communication.

8. **ACS712 Current Sensor**: Provides analog readings interpreted by the ESP32, facilitating current monitoring.

9. **12V Li-ion Battery**: Selected for its high current output to power the circuit effectively, maximizing the performance of Peltier modules.

10. **Voltage Regulator**: Converts the 12V battery output to 5V for powering the ESP32 and sensors efficiently.


# Proteus Simulation
![image](https://github.com/Alihassan7212/Temperature-Control-and-Energy-Monitoring-System/assets/93111418/f97b2776-7205-4dd2-ad67-596ca224c36d)

# What if

Okay so we are receiving the data on cloud. we can use that information to feed the RL agent and then RL angent will predict the next action (On/off or set temperature) to the controller the benefit is that we can store the ocupancy and sleeping patterns already in the cloud plus the weather data is already cnin==oming from an online source so we already have all the information stored in the clouad we just need current temperature, humidy ot any other thing we want to measure to control the temperature intelliently.

