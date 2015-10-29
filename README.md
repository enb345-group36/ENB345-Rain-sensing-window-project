# ENB345-Rain-sensing-window-project
# Project Code

## open_close_window_final.ino

This is the Arduino code to enable the core functionality of the rain-sensing window.
Window will open and close according to water and obstruction sensors. Motor will remain
active while IR distance sensor is within the programmed distance of a set flag.

## wifi_functionality_only.ino

This is the Arduino code to enable remote control of the window only. Sensors will not
open or close the window. However, notifications can be received on status of the sensors
or window. There are 5 possible status notifications -
<ol>
</ol>

      1. NO RAIN, WINDOW CLOSED
      2. NO RAIN, WINDOW OPEN
      3. NO RAIN, WINDOW OBSTRUCTED
      4. RAINING, WINDOW CLOSED
      5. RAINING, WINDOW OBSTRUCTED

Ensure Android application code is uploaded to phone to enable remote control.

## phone_app/Httphelper

Basic mobile phone application to allow remote control of aforementioned window. Upload
to phone via Android Studio.

Credit has to be given to allaboutee.com for their communication skeletons.
