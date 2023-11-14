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
#define MCP_CS        10;
#define MCP_FREQUENCY MCP_8MHZ //Available values : MCP_8MHZ, MCP_16MHZ, MCP_20MHZ
#define SERIAL_SPEED  115200 //Recommended values : 115200, 150000, 250000, 500000, 1000000, 2000000
#DEFAULT_CAN_SPEED    CAN_500KBPS; //Available values : CAN_10KBPS, CAN_20KBPS, CAN_50KBPS, CAN_100KBPS, CAN_125KBPS, CAN_250KBPS, CAN_500KBPS, CAN_1000KBPS
//----------//

//Don't forget to search in mcp_can library sources to understand how functions works
#include <SPI.h>
#include "mcp_can.h"//MCP2515 library

MCP_CAN CAN(MCP_CS); // Set CS pin

void setup() {
  Serial.begin(SERIAL_SPEED);
}


uint8_t rx_buffer[16] = {0};
uint8_t rx_buffer_len = 0;

uint8_t CAN_speed = CAN_500KBPS;
uint8_t CAN_freq  = MCP_8MHZ;
bool CAN_active = false;
bool auto_poll = false;
uint8_t buffered_frame[25] = {0};
uint8_t buffered_frame_len = 0;

void loop() {
  slcanCheckRX();

  if (CAN.checkReceive() == CAN_MSGAVAIL && CAN_active) {
    uint32_t id = 0;
    uint8_t  len = 0;
    byte cdata[8] = {0};
    CAN.readMsgBuf(&id, &len, cdata);

    id = id & 0x1FFFFFFF;

    buffered_frame[0] = int2asciiHex((id >> 28) & 0xF);
    buffered_frame[1] = int2asciiHex((id >> 24) & 0xF);
    buffered_frame[2] = int2asciiHex((id >> 20) & 0xF);
    buffered_frame[3] = int2asciiHex((id >> 16) & 0xF);
    buffered_frame[4] = int2asciiHex((id >> 12) & 0xF);
    buffered_frame[5] = int2asciiHex((id >> 8) & 0xF);
    buffered_frame[6] = int2asciiHex((id >> 4) & 0xF);
    buffered_frame[7] = int2asciiHex(id & 0xF);
    buffered_frame[8] = int2asciiHex(len);

    for(int i = 0; i <= len*2; i++)
    {
      buffered_frame[9+i] = (i % 2 == 1) ? int2asciiHex(cdata[i / 2] >> 4) : int2asciiHex(cdata[i / 2] & 0xF);
    }

    if(auto_poll)
    {
      slcanFrameTX(buffered_frame, 9 + len*2);
    }
    else
    {
      buffered_frame_len = 9 + len*2;
    }
  }
}

void slcanCheckRX() {
  if(Serial.available())
  {
	  byte received_char = Serial.read();
    if(received_char != 13)//Process the character received
    {
      rx_buffer[rx_buffer_len] = received_char;
      rx_buffer_len++;
      if(rx_buffer_len > 15) rx_buffer_len = 0; //Frame too long, destroying the buffer

      return;
    }

    if(rx_buffer_len < 1)//Frame too short, drop it
    {
      rx_buffer_len = 0;
      return;
    }

    slcanProcessRX(rx_buffer_len, rx_buffer);
    rx_buffer_len = 0;
  }
}

void slcanProcessRX(uint8_t rx_buffer_len, uint8_t *rx_buffer)
{
  if(rx_buffer[0] == 'S')
  {
    if(rx_buffer_len != 2) return slcanInvalidRX();
    if(CAN_active) return slcanInvalidRX();

    uint8_t speed = rx_buffer[1] - 0x30;

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
      default:
        slcanInvalidRX();
    }

    Serial.write(13);
  }

  else if(rx_buffer[0] == 'O')
  {
    if(CAN_active) return slcanInvalidRX();

    CAN_active = true;

    if(CAN.begin(MCP_ANY, CAN_speed, CAN_freq) != CAN_OK) return slcanInvalidRX();
    CAN.setMode(MCP_NORMAL);

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

  else if(rx_buffer[0] == 't')
  {
    if(!CAN_active) return slcanInvalidRX();

    uint16_t address = (asciiHex2int(rx_buffer[1]) << 8) + (asciiHex2int(rx_buffer[2]) << 4) + asciiHex2int(rx_buffer[3]);
    uint8_t length =   asciiHex2int(rx_buffer[1]);

    if(address > 0x7FF) return slcanInvalidRX();
    if(length > 8)      return slcanInvalidRX();
    if(rx_buffer_len != length * 2 + 5) return slcanInvalidRX();

    uint8_t buf[length] = {0};
    for(int i = 5; i < rx_buffer_len; i++)
    {
      buf[i] |= (i % 2 == 0) ? asciiHex2int(rx_buffer[i]) << 4 : asciiHex2int(rx_buffer[i]);
    }

    canTX(address, length, buf);

    Serial.write(13);
  }

  else if(rx_buffer[0] == 'T')
  {
    if(!CAN_active) return slcanInvalidRX();

    uint32_t address = ((uint32_t)asciiHex2int(rx_buffer[1]) << 28) +
      ((uint32_t)asciiHex2int(rx_buffer[2]) << 24) +
      ((uint32_t)asciiHex2int(rx_buffer[3]) << 20) +
      ((uint32_t)asciiHex2int(rx_buffer[4]) << 16) +
      ((uint32_t)asciiHex2int(rx_buffer[5]) << 12) +
      ((uint32_t)asciiHex2int(rx_buffer[6]) << 8) +
      ((uint32_t)asciiHex2int(rx_buffer[7]) << 4) +
      ((uint32_t)asciiHex2int(rx_buffer[8]) << 0);
    uint8_t length =   asciiHex2int(rx_buffer[9]);

    if(address > 0x1FFFFFFF) return slcanInvalidRX();
    if(length > 8)      return slcanInvalidRX();
    if(rx_buffer_len != length * 2 + 10) return slcanInvalidRX();

    uint8_t buf[length] = {0};
    for(int i = 10; i < rx_buffer_len; i++)
    {
      buf[i] |= (i % 2 == 1) ? asciiHex2int(rx_buffer[i]) << 4 : asciiHex2int(rx_buffer[i]);
    }

    canTX(address, length, buf);

    Serial.write(13);
  }

  else if(rx_buffer[0] == 'P' || rx_buffer[0] == 'A')
  {
    if(!CAN_active) return slcanInvalidRX();
    if(auto_poll) return slcanInvalidRX();

    slcanFrameTX(buffered_frame, buffered_frame_len);
    buffered_frame_len = 0;
    Serial.write(13);
  }

  else if(rx_buffer[0] == 'X')
  {
    if(rx_buffer_len != 2) return slcanInvalidRX();

    switch(asciiHex2int(rx_buffer[1]))
    {
      case 0:
        auto_poll = false;
        break;
      case 1:
        auto_poll = true;
        break;
      default:
        return slcanInvalidRX();
    }
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

  else if(rx_buffer[0] == 'F') return;
  else if(rx_buffer[0] == 'W') return;
  else if(rx_buffer[0] == 'M') return;
  else if(rx_buffer[0] == 'm') return;
  else if(rx_buffer[0] == 'U') return;
  else if(rx_buffer[0] == 'Q') return;
}

void slcanInvalidRX()
{
  Serial.write(7);
}

void slcanFrameTX(uint8_t *buf, uint8_t len)
{
  Serial.write('T');
  for(int i = 0; i < len; i++)
  {
    Serial.write(buf[i]);
  }
  Serial.write(13);
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
