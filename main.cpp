/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
/*
#include "mbed.h"
#include "LSM6DSO.h"

#define WAIT_TIME_MS 500 
DigitalOut led1(LED1);
//I2C i2c(PB_9,PB_8);
LSM6DS33 DOF6(PB_9, PB_8);
int main()
{
    char cmd[2];
    printf("This is the bare metal blinky example running on Mbed OS %d.%d.%d.\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
DOF6.begin(LSM6DS33::G_SCALE_500DPS,LSM6DS33::A_SCALE_8G,LSM6DS33::G_ODR_104,LSM6DS33::A_ODR_104);
DOF6.calibration(1000);
DOF6.readAccel();
printf("%f %f %f \r\n ",DOF6.ax,DOF6.ay,DOF6.az);
    while (true)
    {
       //
       DOF6.readAccel();
printf("%f %f %f \r\n ",DOF6.ax,DOF6.ay,DOF6.az);
DOF6.readGyro();
printf("%f %f %f \r\n ",DOF6.gx,DOF6.gy,DOF6.gz);
       cmd[0]=0x0F;
        cmd[1]=0xAA;
       //i2c.write(0xD6,cmd,1);
       cmd[0]=0x0F;
        cmd[1]=0xAA;
       // i2c.read(0xD6,cmd,1);
        printf("%02X, %02X \r\n", cmd[0],cmd[1]);
        led1 = !led1;
        thread_sleep_for(WAIT_TIME_MS);
    }
}
*/
//////////////////////////
// Comparaison :
// Inclinometre a Acceleromètre
// Inclinometre à Gyrometre 
// 
// GR YLP Mai 2022
// IUT de Cachan
/////////////////////////
#include "mbed.h"
#include "LSM6DSO.h"
#include "string.h"


//
#define TE 10ms
LSM6DS33 DOF6(PB_9, PB_8);
UnbufferedSerial pc(USBTX,USBRX,9600);
Ticker ticker;
DigitalOut led1(LED1); 
char ligne[100];
volatile bool flag=0;
double psig=0,psia;
double gyro_zero(void)
{
    const int NN=1000;
    float GyroBuffer[3];
    double gy_off=0;
    for(int i=0; i<NN; i++) {
        DOF6.readGyro();
        gy_off=gy_off+DOF6.gy/NN;
    }
    return(gy_off);
}
double angle_zero(void)
{
    const int NN=1000;
    int16_t AccBuffer[3];
    double PI=4*atan(1.0);
    double ang_off=0;
    for(int i=0; i<NN; i++) {
        DOF6.readAccel();
        double ang=(180/PI)*atan2((double)DOF6.ax,(double)DOF6.ay);
        ang_off=ang_off+ang/NN;
    }
    return ang_off;
}
void mesure(void)
{
    flag=1;
}
int main()
{
    float GyroBuffer[3];
    int16_t AccBuffer[3];
    sprintf(ligne,"Super inclinometre VAA\n\r");
    pc.write(ligne, strlen(ligne));
    DOF6.begin(LSM6DS33::G_SCALE_500DPS,LSM6DS33::A_SCALE_8G,LSM6DS33::G_ODR_104,LSM6DS33::A_ODR_104);
    double PI=4*atan(1.0);
    //double gyr_off=gyro_zero();
    //double ang_off=angle_zero();
    double gyr_off=0;
    double ang_off=0;
    ticker.attach(&mesure,TE);
    unsigned char cpt=0;
    
    while(1) {
        if(flag) {
            DOF6.readAccel();
            psia=((180/PI)*atan2((double)DOF6.ay,(double)DOF6.az))-ang_off;
            DOF6.readGyro();
            //psig=psig+(DOF6.gx-gyr_off)*TE;
            if(cpt==9) {
                cpt=0;
                led1 = !led1;
                sprintf(ligne,"$%f %f %f;\n",psia,psig,gyr_off);
                pc.write(ligne,strlen(ligne));
            }
            cpt++;
            flag=0;
        }
    }
}