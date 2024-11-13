# Stopwatch-with-Dual-Mode-Increment-and-Countdown-Using-ATmega32-and-Seven-Segment-Display
This repository contains the Stopwatch with Dual Mode project, developed as part of the Edges For Training Standard Embedded Systems Diploma. The project is a digital stopwatch system built with an ATmega32 microcontroller, six multiplexed seven-segment displays, and multiple push buttons, offering two operational modes: counting up and counting down.

Project Overview
This stopwatch provides the following modes and features:

1. Increment Mode (Default): The stopwatch counts up from zero, displaying hours, minutes, and seconds on six seven-segment displays.
2. Countdown Mode: The stopwatch counts down from a user-defined start time, acting as a timer.

Key Features
- Reset: Resets time to zero.
- Pause/Resume: Temporarily halts time counting or resumes from the paused state.
- Mode Toggle: Switches between increment and countdown modes.
- Time Adjustments: Allows incrementing or decrementing hours, minutes, and seconds.
- Alarm/Buzzer: Triggers when countdown reaches zero.
- LED Indicators: Red LED for increment mode and yellow LED for countdown mode.

Hardware Components
- Microcontroller: ATmega32
- Display: Six multiplexed common-anode seven-segment displays driven by a 7447 BCD decoder.
- Input Buttons:
  - Reset: Connected to PD2 (INT0) - resets stopwatch.
  - Pause: Connected to PD3 (INT1) - pauses time.
  - Resume: Connected to PB2 (INT2) - resumes time.
  - Mode Toggle: Connected to PB7 - toggles between modes.
  - Time Adjustments: Buttons for incrementing/decrementing hours, minutes, and seconds.
- Alarm/Buzzer: Connected to PD0 - activates when countdown ends.
- LED Indicators:
  - Counting Up: Red LED on PD4.
  - Counting Down: Yellow LED on PD5.

System Requirements
- ATmega32 Microcontroller at 16MHz.
- Timer1 configured in CTC mode for time tracking.
- Multiplexed display control using transistors.
- External interrupts for reset, pause, and resume functionality.

Operation Details
1. Increment Mode: The stopwatch starts counting up automatically when powered on.
2. Countdown Mode Setup:
  - Pause the stopwatch.
  - Toggle to countdown mode.
  - Set the countdown start time using adjustment buttons.
  - Start the countdown by resuming the stopwatch.
  - The buzzer will activate when the countdown reaches zero.
