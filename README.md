# msp430F5529_ssd1306
the project is based upon https://github.com/sdp8483/MSP430G2_SSD1306_OLED.
Updated registernames and bitnames
I2C address is 0x78
and teh ssd1306_command function is updated so it works
void ssd1306_command(unsigned char command) {
    buffer[0] = 0x00; //0x80;    sometimes it should be 0x80 instead of 0x00
    buffer[1] = command;
    i2c_write(SSD1306_I2C_ADDRESS, buffer, 2);
} 
compiled  by
Code Composer Studio Version: 12.3.0.00005 
