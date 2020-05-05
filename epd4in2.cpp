/**
 *  @filename   :   epd4in2.cpp
 *  @brief      :   Implements for Dual-color e-paper library
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

#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <epd4in2.h>
#include "ePaper42hardware.h"
#include "ledHardware.h"
#include "MyTimer.h"

extern volatile bool isDisplayReady;
extern volatile TIMER MyTimers[];

extern unsigned char *globalImage;

Epd::~Epd() {
};

Epd::Epd() {
//    reset_pin = RST_PIN;
//   dc_pin = DC_PIN;
//  cs_pin = CS_PIN;
//    busy_pin = BUSY_PIN;
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
};

int Epd::Init(SPI_Master_t *spiMaster) {
    /* this calls the peripheral hardware interface, see epdif */
    uint8_t init1[] = {0x03,0x00,0x2b,0x2b,0xff};
    if (IfInit(spiMaster) != 0) {
        return -1;
    }
    /* EPD hardware init start */
    Reset();
    SendCommand(POWER_SETTING);
    TransferingData(init1,5);
    while(SpiTransferReady()==false)
      ;
/*    SendData(0x03);                  // VDS_EN, VDG_EN
    SendData(0x00);                  // VCOM_HV, VGHL_LV[1], VGHL_LV[0]
    SendData(0x2b);                  // VDH
    SendData(0x2b);                  // VDL
    SendData(0xff); */                 // VDHR
    SendCommand(BOOSTER_SOFT_START);
    FillingData(0X17,3);
    while(SpiTransferReady()==false)
      ;
/*    SendData(0x17);
    SendData(0x17);
    SendData(0x17);                  //07 0f 17 1f 27 2F 37 2f*/
    SendCommand(POWER_ON);
    WaitUntilIdle();
    SendCommand(PANEL_SETTING);
    SendData(0xbf);    // KW-BF   KWR-AF  BWROTP 0f
    SendData(0x0b);
    SendCommand(PLL_CONTROL);
    SendData(0x3c);        // 3A 100HZ   29 150Hz 39 200HZ  31 171HZ
    /* EPD hardware init end */
    return 0;
}

/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command) {
    //DigitalWrite(dc_pin, LOW);
    DC_PORT.OUTCLR = DC_PIN;
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendData(unsigned char data) {
//    DigitalWrite(dc_pin, HIGH);
    DC_PORT.OUTSET = DC_PIN;
    SpiTransfer(data);
}

/**
 *  @brief: function for filling data
 */
void Epd::FillingData(unsigned char data,uint32_t len)
{
//    DC_PORT.OUTSET = DC_PIN;
    IfFillingData(data,len);
}

/**
 *  @brief: function for filling data
 */
void Epd::FillingCompleteData(unsigned char data)
{
//    DC_PORT.OUTSET = DC_PIN;
    IfFillingData(data, (width * height)/ 8 );
}

/**
 *  @brief: function for transfering data without receiving
 */
void Epd::TransferingData(uint8_t *  data,uint32_t len)
{
//    DC_PORT.OUTSET = DC_PIN;
    IfTransferingData(data,len);
}

/**
 *  @brief: function for transfering complete display-data without receiving
 */
void Epd::TransferingCompleteData(const uint8_t *  data)
{
//    DC_PORT.OUTSET = DC_PIN;
    IfTransferingData(data,(width * height) / 8 );
}

uint8_t Epd::SpiTransferReady()
{
  return(dataPacket.complete);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void Epd::WaitUntilIdle(void) {
//    while(DigitalRead(busy_pin) == 0) {      //0: busy, 1: idle
    while((BUSY_PORT.IN & BUSY_PIN) == 0) {      //0: busy, 1: idle
        _delay_ms(100);
    }
}

/**
 *  @brief: Gives Status of the busy_pin
 */
uint8_t Epd::getIdle(void) {
    return((BUSY_PORT.IN & BUSY_PIN) == BUSY_PIN);       //0: busy, 1: idle
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void Epd::Reset(void) {
//    DigitalWrite(reset_pin, LOW);
    RST_PORT.OUTCLR = RST_PIN;
    _delay_ms(200);
//    DigitalWrite(reset_pin, HIGH);
    RST_PORT.OUTSET = RST_PIN;
    _delay_ms(200);
}

/**
 *  @brief: transmit partial data to the SRAM
 */
void Epd::SetPartialWindow(const unsigned char* buffer_black, int x, int y, int w, int l) {
    SendCommand(PARTIAL_IN);
    SendCommand(PARTIAL_WINDOW);
    SendData(x >> 8);
    SendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    SendData(((x & 0xf8) + w  - 1) >> 8);
    SendData(((x & 0xf8) + w  - 1) | 0x07);
    SendData(y >> 8);
    SendData(y & 0xff);
    SendData((y + l - 1) >> 8);
    SendData((y + l - 1) & 0xff);
    SendData(0x01);         // Gates scan both inside and outside of the partial window. (default)
    _delay_ms(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    int i;
    if (buffer_black != NULL)
    {
        for(i = 0; i < (w  / 8 )* l; i++)
        {
            SendData(buffer_black[i]);
        }
    }
    else
    {
        for(i = 0; i < (w  / 8 ) * l; i++)
        {
            SendData(0x00);
        }
    }
    _delay_ms(2);
    SendCommand(PARTIAL_OUT);
}

/**
 *  @brief: set the look-up table
 */
void Epd::SetLut(void) {
    unsigned int count;
    SendCommand(LUT_FOR_VCOM);                            //vcom
    for(count = 0; count < 44; count++) {
        SendData(lut_vcom0[count]);
    }

    SendCommand(LUT_WHITE_TO_WHITE);                      //ww --
    for(count = 0; count < 42; count++) {
        SendData(lut_ww[count]);
    }

    SendCommand(LUT_BLACK_TO_WHITE);                      //bw r
    for(count = 0; count < 42; count++) {
        SendData(lut_bw[count]);
    }

    SendCommand(LUT_WHITE_TO_BLACK);                      //wb w
    for(count = 0; count < 42; count++) {
        SendData(lut_bb[count]);
    }

    SendCommand(LUT_BLACK_TO_BLACK);                      //bb b
    for(count = 0; count < 42; count++) {
        SendData(lut_wb[count]);
    }
}

/**
 * @brief: refresh and displays the frame
 */
void Epd::DisplayFrame(const unsigned char* frame_buffer) {
    SendCommand(RESOLUTION_SETTING);
    SendData(width >> 8);
    SendData(width & 0xff);
    SendData(height >> 8);
    SendData(height & 0xff);

    SendCommand(VCM_DC_SETTING);
    SendData(0x12);

    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0x97); //war sendCommand    //VBDF 17|D7 VBDW 97  VBDB 57  VBDF F7  VBDW 77  VBDB 37  VBDR B7

    if (frame_buffer != NULL) {
        SendCommand(DATA_START_TRANSMISSION_1);
        for(unsigned int i = 0; i < (width / 8) * height; i++) {
            SendData(0xFF);      // bit set: white, bit reset: black
        }
        _delay_ms(2);
        SendCommand(DATA_START_TRANSMISSION_2);
        for(unsigned int i = 0; i < (width / 8) * height; i++) {
            SendData(pgm_read_byte(&frame_buffer[i]));
        }
        _delay_ms(2);
    }

    SetLut();

    SendCommand(DISPLAY_REFRESH);
    _delay_ms(100);
    WaitUntilIdle();
}

int8_t Epd::DisplayFrameStep(unsigned char* frame_buffer)
{
//  char startBytes[9]={0,0,(char)((EPD_WIDTH-1)>> 8),(char)((EPD_WIDTH-1)|0x0007),0,0,(char)((EPD_HEIGHT-1)>> 8),(char)((EPD_HEIGHT-1)|0x00ff),0x01};
  char startBytes[9]={0,0,0x01,0x8f,0,0,0x01,0x2b,0x01};
  switch(status)
  {
    case NOTHING:
 //     SendCommand(PARTIAL_IN);
 //     SendCommand(PARTIAL_WINDOW);
 //     TransferingData(startBytes,9);
 //     status = DISPLAYFRAME1;


    SendData(0);
    SendData(0);     // x should be the multiple of 8, the last 3 bit will always be ignored
    SendData(1);
    SendData(0x8f);
    SendData(0);
    SendData(0);
    SendData(1);
    SendData(0x2b);
    SendData(0x01);         // Gates scan both inside and outside of the partial window. (default)
    status = DISPLAYFRAME2;

    break;
    case DISPLAYFRAME1:
      if(SpiTransferReady()==true)
        status += 1;
    break;
    case DISPLAYFRAME2:
      _delay_ms(2);
      if(getIdle()==1)
        status += 1;
    break;
    case DISPLAYFRAME3:
      SendCommand(DATA_START_TRANSMISSION_2);
//      TransferingCompleteData(frame_buffer);
//      TransferingData(globalImage,15000);
      for(uint16_t i = 0; i < 15000; i++)
        {
            SendData(frame_buffer[i]);
        }

      status += 1;
      status += 1;
    break;
    case DISPLAYFRAME4:
      if(SpiTransferReady()==true)
      {
        status += 1;
      }
    break;
    case DISPLAYFRAME5:
      _delay_ms(2);
      if(getIdle()==1)
        status += 1;
    break;
    case DISPLAYFRAME6:
      SendCommand(PARTIAL_OUT);
      SetLut();
      SendCommand(DISPLAY_REFRESH);
      _delay_ms(100);
      status += 1;
    break;
    case DISPLAYFRAME7:
       if(getIdle()==1)
         status = NOTHING;
    break;
    default:
      status = NOTHING;
      return(-1);
  }
  return(status);
}

/**
 * @brief: clear the frame data from the SRAM, this won't refresh the display
 */
void Epd::ClearFrame(void) {
    SendCommand(RESOLUTION_SETTING);
    SendData(width >> 8);
    SendData(width & 0xff);
    SendData(height >> 8);
    SendData(height & 0xff);

    SendCommand(DATA_START_TRANSMISSION_1);
    _delay_ms(2);
    for(unsigned int i = 0; i < width / 8 * height; i++) {
        SendData(0xFF);
    }
    _delay_ms(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    _delay_ms(2);
    for(unsigned int i = 0; i < width / 8 * height; i++) {
        SendData(0xFF);
    }
    _delay_ms(2);
}

/**
 * @brief: Step 01 to clear the frame data from the SRAM, this won't refresh the display
 */
int8_t Epd::ClearFrameStep(void)
{
  switch(status)
  {
    case NOTHING:
      SendCommand(RESOLUTION_SETTING);
      SendData(width >> 8);
      SendData(width & 0xff);
      SendData(height >> 8);
      SendData(height & 0xff);
      SendCommand(DATA_START_TRANSMISSION_1);
      status = CLEAR1;
    break;
    case CLEAR1:
      if(getIdle()==1)
        status += 1;
    break;
    case CLEAR2:
      FillingCompleteData(0xff);
      status += 1;
    break;
    case CLEAR3:
      if(SpiTransferReady()==true)
        status += 1;
    break;
    case CLEAR4:
      if(getIdle()==1)
        status += 1;
    break;
    case CLEAR5:
      SendCommand(DATA_START_TRANSMISSION_2);
      status += 1;
    break;
    case CLEAR6:
      if(getIdle()==1)
        status += 1;
    break;
    case CLEAR7:
      FillingCompleteData(0xff);
      status += 1;
    break;
    case CLEAR8:
      if(SpiTransferReady()==true)
      status += 1;
    break;
    case CLEAR9:
      if(getIdle()==1)
      {
        status = NOTHING;
      }
    break;
    default:
      status = NOTHING;
      return(-1);
  }
  return(status);

}

/**
 * @brief: This displays the frame data from SRAM
 */
void Epd::DisplayFrame(void) {
    SetLut();
    SendCommand(DISPLAY_REFRESH);
    _delay_ms(100);
    WaitUntilIdle();
}

/**
 * @brief: This displays the frame data from SRAM
 */
int8_t Epd::DisplayFrameStep(void) {
  switch(status)
  {
    case NOTHING:
      SetLut();
      SendCommand(DISPLAY_REFRESH);
      isDisplayReady = false;
      MyTimers[DISPLAY_READY_TIMER].value = 10;
      MyTimers[DISPLAY_READY_TIMER].state = TM_START;
      status = DISPLAYSHOW1;
    break;
    case DISPLAYSHOW1:
      if(isDisplayReady==true)
        status++;
    break;
    case DISPLAYSHOW2:
      if(getIdle()==1)
        status = NOTHING;
    break;
    case DISPLAYSHOW3:
    break;
    case DISPLAYSHOW4:
    break;
    case DISPLAYSHOW5:
    break;
    case DISPLAYSHOW6:
    break;
  }
  return(status);
}

/**
 * @brief: After this command is transmitted, the chip would enter the deep-sleep mode to save power.
 *         The deep sleep mode would return to standby by hardware reset. The only one parameter is a
 *         check code, the command would be executed if check code = 0xA5.
 *         You can use Epd::Reset() to awaken and use Epd::Init() to initialize.
 */
void Epd::Sleep() {
    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0x17);                       //border floating
    SendCommand(VCM_DC_SETTING);          //VCOM to 0V
    SendCommand(PANEL_SETTING);
    _delay_ms(100);

    SendCommand(POWER_SETTING);           //VG&VS to 0V fast
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);
    _delay_ms(100);

    SendCommand(POWER_OFF);          //power off
    WaitUntilIdle();
    SendCommand(DEEP_SLEEP);         //deep sleep
    SendData(0xA5);
}


int8_t Epd::ResetStep(void)
{
  uint8_t init1[] = {0x03,0x00,0x2b,0x2b,0xff};
  switch(status)
  {
    case NOTHING:
      RST_PORT.OUTCLR = RST_PIN;
      isDisplayReady = false;
      MyTimers[DISPLAY_READY_TIMER].value = 20;
      MyTimers[DISPLAY_READY_TIMER].state = TM_START;
      status = RESET1;
    break;
    case RESET1:
      if(isDisplayReady==true)
        status++;
    break;
    case RESET2:
      RST_PORT.OUTSET = RST_PIN;
      isDisplayReady = false;
      MyTimers[DISPLAY_READY_TIMER].value = 20;
      MyTimers[DISPLAY_READY_TIMER].state = TM_START;
      status++;
    break;
    case RESET3:
//       _delay_ms(200);
      if(isDisplayReady==true)
        status++;
    break;
    case RESET4:
      SendCommand(POWER_SETTING);
      TransferingData(init1,5);
      status++;
    break;
    case RESET5:
      if(SpiTransferReady()==true)
        status++;
    break;
    case RESET6:
      SendCommand(BOOSTER_SOFT_START);
      FillingData(0X17,3);
      status++;
    break;
    case RESET7:
      if(SpiTransferReady()==true)
        status++;
    break;
    case RESET8:
       SendCommand(POWER_ON);
       status++;
    break;
    case RESET9:
      if(getIdle()==1)
        status += 1;
    break;
    case RESET10:
      SendCommand(PANEL_SETTING);
      SendData(0xbf);    // KW-BF   KWR-AF  BWROTP 0f
      SendData(0x0b);
      SendCommand(PLL_CONTROL);
      SendData(0x3c);    // 3A 100HZ   29 150Hz 39 200HZ  31 171HZ
      status = NOTHING;
    break;
    default:
      status = NOTHING;
      return(-1);
  }
  return(status);
}
/*
    uint8_t init1[] = {0x03,0x00,0x2b,0x2b,0xff};
    if (IfInit(spiMaster) != 0) {
        return -1;
    }
    Reset();
    SendCommand(POWER_SETTING);
    TransferingData(init1,5);
    while(SpiTransferReady()==false)
      ;
    SendCommand(BOOSTER_SOFT_START);
    FillingData(0X17,3);
    while(SpiTransferReady()==false)
      ;

    SendCommand(POWER_ON);
    WaitUntilIdle();
    SendCommand(PANEL_SETTING);
    SendData(0xbf);    // KW-BF   KWR-AF  BWROTP 0f
    SendData(0x0b);
    SendCommand(PLL_CONTROL);
    SendData(0x3c);        // 3A 100HZ   29 150Hz 39 200HZ  31 171HZ

*/
/**
 * @brief: After this command is transmitted, the chip would enter the deep-sleep mode to save power.
 *         The deep sleep mode would return to standby by hardware reset. The only one parameter is a
 *         check code, the command would be executed if check code = 0xA5.
 *         You can use Epd::Reset() to awaken and use Epd::Init() to initialize.
 */
int8_t Epd::SleepStep(void)
{
  switch(status)
  {
    case NOTHING:
      SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
      SendData(0x17);                       //border floating
      SendCommand(VCM_DC_SETTING);          //VCOM to 0V
      SendCommand(PANEL_SETTING);
      status = SLEEP1;
    break;
    case SLEEP1:
      if(getIdle()==1)
        status += 1;
    break;
    case SLEEP2:
      SendCommand(POWER_SETTING);           //VG&VS to 0V fast
      FillingData(0x00,5);

/*      SendData(0x00);
      SendData(0x00);
      SendData(0x00);
      SendData(0x00);
      SendData(0x00);*/
      status += 1;
    break;
    case SLEEP3:
      if(SpiTransferReady()==true)
        status += 1;
    break;
    case SLEEP4:
      if(getIdle()==1)
        status += 1;
    break;
    case SLEEP5:
      SendCommand(POWER_OFF);          //power off
      status += 1;
    break;
    case SLEEP6:
      if(getIdle()==1)
        status += 1;
    break;
    case SLEEP7:
      SendCommand(DEEP_SLEEP);         //deep sleep
      SendData(0xA5);
      status = NOTHING;
    break;
    default:
      status = NOTHING;
      return(-1);
  }
  return(status);
}

const unsigned char lut_vcom0[] =
{
0x00, 0x17, 0x00, 0x00, 0x00, 0x02,
0x00, 0x17, 0x17, 0x00, 0x00, 0x02,
0x00, 0x0A, 0x01, 0x00, 0x00, 0x01,
0x00, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};
const unsigned char lut_ww[] ={
0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};
const unsigned char lut_bw[] ={
0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};

const unsigned char lut_bb[] ={
0x80, 0x17, 0x00, 0x00, 0x00, 0x02,
0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
0x80, 0x0A, 0x01, 0x00, 0x00, 0x01,
0x50, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};

const unsigned char lut_wb[] ={
0x80, 0x17, 0x00, 0x00, 0x00, 0x02,
0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
0x80, 0x0A, 0x01, 0x00, 0x00, 0x01,
0x50, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};



/* END OF FILE */





