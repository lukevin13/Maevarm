# Maevarm
This repository contains a number of programs to be flashed on to a Maevarm 2 (M2) microcontroller for projects in the Mechatronics course at the University of Pennsylvania.

## What programs are in this repository?
### Clean
This is a set of clean files for new projects that is used as the base file-structure and framework for new projects.

### Acrobot
This is a self-balancing robot on two wheels. It uses a gyrometer and accelerometer to sense the degree and velocity of tilt. It then responds accordingly through a PID controller that will drive mounted motors.

### Orchestra
[Demo Video - Bass Range](https://goo.gl/photos/28tcafCCFrdNMV7v8)  
Orchestra receives and interprets wireless packets from a master microcontroller. These packets contain information on the duration and frequency of a music note. The Orchestra robot will interpret these packets and produce the proper note at the correct frequency and duration to produce music.  

**Note:**

*    Orchestra contains test code used to ensure wireless communications and wave-generation were working.  
*    Orchestra-working is the final working version of the project

## Flashing programs onto the M2
Instructions vary across different operating systems:

* [Windows](http://medesign.seas.upenn.edu/index.php/Guides/MaEvArM-starting)
* [OS X](http://medesign.seas.upenn.edu/index.php/Guides/MaEvArM-starting-mac)
* [Linux](http://medesign.seas.upenn.edu/index.php/Guides/MaEvArM-starting-linux)

**Note:** The files and directories of each project in this repository are structured to be flashed to the M2 from a Linux machine.
## Further Information
For more information, please see the Maevarm documentation on the [MEAM.Design](http://medesign.seas.upenn.edu/index.php/Guides/MaEvArM) page.
