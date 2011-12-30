#ifndef __UNC_CASE__
#define __UNC_CASE__
#include "stdint.h"

namespace unc {
  extern const uint32_t unc_case_max;
  static const int unc_case_uppercase = 0;
  static const int unc_case_lowercase = 1;
  extern const uint32_t unc_case_map[][2];
}
#endif /*__UNC_CASE__*/
