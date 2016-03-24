extern "C" {
    struct Pixel {
        int red;
        int green;
        int blue;
    };


    /**
     * Returns the absolute value of the double passed in
     */
    double fabs(double a);

    /**
     * Prints hello world to the stdout to show that the library is working.
     */
    void helloWorld();

    /**
     * Initialises the hardware on the raspberry pi including the SPI and camera hardware
     */
    void init();

    /**
     * sets the motor speeds
     * Minimum speed is -1 and maximum is 1
     */
    void setMotors(double aSpeed, double bSpeed);

    /**
     * takes camera picture and stores it into the buffer
     */
    char *takePicture();

    /**
     * Saves the picture in the buffer to image.ppm
     */
    void savePicture();

    /**
     * Returns color component (0:red, 1:green, 2:blue, 3:luminosity)
     * for pixel located at row x col
     */
    Pixel getPixelAt(int column, int row);

    /**
     * Returns the position of a white line in the camera frame,
     * 0.0 -> left
     * 0.5 -> center
     * 1.0 -> right
     */
    double getLine();

    /**
     * Converts a pin label to the raspberry pi GPIO number
     */
    int labelToGpio(int pin);

    /**
     * Set the pin mode
     * mode, 0:output, 1:input
     */
    void pinMode(int pin, int mode);

    /**
     * sets a digital output high
     */
    void digitalWriteNew(int pin, int level);

    /**
     * Reads the digital input and returns the value
     */
    int readDigital(int pin);

    /**
     * sets the duty cycle of pin to value
     * value is a value between 0 and 1
     */
    void setPWM(int pin, double value);

    /**
     * Converts an ADC pin label to a raspberry pi GPIO number
     */
    int labelAdcToPinNumber(int pin);

    /**
     * read analog voltage from ADC
     */
    int readAnalog(int pin);


    char * getPage(const char * host, char * arguments, unsigned int portNumber);
    void getPassword(const char * URL, char buffer[50]);
}