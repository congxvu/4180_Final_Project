# Smart Water System
Team Members: Hung Nguyen and Cong Vu

**Georgia Institute of Technology**

## Table of Contents  
- [Project Idea](#project-idea)
- [Block Diagram](#block-diagram)
- [Project Prototype](#project-prototype)
- [Parts List](#parts-list)

## Project Idea
Since 60% of the human body contains water, we need to drink about 4 liters (0.5 gallons) per day. Our smart devices remind and track people's hydration levels.

- The automatic intelligent water system tracks and reminds you of your hydration. 

- Smart Weather Detective helps users detect temperature and sunlight.

- Automatic cooling fan system for ventilation.

- Enable the reset button to allow the user to update the system.

- Portable and compact system.

- Esay to use and set up.

## Block Diagram
![Screenshot (148)](https://user-images.githubusercontent.com/70187862/166420389-0ea8c96e-ce27-4300-b863-19cc541653c1.png)

As the figure above, there is an LCD screen that indicates the temperature, water level, and light level. There are also two pushbuttons to control the fan and reset button. In every cycle 15s as we set in the timer, the LED will turn on red to remind user to drink water. If the water level drop down which mean the user drank the water, the LED will turn off. If the user still haven`t drink water after 10s, the speaker will play the warn sound. When the water level falls below the set minimum water level, the speaker will  sound a warning and the LED will blink.
The purpose of RBG is to indicate temperature and sunlight. The RBG turns blue when the temperature exceeds 27 ° C and turns yellow when the light level is high. When both the temperature and light levels are high, the fan is activated and turned on and the RBG will turn red. There are two push buttons for the user to control the fan and reset the system freely.

## Project Prototype 

![278608709_524870539284090_1579216101570779499_n](https://user-images.githubusercontent.com/70187862/166414445-57d0e6da-788b-47e2-8259-56ab0ee61ad5.jpg)

![279167041_1158609128304466_7568191816031071137_n](https://user-images.githubusercontent.com/70187862/166414455-a4c7dea1-6582-4226-a92a-7fcf4fcee310.jpg)

![279556225_1123972208387081_1864683205247866208_n](https://user-images.githubusercontent.com/70187862/166414463-78eda838-d684-4e5a-ad54-41eeb4d411c7.jpg)

## Parts List
- Mbed LPC1768(https://www.sparkfun.com/products/9564)
