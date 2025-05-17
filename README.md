# BreezeIQ
SmartBreeze is a behavioral learning fan system that adapts to your personal comfort in real-time. Using ESP32, DHT22 sensors, and Python-based behavioral learning , it analyzes temperature, humidity, and user preferences to auto-adjust airflow—no manual tweaking needed!

# Working 
The DHT22 sensor continuously measures the surrounding temperature and sends this data to the ESP32. When a user enters the room, they press their assigned push button (e.g., User 1 presses Button 1).
The ESP32 then communicates the temperature and user ID to a Python model over Wi-Fi. This model predicts the preferred fan speed for that user at the given temperature. 
Finally, the ESP32 adjusts the fan speed based on the model’s prediction, providing a personalized comfort experience.

# Future improvements 
1)) Replace push buttons with facial recognition to identify users automatically and eliminate manual input.
2)) Run the Python prediction model directly on the ESP32 using frameworks like MicroPython or TensorFlow Lite for on-device AI, allowing the fan to operate independently without needing a laptop.
3)) Add Wi-Fi control via a mobile app for remote monitoring and manual overrides.
4)) Integrate voice control using Alexa, Google Assistant, or custom commands.

#wokwi simulation 
used led as output (as wokwi does not have a motor driver and motor.)
