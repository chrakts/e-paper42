/**
 *  @filename   :   epdif.cpp
 *  @brief      :   Implements EPD interface functions
 *                  Users have to implement all the functions in epdif.cpp
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "epdif.h"
#include <util/delay.h>
#include <SPI.h>
#include "ePaper42hardware.h"

EpdIf::EpdIf() {
};

EpdIf::~EpdIf() {
};
/*
void EpdIf::DigitalWrite(int pin, int value) {
    digitalWrite(pin, value);
}

int EpdIf::DigitalRead(int pin) {
    return digitalRead(pin);
}*/


void EpdIf::SpiTransfer(unsigned char data) {
//    digitalWrite(CS_PIN, LOW);
    CS_PORT.OUTCLR = CS_PIN;
    SPI_MasterTransceiveByte(epSpiMaster, data);
//    epSPI->transfer((uint8_t)data); ****************************
//    digitalWrite(CS_PIN, HIGH);
    CS_PORT.OUTSET = CS_PIN;
}

int EpdIf::IfInit(SPI_Master_t *spi) {
/*    pinMode(CS_PIN, OUTPUT);
    pinMode(RST_PIN, OUTPUT);
    pinMode(DC_PIN, OUTPUT);*/
    CS_PORT.DIRSET  = CS_PIN;
    RST_PORT.DIRSET = RST_PIN;
    DC_PORT.DIRSET  = DC_PIN;
//    pinMode(BUSY_PIN, INPUT);
    BUSY_PORT.DIRCLR  = BUSY_PIN;
    //epSPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    //epSPI = new SPI(0); // ***************
    //epSPI->begin(); // ***************
    epSpiMaster = spi;
    SPI_MasterCreateDataPacket(&dataPacket,NULL,NULL,0,&CS_PORT,CS_PIN);
    return 0;
}

uint8_t EpdIf::IfTransferPacket()
{
  return( SPI_MasterInterruptTransceivePacket(epSpiMaster,&dataPacket) );
}

void EpdIf::IfFillingData(unsigned char data,uint32_t len)
{
    dataPacket.bytesToTransceive = len;
    dataPacket.singleByte = data;
    dataPacket.receiveData = NULL;
    dataPacket.transmitData = NULL;
//    dataPacket.complete = false;
    dataPacket.bytesTransceived = 0;
    epSpiMaster->dataPacket = NULL;
    IfTransferPacket();
}

void EpdIf::IfTransferingData(uint8_t * data,uint32_t len)
{
    dataPacket.bytesToTransceive = len;
    dataPacket.singleByte = 0;
    dataPacket.receiveData = NULL;
    dataPacket.transmitData = data;
//    dataPacket.complete = false;
    dataPacket.bytesTransceived = 0;
    epSpiMaster->dataPacket = NULL;
    IfTransferPacket();
}



