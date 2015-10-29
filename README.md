# ENB345-Rain-sensing-window-project
# Project Code

1) open_close_window_final.ino

This is the Arduino code to enable the core functionality of the rain-sensing window.
Window will open and close according to water and obstruction sensors. Motor will remain
active while IR distance sensor is within the programmed distance of a set flag.

2) wifi_functionality_only.ino

This is the Arduino code to enable remote control of the window only. Sensors will not
open or close the window. However, notifications can be received on status of the sensors
or window. There are 5 possible status notifications -
      i. NO RAIN, WINDOW CLOSED
      ii. NO RAIN, WINDOW OPEN
      iii. NO RAIN, WINDOW OBSTRUCTED
      iv. RAINING, WINDOW CLOSED
      v. RAINING, WINDOW OBSTRUCTED

Ensure Android application code is uploaded to phone to enable remote control.

3) phone_app/Httphelper

Basic mobile phone application to allow remote control of aforementioned window. Upload
to phone via Android Studio.

Credit has to be given to allaboutee.com for their communication skeletons.
