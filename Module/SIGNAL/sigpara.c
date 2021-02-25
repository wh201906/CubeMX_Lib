#include "sigpara.h"

float32_t SigPara_RMS(const float32_t* data, uint32_t len)
{
  float32_t res;
  arm_rms_f32(data, len, &res);
  return res;
}