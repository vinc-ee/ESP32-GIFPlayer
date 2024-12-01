# ESP32-CYD: Dynamic GIF Player

This project enables dynamic GIF playback from the microSD reader of the ESP32-CYD, with additional control features via HTTP requests or Google Home (via Arduino IoT)

## Demo

https://github.com/user-attachments/assets/7edc9192-5ec8-405c-b7cc-d6bfda62c60c

## Features

-  **GIF Playback**: Displays GIF images stored on the microSD card.
-  **HTTP Server**: Switch GIFs, adjust brightness, or turn the display on/off using HTTP requests.
-  **Google Home Integration**: Control the display's backlight (on/off) as if it were a smart lamp (uses Arduino IoT).
-  **Dynamic Updates**: Add or update GIFs on the microSD card without reflashing the firmware.

---

## How It Works

1. **Hardware Setup**:
   - ESP32 CYD (Cheap Yellow Display)
   - MicroSD card module (SD_CS_PIN is set to `5`)
   - Arduino IoT account + Google Home

2. **Control Options**:
   - **HTTP Server**:
     - `/on`: Turn on the backlight.
     - `/off`: Turn off the backlight.
     - `/3.gif`: Play the `3.gif` file from the microSD card (SD formatted to FAT32).
     - Root URL (`/`): View current brightness and active GIF.
   - **Google Home**: On/off control of the backlight (*Learn how to set up Arduino IoT and get your own credentials [here](https://docs.arduino.cc/arduino-cloud/guides/overview/).*)

3. **Adding GIFs**:
   - Place `.gif` files on the microSD card. Ensure file names match the predefined list in the hardcoded code (e.g., `/1.gif`, `/2.gif`, `/3.gif`, etc.). 
   - The GIFs should be 320x240 px and use a max 64-color palette. Use an online converter like [ezgif.com](https://ezgif.com/). 
   - **Test GIFs Available**: A folder with test GIFs is included in the GitHub repository. Copy the files (without folder) in your microSD card and insert it into the ESP32. 

4. **Important**:
   - Before flashing the ESP32, ensure that only `.ino` files are present in the Arduino IDE.
   - If you experience issues getting the ESP32 to work, try installing the **ESP32 CYD libraries** provided by RandomNerdTutorials.
