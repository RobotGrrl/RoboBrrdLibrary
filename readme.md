# **RoboBrrd Library v1.0**

---

![](http://robobrrd.com/images/rb3d/rb-pixelforest-small.jpg)

Let's get your RoboBrrd's brain up and chirping. This is the Arduino library that will help make this happen!

With this library, you will be able to...

- Control all of RoboBrrd's servos
- Set its eyes to a RGB or HSI colour
- Read the sensor readings from the light sensors (or just use the callback methods)
- Check out several example sketches

There is also an API available for RoboBrrd through hardware serial, or optionally software serial (or any Stream object for that matter). For more info on all of the API commands, please see _Serial_API_Info.h_.

---

# Getting Started

There are a few steps you will need to do to get started.

1. Close Arduino first, and download this RoboBrrd library as a zip using the button on the right hand side

2. Rename the folder to just 'RoboBrrd'. Install the library to your Arduino/libraries folder. For more info on how to install a library, please [see here](http://arduino.cc/en/Guide/Libraries).

3. Install the [Streaming](http://arduiniana.org/libraries/streaming/) library

4. Install the [Promulgate](https://github.com/RobotGrrl/Promulgate) library

4. Open the _DownByTheBay_ example sketch

5. Connect your RoboBrrd (by using a FTDI cable or similar).

6. Choose the corresponding port for RoboBrrd in the Arduino Tools>Port menu.

7. Select _Arduino Pro or Pro Mini (5V, 16 MHz) w/ ATmega328_ from the Arduino Tools>Boards menu.

8. Upload the sketch to RoboBrrd

9. Open Serial Monitor (the button on the top right corner)

10. Your RoboBrrd should start displaying the lines to 'down by the bay'!

11. Finally, check out the other example sketches available. Just have fun- run it, remix it, make your RoboBrrd do whatever you envisioned!

If there are any problems, feel free to ask me and any of the other RoboBrrd masters on the [forum](http://robobrrd.com/forum).

---

# Credits

The code is licensed under the MIT license- for more info see license.txt.

This library was heavily based off of the [ClydeDev library](https://github.com/RobotGrrl/ClydeDev), so shoutout to Fabule for the inspiration!

Thank you to everyone who backed the RoboBrrd Indiegogo campaign back in 2012. Thanks to everyone who continues to support the development of RoboBrrd by purchasing a kit!

For more updates and information:

- [RoboBrrd 3D's Nest on the Web](http://robobrrd.com/rb3d)
- [RoboBrrd.com](http://robobrrd.com)
- [@RoboBrrd](http://twitter.com/robobrrd)

---
