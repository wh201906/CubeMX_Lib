/**
 * @file  vl53l1_platform.h
 * @brief Those platform functions are platform dependent and have to be implemented by the user
 */

#ifndef _VL53L1_PLATFORM_H_
#define _VL53L1_PLATFORM_H_

#include "vl53l1_types.h"
#include "I2C/softi2c.h"

typedef uint8_t VL53L1_Dev_t;
typedef VL53L1_Dev_t VL53L1_DEV;

typedef int8_t VL53L1X_ERROR;
#define VL53L1X_ERROR_NONE ((VL53L1X_ERROR)0)
#define VL53L1X_ERROR_ERROR ((VL53L1X_ERROR)-99)

extern SoftI2C_Port VL53l1X_port;

#ifdef __cplusplus
extern "C"
{
#endif

  /** @brief VL53L1_WriteMulti() definition.\n
 * To be implemented by the developer
 */
  VL53L1X_ERROR VL53L1_WriteMulti(
      VL53L1_DEV dev,
      uint16_t index,
      uint8_t *pdata,
      uint32_t count);
  /** @brief VL53L1_ReadMulti() definition.\n
 * To be implemented by the developer
 */
  VL53L1X_ERROR VL53L1_ReadMulti(
      VL53L1_DEV dev,
      uint16_t index,
      uint8_t *pdata,
      uint32_t count);
  /** @brief VL53L1_WrByte() definition.\n
 * To be implemented by the developer
 */
  VL53L1X_ERROR VL53L1_WrByte(
      VL53L1_DEV dev,
      uint16_t index,
      uint8_t data);
  /** @brief VL53L1_WrWord() definition.\n
 * To be implemented by the developer
 */
  VL53L1X_ERROR VL53L1_WrWord(
      VL53L1_DEV dev,
      uint16_t index,
      uint16_t data);
  /** @brief VL53L1_WrDWord() definition.\n
 * To be implemented by the developer
 */
  VL53L1X_ERROR VL53L1_WrDWord(
      VL53L1_DEV dev,
      uint16_t index,
      uint32_t data);
  /** @brief VL53L1_RdByte() definition.\n
 * To be implemented by the developer
 */
  VL53L1X_ERROR VL53L1_RdByte(
      VL53L1_DEV dev,
      uint16_t index,
      uint8_t *pdata);
  /** @brief VL53L1_RdWord() definition.\n
 * To be implemented by the developer
 */
  VL53L1X_ERROR VL53L1_RdWord(
      VL53L1_DEV dev,
      uint16_t index,
      uint16_t *pdata);
  /** @brief VL53L1_RdDWord() definition.\n
 * To be implemented by the developer
 */
  VL53L1X_ERROR VL53L1_RdDWord(
      VL53L1_DEV dev,
      uint16_t index,
      uint32_t *pdata);
  /** @brief VL53L1_WaitMs() definition.\n
 * To be implemented by the developer
 */
  VL53L1X_ERROR VL53L1_WaitMs(
      VL53L1_DEV dev,
      int32_t wait_ms);

#ifdef __cplusplus
}
#endif

#endif
