# README #

### What is this repository for? ###
This repository contains a number of programs to be flashed on to a Maevarm 2 (M2) microcontroller.

#### Clean ####
This is a set of clean files used on the M2 and is used as the basis for new programs.

#### Acrobot ####
This is a self-balancing robot on two wheels. It uses a gyrometer and accelerometer to sense the degree and velocity of tilt. It then responds accordingly through a PID controller.

#### Orchestra ####
Orchestra recieves and interprets wireless packets from a master microcontroller. These packets contain information on the duration and frequency of a music note. The Orchestra robot will interpret these packets and produce the proper note at the correct frequency and duration to produce music. *Orchestra-working is the final working version