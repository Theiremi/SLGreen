/*
  Project: SLGreen
  Author: Theirémi
  Date: 13/11/2023
  Contact: https://www.theiremi.fr/#contact or contact@theiremi.fr

  Description:
    This script, "SLGreen", is an interface for the SLCan - MCP2515 using Arduino. It is designed to facilitate 
    communication and control using the MCP2515 CAN Bus controller with Arduino. The script includes functions 
    for initializing the CAN bus, processing and sending CAN messages, and handling serial communications.

  License:
    This code is released under the MIT License. For more details, see the LICENSE file in the root directory 
    or visit https://opensource.org/licenses/MIT.
*/

//----- CONFIGURATION -----//
#define MCP_CS            10
#define MCP_FREQUENCY     MCP_8MHZ //Available values : MCP_8MHZ, MCP_16MHZ, MCP_20MHZ
#define SERIAL_SPEED      115200 //Recommended values : 115200, 150000, 250000, 500000, 1000000, 2000000
#define DEFAULT_CAN_SPEED CAN_500KBPS; //Available values : CAN_10KBPS, CAN_20KBPS, CAN_50KBPS, CAN_100KBPS, CAN_125KBPS, CAN_250KBPS, CAN_500KBPS, CAN_1000KBPS
//----------//

//Don't forget to search in mcp_can library sources to understand how functions works
#include <SPI.h>
#include "mcp_can.h"//MCP2515 library

MCP_CAN CAN(MCP_CS); // Set CS pin

uint8_t CAN_speed = CAN_500KBPS;
uint8_t CAN_freq  = MCP_8MHZ;
void setup() {
  Serial.begin(SERIAL_SPEED);

  if(CAN.begin(MCP_ANY, CAN_speed, CAN_freq) != CAN_OK) return slcanInvalidRX();
  CAN.setMode(MCP_NORMAL);
}


uint8_t rx_buffer[16] = {0};
uint8_t rx_buffer_len = 0;


bool CAN_active = true;

void loop() {
  serialCheckRX();
  canCheckRX();
}

void serialCheckRX() {
  if(Serial.available())
  {
	  byte received_char = Serial.read();

    // If the character is not a Carriage Return, we have received a character of a frame
    // The character is added to the buffer and the function ends here
    if(received_char != 13)
    {
      rx_buffer[rx_buffer_len] = received_char;
      rx_buffer_len++;

      //If the frame is too long, delete the buffer content to avoid a buffer overflow
      //The length 26 has been choosen as the max length of any incoming message (excluding the CR)
      if(rx_buffer_len > 26) rx_buffer_len = 0;

      return;
    }
    // If the character received is a CR, the function continues and the buffer is processed

    serialProcessRX(rx_buffer_len, rx_buffer);
    rx_buffer_len = 0;//Reset the buffer
  }
}

void serialProcessRX(uint8_t rx_buffer_len, uint8_t *rx_buffer)
{
   // No frame can be shorter than 1 character (excluding the CR)
  if(rx_buffer_len < 1)
  {
    rx_buffer_len = 0;
    return;
  }

  if(rx_buffer[0] == 'S')
  {
    if(rx_buffer_len != 2) return slcanInvalidRX();
    if(CAN_active) return slcanInvalidRX();

    uint8_t speed = asciiHex2int(rx_buffer[1]);

    switch(speed)
    {
      case 0:
        CAN_speed = CAN_10KBPS;
        break;
      case 1:
        CAN_speed = CAN_20KBPS;
        break;
      case 2:
        CAN_speed = CAN_50KBPS;
        break;
      case 3:
        CAN_speed = CAN_100KBPS;
        break;
      case 4:
        CAN_speed = CAN_125KBPS;
        break;
      case 5:
        CAN_speed = CAN_250KBPS;
        break;
      case 6:
        CAN_speed = CAN_500KBPS;
        break;
      case 8:
        CAN_speed = CAN_1000KBPS;
        break;
      case 7:// 800kbps not implemented
      default:
        return slcanInvalidRX();
    }

    Serial.write(13);
  }

  else if(rx_buffer[0] == 'O')
  {
    if(CAN_active) return slcanInvalidRX();

    if(CAN.begin(MCP_ANY, CAN_speed, CAN_freq) != CAN_OK) return slcanInvalidRX();
    CAN.setMode(MCP_NORMAL);

    CAN_active = true;
    Serial.write(13);
  }

  else if(rx_buffer[0] == 'L')
  {
    if(CAN_active) return slcanInvalidRX();

    if(CAN.begin(MCP_ANY, CAN_speed, CAN_freq) != CAN_OK) return slcanInvalidRX();
    CAN.setMode(MCP_LISTENONLY);

    CAN_active = true;
    Serial.write(13);
  }

  else if(rx_buffer[0] == 'C')
  {
    if(!CAN_active) return slcanInvalidRX();

    CAN.setMode(MCP_SLEEP);
    CAN_active = false;
    Serial.write(13);
  }

  else if(rx_buffer[0] == 't' || rx_buffer[0] == 'T')
  {
    if(!CAN_active) return slcanInvalidRX();

    bool extended_frame = (rx_buffer[0] == 't') ? 0 : 1;

    uint32_t address = 0;
    if(!extended_frame)
    {
      address = (asciiHex2int(rx_buffer[1]) << 8) +
        (asciiHex2int(rx_buffer[2]) << 4) +
        asciiHex2int(rx_buffer[3]);
    }
    else
    {
      address = ((uint32_t)asciiHex2int(rx_buffer[1]) << 28) +
        ((uint32_t)asciiHex2int(rx_buffer[2]) << 24) +
        ((uint32_t)asciiHex2int(rx_buffer[3]) << 20) +
        ((uint32_t)asciiHex2int(rx_buffer[4]) << 16) +
        ((uint32_t)asciiHex2int(rx_buffer[5]) << 12) +
        ((uint32_t)asciiHex2int(rx_buffer[6]) << 8) +
        ((uint32_t)asciiHex2int(rx_buffer[7]) << 4) +
        ((uint32_t)asciiHex2int(rx_buffer[8]) << 0);
    }

    uint8_t length =   asciiHex2int(rx_buffer[4 + 5*extended_frame]);

    if(address > 0x1FFFFFFF) return slcanInvalidRX();
    if(length > 8)           return slcanInvalidRX();
    if(rx_buffer_len != length * 2 + 5 + 5*extended_frame) return slcanInvalidRX();

    uint8_t buf[length] = {0};
    for(int i = 5 + 5*extended_frame; i < rx_buffer_len; i++)
    {
      buf[i] |= ((i+extended_frame) % 2 == 0) ? asciiHex2int(rx_buffer[i]) << 4 : asciiHex2int(rx_buffer[i]);
    }

    canTX(address, length, buf);

    Serial.write(13);
  }

  else if(rx_buffer[0] == 'V')
  {
    Serial.write('V');
    Serial.print("1010");
    Serial.write(13);
  }

  else if(rx_buffer[0] == 'N')
  {
    Serial.write('N');
    Serial.print("SLGR");
    Serial.write(13);
  }
}

void slcanInvalidRX()
{
  Serial.write(7);
}

void serialTX(bool ext, uint8_t *buf, uint8_t len)
{
  if(ext) Serial.write('T');
  else    Serial.write('t');
  for(int i = 0; i < len; i++)
  {
    Serial.write(buf[i]);
  }
  Serial.write(13);
}

void canCheckRX()
{
  if (CAN.checkReceive() == CAN_MSGAVAIL && CAN_active) {
    uint32_t id = 0;
    uint8_t  len = 0;
    bool ext = true;
    byte cdata[8] = {0};
    CAN.readMsgBuf(&id, &len, cdata);

    id = id & 0x1FFFFFFF;
    if(id <= 0x7FF) ext = false;

    uint8_t frame[4+len*2+ext*5] = {0};

    if(!ext)
    {
      frame[0] = int2asciiHex((id >> 8) & 0xF);
      frame[1] = int2asciiHex((id >> 4) & 0xF);
      frame[2] = int2asciiHex(id & 0xF);
    }
    else
    {
      frame[0] = int2asciiHex((id >> 28) & 0xF);
      frame[1] = int2asciiHex((id >> 24) & 0xF);
      frame[2] = int2asciiHex((id >> 20) & 0xF);
      frame[3] = int2asciiHex((id >> 16) & 0xF);
      frame[4] = int2asciiHex((id >> 12) & 0xF);
      frame[5] = int2asciiHex((id >> 8) & 0xF);
      frame[6] = int2asciiHex((id >> 4) & 0xF);
      frame[7] = int2asciiHex(id & 0xF);
    }

    frame[3+ext*5] = int2asciiHex(len);

    

    for(int i = 0; i < len*2; i++)
    {
      int i_bufpos = (len*2 - (i+1)) / 2;
      frame[(4+ext*5)+i] = (i_bufpos % 2 == 1) ? int2asciiHex(cdata[i_bufpos / 2] >> 4) : int2asciiHex(cdata[i_bufpos / 2] & 0xF);
    }

    serialTX(ext, frame, 4+len*2+ext*5);
  }
}

void canTX(uint32_t address, uint8_t len, uint8_t *buf)
{
  CAN.sendMsgBuf(address, address > 0x7FF ? 1 : 0, len, buf);
}

uint8_t asciiHex2int(uint8_t ascii)
{
  if(ascii >= 0x30 && ascii <= 0x39)
  {
    return ascii - 0x30;
  }
  else if(ascii >= 0x41 && ascii <= 0x46)
  {
    return ascii - 0x37;
  }
  return 0;
}

uint8_t int2asciiHex(uint8_t num)
{
  if(num <= 9)
  {
    return num + 0x30;
  }
  else if(num >= 10 && num <= 15)
  {
    return num + 0x37;
  }
  return 0;
}
