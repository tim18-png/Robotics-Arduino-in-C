README: Arduino Line-Following and Obstacle-Avoiding Robot

This project details the development and testing of an Arduino-based robot designed for line following and obstacle detection. The robot uses Light-Dependent Resistors (LDRs) to track a black line on a white surface and Infrared (IR) sensors to identify obstacles.

---

HARDWARE AND COMPONENTS

The program focuses on developing and testing an Arduino robot.

- Line Detection: 3 x Light-Dependent Resistors (LDRs) detect black and white surfaces.
- Obstacle Detection: Infrared Sensors (IR Sensors) detect obstacles.
- Movement: Servos provide movement for the robot.
- Status Indicators: LEDs indicate the status of the robot (Red, Yellow/Orange, Green).
- Calibration Storage: EEPROM stores and retrieves servo motor stop values (LEFT_STOP and RIGHT_STOP).

---

KEY FEATURES AND FUNCTIONALITY

1. Line Following (followLine())

The robot follows a black line on a white surface using LDR threshold values. The followLine() function reads LDR values using analogRead() and compares them to pre-determined LDR threshold values to decide on the next movement.

- If the middle LDR detects a black line, the robot moves forward.
- If the left LDR detects the line, the robot turns left.
- If the right LDR detects the line, the robot turns right.
- If no LDR detects the line, the robot stops, and the red LED turns on.

2. Obstacle Detection (detectObstacle())

IR sensors are used to check for obstacles.

- If an obstacle is detected, it's printed on a serial monitor.
- The yellow/orange LED will start flashing, indicating the status change (flashYellowLED()).
- Line following resumes only once the obstacle is removed.

3. Robot States (LED Indicators)

LEDs are used to indicate the status of the robot.

- Red: Turns on during recording (when following the line). Implemented.
- Yellow: Flashes when an obstacle is detected. Implemented.
- Green: Flashes during playback. Not Implemented (playback not added to the program).

4. Data Persistence (EEPROM)

EEPROM is used to store and retrieve important calibration values, ensuring consistent movement and avoiding manual calibration.

- Servo Stop Values (LEFT_STOP and RIGHT_STOP) are stored and retrieved using EEPROM.read() and updated with EEPROM.write().
- LDR Threshold Values are also stored, using bit shift to handle values over 255.

---

CHALLENGES AND RESOLUTIONS

- Noisy LDR Readings: LDR values skewed by shadows or uneven light. Program is run in an evenly lit environment to minimize shadow effects.
- Determining LDR Thresholds: Difficulty in setting consistent values. Used mean values calculated in the running conditions to filter out noise and maintain consistency.
- Yellow LED Dimness: Likely a hardware issue, damaged pin, or overheating. No fix was applied, but the light produced was deemed adequate for the program.
- Playback Function: Insufficient programming knowledge to implement the feature. Feature remains unimplemented.

---

TESTING AND DEBUGGING

Testing of the hardware components was performed using a preprocessor to debug functions, specifically using the #ifdef and #endif directives. Each debug was done over 15 trials.

- Left Push Button: Passed. 15 successful presses were checked.
- Servo Motors: Passed. Tested to ensure servos moved forward for 15 trials.
- LDR Sensors: Passed. 15 trials returned values for each LDR.
- IR Sensors: Passed. Checked if an obstacle was detected or not detected.
- LEDs: Passed. Checked on/off for 15 trials, though the yellow LED failed 2 trials due to faintness.
- Line Following: Passed. 10 out of 15 trials were successful; 5 failed due to shadows/lighting affecting line detection.

---

SUGGESTED FIXES AND FUTURE IMPROVEMENTS

The following steps are suggested to improve the robot's reliability:

1. Noisy LDR Readings / Determining Thresholds:
   - Hardware: Add a **light shield/hood** around the LDRs to block ambient light and shadows, focusing the sensor's view only on the immediate surface.
   - Software: Implement **Auto-Calibration on Startup**. The robot should dynamically find Max (White) and Min (Black) LDR values, then calculate the threshold as the midpoint.
   - Software: Use **Digital Filtering (Averaging)**. Take an average of 5-10 rapid successive readings for each LDR to stabilize input values against noise spikes.

2. Dim Yellow LED:
   - Hardware: **Check the Resistor**. Ensure the LED is connected with the correct current-limiting resistor. Replace the LED and resistor with a known good pair if necessary.
   - Hardware: **Use a Different Pin or Driver**. If the current Arduino pin is damaged, move the LED to a different digital output pin. For brighter output, consider an external transistor driver circuit.

3. Missing Playback Functionality:
   - Software: **Implement Playback Logic**. Read the stored path data (e.g., servo commands or directional choices) from EEPROM and execute them sequentially. Use the millis() function to log the timing and duration of actions during recording for accurate playback.
