#include "mbed.h"

DigitalIn Switch(SW2);
DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);
BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
char table[20] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,
                  0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xEF};      // final digit add digit point
int fr[10] = {0};         // frequency
Serial pc( USBTX, USBRX );
AnalogIn Ain(A0);
AnalogOut Aout(DAC0_OUT);

float ADCdata;
int f;
int sample = 1024;
float fr_data[1024] = {0};

void LED(int freq);
void SSD(int freq);
void monitor(void);
//void Sample(void);



int main(void) 
{
    int i;
    monitor();

    while(1) {
        LED(f);
        for (i=0; i<sample; i++) {
            Aout = fr_data[i];
            wait(1./sample);       
        }
    }
}

void LED(int freq) 
{
    if(Switch == 1) {
        redLED = 1;
        greenLED = 0;
        display = 0;
    }
    else {
        redLED = 0;
        greenLED = 1;
        SSD(freq);
    }
}

void SSD(int freq)
{
    int x;
    int n = 0;
    int i;          // loop index

    x = freq;

    while ((x / 10) != 0) {
        fr[n] = (x % 10);
        n++;
        x = (x / 10);
    }
    fr[n] = (x % 10);

    for (i=n; i>=0; i--) {
        if (i == 0) {
            display = table[fr[i] + 10];
            wait(1/2);
        }
        else {
            display = table[fr[i]];
            wait(1);
        }
    }
}

void monitor(void)
{
    int i;
    int a, b;

    a = 0;
    b = 0;
    
    for (i=0; i<sample; i++){
        Aout = Ain;
        fr_data[i] = Ain;
        wait(1./sample);
    }
    for (i=0; i<sample; i++){
        pc.printf("%1.3f\r\n", fr_data[i]);
        //fr_data[i] = ADCdata;
    }

    f = 0;

    for (i=0; (i<sample) && (b == 0 || a == 0); i++) {
        if (fr_data[i] < 0.03) {
            a = i;
        }
        else if (fr_data[i] > 0.57) {
            b = i;
        }
    }

    if (a > b) f = sample / 2 / (a - b) ;
    else f = sample / 2 / (b - a) ; 
}
