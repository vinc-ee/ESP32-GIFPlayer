# ESP32-CYD: Dynamic GIF Player

This project enables dynamic GIF playback from a microSD card, with additional control features via HTTP requests or Google Home (via Arduino IoT)

## Demo
[![ESP32-CYD GIF Demo](https://via.placeholder.com/150)](https://your-demo-link.com)  
*Click the image to watch the demo.*

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
   - **Google Home**: On/off control of the backlight (*Learn how to set up Arduino IoT and create your own credentials [here](https://docs.arduino.cc/arduino-cloud/guides/overview/).*)

3. **Adding GIFs**:
   - Place `.gif` files on the microSD card. Ensure file names match the predefined list in the hardcoded code (e.g., `/1.gif`, `/2.gif`, `/3.gif` etc).







1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/ESP32-CYD.git

