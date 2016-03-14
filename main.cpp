// The functions contained in this file are pretty dummy
// and are included only as a placeholder. Nevertheless,
// they *will* get included in the static library if you
// don't remove them :)
//
// Obviously, you 'll have to write yourself the super-duper
// functions to include in the resulting library...
// Also, it's not necessary to write every function in this file.
// Feel free to add more files in this project. They will be
// included in the resulting library.

#include <stdio.h>
#include <unistd.h>
#include <pigpio.h>
#include "camera.h"

#define MAIN_TEXTURE_WIDTH 320 //Control Resolution from Camera
#define MAIN_TEXTURE_HEIGHT 240 //Control Resolution from Camera

CCamera *camera;
char pictureBuffer[MAIN_TEXTURE_WIDTH * MAIN_TEXTURE_HEIGHT * 4];
int spi_h;


extern "C"
{
    double fabs(double a) {
        return a > 0 ? a : -a;
    }

    void helloWorld () {
        printf("Hello world\n");
    }

    void init() {
        if(gpioInitialise() < 0) {
          printf("GPIO initialization failed \n");
          exit(1);
        }
        // set motor pins
        gpioSetMode(12,PI_OUTPUT);
        gpioSetMode(16,PI_OUTPUT);
        gpioSetMode(20,PI_OUTPUT);
        gpioSetMode(21,PI_OUTPUT);

        //how many detail levels (1 = just the capture res, > 1 goes down by half each level, 4 max)
        int num_levels = 1;
        camera = StartCamera(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT, 30, num_levels, true);
        if(camera == NULL) {
            printf("Camera init failure\n");
            exit(1);
        }

        spi_h = spiOpen(0, 1000000, 0);
        if (spi_h < 0) {
            printf("SPI init failure error code: %d\n", spi_h);
            exit(1);
        }

        //success give camera time to settle
        sleep(1);
    }



    /**
     * sets the motor speeds
     * Minimum speed is -1 and maximum is 1
     */
    void setMotors(double aSpeed, double bSpeed) {
        if(aSpeed > 0) {
            gpioPWM(12, (unsigned int) fabs(aSpeed));
            gpioPWM(16, 0);
        } else {
            gpioPWM(16, (unsigned int) fabs(aSpeed));
            gpioPWM(12, 0);
        }

        if(bSpeed > 0) {
            gpioPWM(20, (unsigned int) fabs(bSpeed));
            gpioPWM(21, 0);
        } else {
            gpioPWM(21, (unsigned int) fabs(bSpeed));
            gpioPWM(20, 0);
        }
    }

    /**
     * takes camera picture and stores it into the buffer
     */
    char * takePicture() {
       printf("Taking camera picture\n");
       camera->ReadFrame(0, &(pictureBuffer[0]), sizeof(pictureBuffer));
       return pictureBuffer;
    }

    /**
     * Saves the picture in the buffer to image.ppm
     */
    void savePicture() {
            FILE *fp = fopen("image.ppm","wb");
            if (!fp) {
               printf("Unable to open the file\n");
                return;
            }

            //write file header
            fprintf(fp, "P6\n %d %d %d\n", MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT, 255);
            int ind = 0;
            for (int row = 0; row < MAIN_TEXTURE_HEIGHT; row++) {
               for (int col = 0; col < MAIN_TEXTURE_WIDTH; col++) {
                 int red = pictureBuffer[ind];
                 int green = pictureBuffer[ind + 1];
                 int blue = pictureBuffer[ind + 2];
                 fprintf(fp, "%c%c%c", red, green, blue);
                 ind = ind + 4;
               }
            }
            fflush(fp);
            fclose(fp);
    }

    /**
     * returns color component (0:red, 1:green, 2:blue, 3:luminosity)
     * for pixel located at row x col
     */
    char getPixelAt(int column, int row, int color) {
        // calculate address in 1D array of pixels
        int address = MAIN_TEXTURE_WIDTH * row * 4 + column * 4;
        if ((row < 0 ) || (row > MAIN_TEXTURE_HEIGHT) ) {
            printf("row is out of range\n");
            return -1;
        }
        if ((column < 0) || (column > MAIN_TEXTURE_WIDTH)) {
            printf("column is out of range\n");
            return -1;
        }

        char red = pictureBuffer[address];
        char green = pictureBuffer[address + 1];
        char blue =pictureBuffer[address + 2];
        char y = (char) ((red + green + blue) / 3);

        switch(color) {
            case 0:
                return red;
            case 1:
                return green;
            case 2:
                return blue;
            case 3:
                return y;
            default:
                printf("Color encoding wrong\n");
                return -1;
        }
    }

    void getLine(int row, int threshold) {
        //TODO: read line
        //takePicture();
        printf("Todo!\n");
    }

    /*
    void sleep(double seconds) {
        int sec = (int)(seconds);
        int usec = (int) ((seconds - sec) * 1000000);
        gpioSleep(PI_TIME_RELATIVE, sec, usec);
    }
     */

    int labelToGpio(int pin) {
        switch(pin) {
            case 0:
                pin = 4;
                break;
            case 1:
                pin = 17;
                break;
            case 2:
                pin = 27;
                break;
            case 3:
                pin = 22;
                break;
            case 4:
                pin = 6;
                break;
            case 5:
                pin = 13;
                break;
            case 6:
                pin = 19;
                break;
            case 7:
                pin = 26;
                break;
            default:
                printf("Invalid pin number %d\n", pin);
                return -1;
        }
        return pin;
    }


    /**
     * Set the pin mode
     * mode, 0:output, 1:input
     */
    void pinMode(int pin, int mode) {
        pin = labelToGpio(pin);
        if(pin > 0) {
            if(mode > 0) {
                gpioSetMode((unsigned int) pin, PI_INPUT);
            } else {
                gpioSetMode((unsigned int) pin, PI_OUTPUT);
            }
        }
    }

    /**
     * sets a digital output high
     */
    void writeDigital(int pin, int level) {
        pin = labelToGpio(pin);
        if(pin > 0) {
            gpioWrite((unsigned int) pin, level > 0 ? 1 : 0);
        }
    }

    /**
     * Reads the digital input and returns the value
     */
    int readDigital(int pin) {
        pin = labelToGpio(pin);
        if(pin > 0) {
            return(gpioRead((unsigned int) pin));
        }
        return 0;
    }

    /**
     * sets the duty cycle of pin to value
     * value is a value between 0 and 1
     */
    int setPWM(int pin, double value) {
        pin = labelToGpio(pin);
        if(pin > 0) {
            value = value > 1 ? 1 : value < 0 ? 0 : value;
            gpioPWM((unsigned int) pin, (unsigned int) (value * 255));
        }
    }


    int labelAdcToPinNumber(int pin) {
        switch(pin) {
            default:
            case 0:
                return 7;
            case 1:
                return 6;
            case 2:
                return 5;
            case 3:
                return 4;
            case 4:
                return 3;
            case 5:
                return 2;
            case 6:
                return 1;
            case 7:
                return 0;
        }
    }

    /**
     * read analog voltage from ADC
     */
    int readAnalog(int pin) {
        char txBuf[3], rxBuf[3];
        pin = labelAdcToPinNumber(pin);
        int ch_adc_b0 = pin & 1;
        int ch_adc_b1 = (pin & 2) >> 1;
        int ch_adc_b2 = (pin & 4) >> 2;
        txBuf[0] = 1;
        txBuf[1] = (char) (8 | (ch_adc_b2 << 2) | (ch_adc_b1 << 1) | ch_adc_b0);
        txBuf[1] = txBuf[1]<<4;
        txBuf[2] = 0;
        spiXfer((unsigned int) spi_h, txBuf, rxBuf, 3);
        return ((rxBuf[1]&3) << 8) | (rxBuf[2]);
    }
}

