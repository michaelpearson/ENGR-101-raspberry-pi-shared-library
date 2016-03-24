#include <stdio.h>
#include <unistd.h>
#include "wiringPi.h"
#include "wiringPiSPI.h"
#include "softPwm.h"
#include "camera.h"
#include "main.h"

#include <netdb.h>
#include <netinet/in.h>
#include <string.h>


#define MAIN_TEXTURE_WIDTH 640 //Control Resolution from Camera
#define MAIN_TEXTURE_HEIGHT 480 //Control Resolution from Camera
#define CAMERA_FRAME_RATE 90

CCamera *camera;
char pictureBuffer[MAIN_TEXTURE_WIDTH * MAIN_TEXTURE_HEIGHT * 4];
int spi;


extern "C"
{



    double fabs(double a) {
        return a > 0 ? a : -a;
    }

    void helloWorld () {
        printf("Hello world\n");
    }

    void init() {
        wiringPiSetup();

        // set motor pins
        softPwmCreate(26, 0, 100);
        softPwmCreate(27, 0, 100);
        softPwmCreate(28, 0, 100);
        softPwmCreate(29, 0, 100);

        //how many detail levels (1 = just the capture res, > 1 goes down by half each level, 4 max)
        camera = StartCamera(MAIN_TEXTURE_WIDTH, MAIN_TEXTURE_HEIGHT, CAMERA_FRAME_RATE, 1, true);
        if(camera == NULL) {
            printf("Camera init failure\n");
            exit(1);
        }
        delay(500);

        spi = wiringPiSPISetup(0, 1000000);
        if (spi < 0) {
            printf("SPI init failure error code: %d\n", spi);
            exit(1);
        }
        printf("Init complete!\n");
    }

    /**
     * sets the motor speeds
     * Minimum speed is -1 and maximum is 1
     */
    void setMotors(double aSpeed, double bSpeed) {
        if(aSpeed > 0) {
            softPwmWrite(26, (int)(fabs(aSpeed) * 100));
            softPwmWrite(27, 0);
        } else {
            softPwmWrite(27, (int)(fabs(aSpeed) * 100));
            softPwmWrite(26, 0);
        }

        if(bSpeed > 0) {
            softPwmWrite(28, (int)(fabs(bSpeed) * 100));
            softPwmWrite(29, 0);
        } else {
            softPwmWrite(29, (int)(fabs(bSpeed) * 100));
            softPwmWrite(28, 0);
        }
    }

    /**
     * takes camera picture and stores it into the buffer
     */
    char * takePicture() {
        delay(1000 / CAMERA_FRAME_RATE);
        camera->ReadFrame(0, pictureBuffer, sizeof(pictureBuffer));
        return pictureBuffer;
    }

    /**
     * Saves the picture in the buffer to image.ppm
     */
    void savePicture() {
        takePicture();
        FILE *fp = fopen("image.ppm","wb");
        if(!fp) {
            printf("Unable to create the image\n");
            exit(1);
        }
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
    Pixel getPixelAt(int column, int row) {
        if ((row < 0 ) || (row > MAIN_TEXTURE_HEIGHT) ) {
            printf("row is out of range\n");
            return (Pixel) {0, 0, 0};
        }
        if ((column < 0) || (column > MAIN_TEXTURE_WIDTH)) {
            printf("column is out of range\n");
            return (Pixel) {0, 0, 0};
        }
        // calculate address in 1D array of pixels
        int address = MAIN_TEXTURE_WIDTH * row * 4 + column * 4;
        return (Pixel) {pictureBuffer[address], pictureBuffer[address + 1], pictureBuffer[address + 2]};
    }

    double getLine() {
        takePicture();
        int row = MAIN_TEXTURE_HEIGHT / 2;
        double summation = 0;
        for(int a = 0;a < MAIN_TEXTURE_WIDTH;a++) {
            Pixel p = getPixelAt(a, row);
            double luminosity = (double)(p.red + p.green + p.blue) / (3.0 * 255.0);
            summation += (luminosity * a);
        }
        return (summation / MAIN_TEXTURE_WIDTH);
    }

    int labelToGpio(int pin) {
        switch(pin) {
            case 0:
                pin = 7;
                break;
            case 1:
                pin = 0;
                break;
            case 2:
                pin = 2;
                break;
            case 3:
                pin = 3;
                break;
            case 4:
                pin = 22;
                break;
            case 5:
                pin = 23;
                break;
            case 6:
                pin = 24;
                break;
            case 7:
                pin = 25;
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
    void pinModeNew(int pin, int mode) {
        pin = labelToGpio(pin);
        if(pin > -1) {
            pinMode(pin, mode);
        }
    }

    /**
     * sets a digital output high
     */
    void digitalWriteNew(int pin, int level) {
        pin = labelToGpio(pin);
        if(pin > -1) {
            digitalWrite(pin, level > 0 ? HIGH : LOW);
        }
    }

    /**
     * Reads the digital input and returns the value
     */
    int digitalReadNew(int pin) {
        pin = labelToGpio(pin);
        if(pin > -1) {
            return(digitalRead(pin));
        }
        return 0;
    }

    /**
     * sets the duty cycle of pin to value
     * value is a value between 0 and 1
     */
    void setPWM(int pin, double value) {
        printf("Cant do that!\n");
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
        write(spi, txBuf, 3);
        read(spi, rxBuf, 3);
        return ((rxBuf[1]&3) << 8) | (rxBuf[2]);
    }


    /**
     * Downloads the password at the given URL
     * Password is a char array with a maximum length of 10
     */
    void getPassword(const char * URL, char buffer[50]) {
        char * data = getPage(URL, (char *) "", 80);
        memset(buffer, '\0', 50);
        if(data != NULL) {
            memcpy(buffer, data, strlen(data));
            free(data);
        }
    }

    /**
     * Submits the password to the given url
     * returns 1 for success (gate will open) 0 for incorrect password
     */
    int sendPassword(const char * URL, char * password) {
        char * passwordFormat = (char *) "?password=%s";
        char * passwordBuilt = (char *) malloc(strlen(password) + strlen(passwordFormat));
        sprintf(passwordBuilt, passwordFormat, password);
        char * data = getPage(URL, passwordBuilt, 80);
        free(passwordBuilt);
        int success = 0;
        if(strcmp(data, "success") == 0) {
            success = 1;
        }
        free(data);
        return success;
    }

    char * getPage(const char * host, char * arguments, unsigned int portNumber) {
        int sockfd, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        /* Create a socket point */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("ERROR opening socket");
            return 0;
        }
        server = gethostbyname(host);
        if (server == NULL) {
            fprintf(stderr,"ERROR, no such host\n");
            return NULL;
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy(server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (size_t) server->h_length);
        serv_addr.sin_port = (in_port_t) htons(portNumber);
        /* Now connect to the server */
        if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR connecting");
            return NULL;
        }
        const char *requestFormat = (char *) "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: ENGR 101 AVC\r\n\r\n";
        char * requestBuilt = (char *) malloc(sizeof(char) * (strlen(requestFormat) + strlen(host) + strlen(arguments)));
        sprintf(requestBuilt, requestFormat, arguments, host);
        n = write(sockfd, requestBuilt, strlen(requestBuilt));
        free(requestBuilt);
        if (n < 0) {
            perror("ERROR writing to socket");
            return NULL;
        }
        static char response[2048];
        bzero(response, 2048);
        n = read(sockfd, response, 2047);
        if (n < 0) {
            perror("ERROR reading from socket");
            return NULL;
        }

        char * a = response;
        //find the end of the response header!
        while(*a != '\0') {
            if(*a++ == '\n') {
                if((*a == '\r' && *(++a) == '\n') || (*a) == '\n') {
                    a++;
                    break;
                }
            }
        }
        //End at the first new line
        if(*(a) != '\0') {
            int i = 0;
            while(*(a + i) != '\0' && *(a + i) != '\n' && *(a + i) != '\r') {
                i++;
            }
            *(a + i) = '\0';
        }
        char * data = (char *) calloc(strlen(a) + 1, sizeof(char));
        memcpy(data, a, strlen(a));
        return data;
    }
}

