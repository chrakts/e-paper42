/**
 *  @filename   :   epd4in2.h
 *  @brief      :   Header file for Dual-color e-paper library epd4in2.cpp
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

#ifndef EPD4IN2_H
#define EPD4IN2_H

#include "epdif.h"
#include "spi_driver.h"

// Display resolution
#define EPD_WIDTH       400
#define EPD_HEIGHT      300

// EPD4IN2 commands
#define PANEL_SETTING                               0x00
#define POWER_SETTING                               0x01
#define POWER_OFF                                   0x02
#define POWER_OFF_SEQUENCE_SETTING                  0x03
#define POWER_ON                                    0x04
#define POWER_ON_MEASURE                            0x05
#define BOOSTER_SOFT_START                          0x06
#define DEEP_SLEEP                                  0x07
#define DATA_START_TRANSMISSION_1                   0x10
#define DATA_STOP                                   0x11
#define DISPLAY_REFRESH                             0x12
#define DATA_START_TRANSMISSION_2                   0x13
#define LUT_FOR_VCOM                                0x20
#define LUT_WHITE_TO_WHITE                          0x21
#define LUT_BLACK_TO_WHITE                          0x22
#define LUT_WHITE_TO_BLACK                          0x23
#define LUT_BLACK_TO_BLACK                          0x24
#define PLL_CONTROL                                 0x30
#define TEMPERATURE_SENSOR_COMMAND                  0x40
#define TEMPERATURE_SENSOR_SELECTION                0x41
#define TEMPERATURE_SENSOR_WRITE                    0x42
#define TEMPERATURE_SENSOR_READ                     0x43
#define VCOM_AND_DATA_INTERVAL_SETTING              0x50
#define LOW_POWER_DETECTION                         0x51
#define TCON_SETTING                                0x60
#define RESOLUTION_SETTING                          0x61
#define GSST_SETTING                                0x65
#define GET_STATUS                                  0x71
#define AUTO_MEASUREMENT_VCOM                       0x80
#define READ_VCOM_VALUE                             0x81
#define VCM_DC_SETTING                              0x82
#define PARTIAL_WINDOW                              0x90
#define PARTIAL_IN                                  0x91
#define PARTIAL_OUT                                 0x92
#define PROGRAM_MODE                                0xA0
#define ACTIVE_PROGRAMMING                          0xA1
#define READ_OTP                                    0xA2
#define POWER_SAVING                                0xE3

#define RES_400x300 0b00000000
#define RES_320x300 0b01000000
#define RES_320x240 0b10000000
#define RES_200x300 0b11000000

#define RES1      (1<<7)
#define RES0      (1<<6)
#define REG_EN    (1<<5)
#define BWR       (0)
#define BW        (1<<4)
#define UD        (1<<3)
#define SHL       (1<<2)
#define SHD_N     (1<<1)
#define RST_N     (1<<0)

enum{NOTHING=0,CLEAR1,CLEAR2,CLEAR3,CLEAR4,CLEAR5,CLEAR6,CLEAR7,CLEAR8,CLEAR9,
      DISPLAYFRAME1,DISPLAYFRAME2,DISPLAYFRAME3,DISPLAYFRAME4,DISPLAYFRAME5,DISPLAYFRAME6,DISPLAYFRAME7,DISPLAYFRAME8,
      SLEEP1,SLEEP2,SLEEP3,SLEEP4,SLEEP5,SLEEP6,SLEEP7,
      RESET1,RESET2,RESET3,RESET4,RESET5,RESET6,RESET7,RESET8,RESET9,RESET10,
      DISPLAYSHOW1,DISPLAYSHOW2,DISPLAYSHOW3,DISPLAYSHOW4,DISPLAYSHOW5,DISPLAYSHOW6
      };
extern const unsigned char lut_vcom0[];
extern const unsigned char lut_ww[];
extern const unsigned char lut_bw[];
extern const unsigned char lut_bb[];
extern const unsigned char lut_wb[];

class Epd : EpdIf {
public:
    unsigned int width;
    unsigned int height;

    Epd();
    ~Epd();
    int  Init(SPI_Master_t *spiMaster);
    void SendCommand(unsigned char command);
    void SendData(unsigned char data);
    void FillingData(unsigned char data,uint32_t len);
    void FillingCompleteData(const uint8_t data);
    void TransferingData(const uint8_t *  data,uint32_t len);
    void TransferingCompleteData(const uint8_t *  data);
    uint8_t SpiTransferReady();
    void WaitUntilIdle(void);
    uint8_t getIdle(void);
    void Reset(void);
    void SetPartialWindow(const unsigned char* frame_buffer, int x, int y, int w, int l);
    void SetPartialWindowBlack(const unsigned char* buffer_black, int x, int y, int w, int l);
    void SetPartialWindowRed(const unsigned char* buffer_red, int x, int y, int w, int l);
    void SetLut(void);
    void DisplayFrame(const unsigned char* frame_buffer);
    int8_t DisplayFrameStep(unsigned char* frame_buffer);
    void DisplayFrame(void);
    int8_t DisplayFrameStep(void);
    void ClearFrame(void);
    int8_t ClearFrameStep(void);
    void Sleep(void);
    int8_t SleepStep(void);
    int8_t ResetStep(void);


private:
    uint8_t status = NOTHING;
//    unsigned int reset_pin;
//    unsigned int dc_pin;
//    unsigned int cs_pin;
//    unsigned int busy_pin;
};

#endif /* EPD4IN2_H */

/* END OF FILE */
