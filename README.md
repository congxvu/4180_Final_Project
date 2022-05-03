# Smart Water System
Team Members: Hung Nguyen and Cong Vu

**Georgia Institute of Technology**

## Table of Contents  
- [Project Idea](#project-idea)
- [Block Diagram](#block-diagram)
- [Project Prototype](#project-prototype)
- [Parts List](#parts-list)
- [Device Setup and Connection Table](#device-setup-and-connection-table)
- [Source Code](#source-code)

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


## Parts List
**Electronic components:**
- Mbed LPC1768 https://www.sparkfun.com/products/9564
- LCD Display: uLCD-144G2 https://www.sparkfun.com/products/11377
- Water Level Sensor https://www.amazon.com/gp/product/B07THDH7Y4/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&psc=1
- Temperature TMP36 https://www.sparkfun.com/products/10988
- Ambient Light Sensor: TEMT-6000 https://www.sparkfun.com/products/8688
- Red LED https://www.sparkfun.com/products/10632
- 2 Pushbuttons: https://www.sparkfun.com/products/97
- Speaker https://www.sparkfun.com/products/11089
- H-Bridge TB6612FNG https://www.sparkfun.com/products/14450
- Small geared DC motor https://www.sparkfun.com/products/13302
- LED - RGB https://www.sparkfun.com/products/105
- Resistors 330ohm, https://www.sparkfun.com/products/14490
- Jumper Wires(M/M and M/F) https://www.sparkfun.com/products/124, https://www.sparkfun.com/products/12794
- Battery Clip https://www.amazon.com/5pack-Battery-2-1mm-Arduino-Corpco/dp/B01AXIEDX8/ref=sr_1_3?keywords=arduino+battery&qid=1651567087&sr=8-3
- Battery https://www.amazon.com/Amazon-Basics-Performance-All-Purpose-Batteries/dp/B00MH4QM1S/ref=sr_1_7?crid=226BCE7DZS5V6&keywords=battery+E&qid=1651567157&sprefix=battery+e%2Caps%2C88&sr=8-7

**Non-electronics:**
- Small Box
- Water cup

## Device Setup and Connection Table
![279556225_1123972208387081_1864683205247866208_n](https://user-images.githubusercontent.com/70187862/166427471-d6adbf63-5c74-48b8-b93e-a19ae33b8802.jpg)
![278904672_564947624902952_2204330036185360628_n](https://user-images.githubusercontent.com/70187862/166427498-c2da9c22-e1ac-4196-b2b3-08bb42f3a948.jpg)

Control all components using the mbed LPC1768 microcontroller, as shown in the figure above. At the  top of the box are the TMP36 (temperature sensor), TEMT6000 (light sensor), RBG, red LED, LCD display,  small gear motor, and two pushbuttons. Below the box are speakers, an Hbridge, and an external power source (E-battery). The entire system is battery powered. 


Table of major components connection

| Mbed | Water Level  | TMP36 | TEMT6000| Speaker|RBG|LED|DC_motor|
| ----- |:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|
| 5V|||||||+|
| 3.3 V| + | + |+|+||+|
| GND |-|-|-|-|
| P20 |SIG|||
| P15 ||SIG||
| P18|||SIG|
| P26 ||||SIG|
| P22 |||||RED|
| P23 |||||GREEN|
| P24 |||||BLUE|
| P25||||||SIG|
| P7|||||||SIG|
| P8|||||||SIG|
| P21|||||||SIG|

Note: Our system uses a 5V battery to power the entire system. If you connect the mbed to your computer, you need a 5V power supply to run the motor.

## Source Code
The source code is included in this repository. Import the repository into MBedCloud Compiler and run it. Be sure to include all  libraries.
``` C#
#include "mbed.h"
#include "uLCD_4DGL.h"
#include "PinDetect.h"
#include "rtos.h"
//#include "NeoMatrix.h"
#include "Speaker.h"
#define LED_COUNT 24 // uses a 24-led ring
#define YELLOW 0xFFFF00
#include "Motor.h"

Mutex LCD;
Motor DC_motor(p21,p7,p8);

const int offset = 3;

uLCD_4DGL uLCD(p9,p10,p11); // serial tx, serial rx, reset pin;
Serial pc(USBTX, USBRX); // tx, rx
Timeout countdown; // for alarm duration
Timeout snooze;

class RGBLed
{
public:
    RGBLed(PinName redpin, PinName greenpin, PinName bluepin);
    void write(float red,float green, float blue);
private:
    PwmOut _redpin;
    PwmOut _greenpin;
    PwmOut _bluepin;
};

RGBLed::RGBLed (PinName redpin, PinName greenpin, PinName bluepin)
    : _redpin(redpin), _greenpin(greenpin), _bluepin(bluepin)
{
    //50Hz PWM clock default a bit too low, go to 2000Hz (less flicker)
    _redpin.period(0.2);
}

void RGBLed::write(float red,float green, float blue)
{
    _redpin = red;
    _greenpin = green;
    _bluepin = blue;
}
//Setup RGB led using PWM pins and class
RGBLed myRGBled(p22,p23,p24); //RGB PWM pins



// pb
PinDetect button1(p12,PullUp);
PinDetect button2(p13,PullUp);
PinDetect button3(p14,PullUp);


// sensors
AnalogIn waterSensor(p20);
AnalogIn lightSensor(p18);
AnalogIn tempSensor(p15);
// outputs
Speaker speaker(p26);
PwmOut myled(p25);
PwmOut led1(LED1);

// global variables
volatile float setWaterLevel = 0;
volatile float setLightLevelHigh = 0.5;
volatile float setLightLevelLow = 0.2;


int x_light_low, x_light_high;
// debug varibles

// sensor readings
volatile float water = 0.0;
volatile float current_water = 0.0;
volatile float pre_water = 0.0;
volatile float light = 0.0;
volatile float temp = 0.0;

volatile int fan_on = 0;

void Button3_Callback (void){
    // Turn off/on Fan
    fan_on = !fan_on;
}

void water_filling_function(void const *argument){
    while(1){      
        water = waterSensor.read();
        if (water<.2){
            for(float p = 0.0f; p < 1.0f; p += 0.1f) {
            myled = p; //dim led using PWM
            wait(0.1); //0.1 second time delay-so a human can see it
            }
            speaker.PlayNote(969.0, 2, 0.5);
            speaker.PlayNote(800.0, 0.5, 0.5);
            myled = 0;
        }
        else {
            Thread::wait(15000); // wait 15 mins for drinking but 15s in demo
            current_water = waterSensor.read();
                if (current_water >= (water-0.005)){
                        while (1){
                            myled = 1;
                            Thread::wait(8000); //wait 10 sec to check back
                            pre_water = waterSensor.read();
                            if (pre_water <= (current_water-0.005)){
                                myled = 0; //reset led
                                break;
                            }
                            else {
                                speaker.PlayNote(969.0, 2, 0.5);
                                speaker.PlayNote(800.0, 0.5, 0.5);                            
                            }
                        }
                }
                else{
                        myled = 0;
                }
        }
        Thread::wait(100);
    }
}


void fan_function(void const *argument){
    while(1){
        light = lightSensor.read();
        float tempC = ((tempSensor.read() * 3.3)-0.5)*100;
        if (light > 0.5 && tempC > 27 ){
            DC_motor.speed(1);
        }
        else { DC_motor.speed(fan_on);}
    Thread::wait(100);
    }
}

void rgb_function(void const *argument){
    while(1){
        light = lightSensor.read();
        float tempC = ((tempSensor.read() * 3.3)-0.5)*100;
        if (tempC > 27 && light < 0.5){
            myRGBled.write(0.0,0.0,1.0); //blue
            Thread::wait(100);
        }
        else if (light > 0.5 && tempC < 27){
            myRGBled.write(1.0,0.2,0.0); //yellow = red + some green
            Thread::wait(100);
        }
        else if (light > 0.5 && tempC > 27){
            myRGBled.write(1.0,0.0,0.0); //red
            Thread::wait(100);
        }
        else {
            myRGBled.write(0.0,1.0,0.0); //green
            Thread::wait(100);
        }
    Thread::wait(100);
    }
}

//volatile float current_water = 0.0;
int main() {
    uLCD.printf("\n\rstart printing\n");
    uLCD.cls();

    // attach button callbacks
    button3.attach_deasserted(&Button3_Callback);
    button3.setSampleFrequency();

    // attach threads
    //Thread water_timing_thread(water_timing_function);
    Thread water_filling_thread(water_filling_function);
    Thread fan_thread(fan_function);
    Thread rgb_thread(rgb_function);

    while(1) {

        light = lightSensor.read();
        water = waterSensor.read();
        temp = tempSensor.read();
        float tempC = ((temp * 3.3)-0.5)*100;
        float tempF = (tempC * 9 / 5) + 32.0; // calculate temperature F

        LCD.lock();

        // Draw a bar to display temperature
        uLCD.locate(1,1);
        uLCD.printf("%5.2F C %5.2F F \n\r", tempC, tempF);

        // Draw a bar to display light level
        uLCD.locate(0,5);
        uLCD.printf("LIGHT: %f\n",light);
        uLCD.rectangle(15, 47+offset, 112, 64+offset, WHITE);
        uLCD.filled_rectangle(16, 48+offset, 111, 63+offset, BLACK);
        if (light > 0.9)
            uLCD.filled_rectangle(16, 48+offset, 16 + floor((light/1.0f) * 96), 63+offset, RED);
        else if (light > setLightLevelHigh)
            uLCD.filled_rectangle(16, 48+offset, 16 + floor((light/1.0f) * 96), 63+offset, YELLOW);
        else if (light < setLightLevelLow)
            uLCD.filled_rectangle(16, 48+offset, 16 + floor((light/1.0f) * 96), 63+offset, YELLOW);
        else
            uLCD.filled_rectangle(16, 48+offset, 16 + floor((light/1.0f) * 96), 63+offset, GREEN);
        x_light_low = 16 + floor((setLightLevelLow/1.0f) * 96);
        x_light_high = 16 + floor((setLightLevelHigh/1.0f) * 96);
        uLCD.line(x_light_low, 48+offset, x_light_low, 63+offset, WHITE);
        uLCD.line(x_light_high, 48+offset, x_light_high, 63+offset, WHITE);


        // Draw a bar to display water level
        uLCD.locate(0,9);
        uLCD.printf("WATER: %f\n", water);
        uLCD.rectangle(15, 79+offset, 112, 96+offset, WHITE);
        uLCD.filled_rectangle(16, 80+offset, 111, 95+offset, BLACK);
        if (water > 0.4)
            uLCD.filled_rectangle(16, 80+offset, 16 + floor((water/1.0f) * 96), 95+offset, GREEN);
        else if (water > 0.2)
            uLCD.filled_rectangle(16, 80+offset, 16 + floor((water/1.0f) * 96), 95+offset, YELLOW);
        else
            uLCD.filled_rectangle(16, 80+offset, 16 + floor((water/1.0f) * 96), 95+offset, RED);
        LCD.unlock();
        Thread::wait(1000);
    }
}
