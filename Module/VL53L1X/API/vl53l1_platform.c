
/* 
* This file is part of VL53L1 Platform 
* 
* Copyright (c) 2016, STMicroelectronics - All Rights Reserved 
* 
* License terms: BSD 3-clause "New" or "Revised" License. 
* 
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are met: 
* 
* 1. Redistributions of source code must retain the above copyright notice, this 
* list of conditions and the following disclaimer. 
* 
* 2. Redistributions in binary form must reproduce the above copyright notice, 
* this list of conditions and the following disclaimer in the documentation 
* and/or other materials provided with the distribution. 
* 
* 3. Neither the name of the copyright holder nor the names of its contributors 
* may be used to endorse or promote products derived from this software 
* without specific prior written permission. 
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
* 
*/

#include "vl53l1_platform.h"
#include "DELAY/delay.h"

SoftI2C_Port VL53L1X_port;

VL53L1X_ERROR VL53L1_WriteMulti(VL53L1_DEV dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
  if (SoftI2C_16Write(&VL53L1X_port, dev, index, pdata, count))
    return VL53L1X_ERROR_NONE;
  else
    return VL53L1X_ERROR_ERROR;
}

VL53L1X_ERROR VL53L1_ReadMulti(VL53L1_DEV dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
  if (SoftI2C_16Read(&VL53L1X_port, dev, index, pdata, count))
    return VL53L1X_ERROR_NONE;
  else
    return VL53L1X_ERROR_ERROR;
}

VL53L1X_ERROR VL53L1_WrByte(VL53L1_DEV dev, uint16_t index, uint8_t data)
{
  return VL53L1_WriteMulti(dev, index, &data, 1);
}

VL53L1X_ERROR VL53L1_WrWord(VL53L1_DEV dev, uint16_t index, uint16_t data)
{
  uint8_t buffer[2] = {data >> 8, data & 0xFF};
  return VL53L1_WriteMulti(dev, index, buffer, 2);
}

VL53L1X_ERROR VL53L1_WrDWord(VL53L1_DEV dev, uint16_t index, uint32_t data)
{
  uint8_t buffer[4] = {(data >> 24) & 0xFF, (data >> 16) & 0xFF, (data >> 8) & 0xFF, data & 0xFF};
  return VL53L1_WriteMulti(dev, index, buffer, 4);
}

VL53L1X_ERROR VL53L1_RdByte(VL53L1_DEV dev, uint16_t index, uint8_t *data)
{
  return VL53L1_ReadMulti(dev, index, data, 1);
}

VL53L1X_ERROR VL53L1_RdWord(VL53L1_DEV dev, uint16_t index, uint16_t *data)
{
  VL53L1X_ERROR Status = VL53L1X_ERROR_NONE;
  uint8_t buffer[2];
  Status = VL53L1_ReadMulti(dev, index, buffer, 2);
  if (Status != VL53L1X_ERROR_NONE)
    return Status;
  *data = ((uint16_t)buffer[0] << 8) + (uint16_t)buffer[1];
  return Status;
}

VL53L1X_ERROR VL53L1_RdDWord(VL53L1_DEV dev, uint16_t index, uint32_t *data)
{
  VL53L1X_ERROR Status = VL53L1X_ERROR_NONE;
  uint8_t buffer[4];
  Status = VL53L1_ReadMulti(dev, index, buffer, 4);
  if (Status != VL53L1X_ERROR_NONE)
    return Status;
  *data = ((uint32_t)buffer[0] << 24) + ((uint32_t)buffer[1] << 16) + ((uint32_t)buffer[2] << 8) + (uint32_t)buffer[3];
  return Status;
}

VL53L1X_ERROR VL53L1_WaitMs(VL53L1_DEV dev, int32_t wait_ms)
{
  Delay_ms(1);
  return VL53L1X_ERROR_NONE;
}
