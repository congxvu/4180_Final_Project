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
