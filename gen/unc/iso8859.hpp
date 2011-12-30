#ifndef __ISO_8859__
#define __ISO_8859__
#include "stdint.h"

namespace unc {
  typedef uint16_t lowerplane_t ;
  struct iso_map_record {
    lowerplane_t key;
    lowerplane_t val;
  };
  /* map segment size, the map size is
   * (iso_map_size * rev_<name>_size) */
  static const int iso_map_size = 0x100;
  extern const lowerplane_t map_iso8859_1[iso_map_size];
  extern const int rev_iso8859_1_size;
  extern const iso_map_record rev_iso8859_1[];
  extern const lowerplane_t map_iso8859_10[iso_map_size];
  extern const int rev_iso8859_10_size;
  extern const iso_map_record rev_iso8859_10[];
  extern const lowerplane_t map_iso8859_11[iso_map_size];
  extern const int rev_iso8859_11_size;
  extern const iso_map_record rev_iso8859_11[];
  extern const lowerplane_t map_iso8859_13[iso_map_size];
  extern const int rev_iso8859_13_size;
  extern const iso_map_record rev_iso8859_13[];
  extern const lowerplane_t map_iso8859_14[iso_map_size];
  extern const int rev_iso8859_14_size;
  extern const iso_map_record rev_iso8859_14[];
  extern const lowerplane_t map_iso8859_15[iso_map_size];
  extern const int rev_iso8859_15_size;
  extern const iso_map_record rev_iso8859_15[];
  extern const lowerplane_t map_iso8859_16[iso_map_size];
  extern const int rev_iso8859_16_size;
  extern const iso_map_record rev_iso8859_16[];
  extern const lowerplane_t map_iso8859_2[iso_map_size];
  extern const int rev_iso8859_2_size;
  extern const iso_map_record rev_iso8859_2[];
  extern const lowerplane_t map_iso8859_3[iso_map_size];
  extern const int rev_iso8859_3_size;
  extern const iso_map_record rev_iso8859_3[];
  extern const lowerplane_t map_iso8859_4[iso_map_size];
  extern const int rev_iso8859_4_size;
  extern const iso_map_record rev_iso8859_4[];
  extern const lowerplane_t map_iso8859_5[iso_map_size];
  extern const int rev_iso8859_5_size;
  extern const iso_map_record rev_iso8859_5[];
  extern const lowerplane_t map_iso8859_6[iso_map_size];
  extern const int rev_iso8859_6_size;
  extern const iso_map_record rev_iso8859_6[];
  extern const lowerplane_t map_iso8859_7[iso_map_size];
  extern const int rev_iso8859_7_size;
  extern const iso_map_record rev_iso8859_7[];
  extern const lowerplane_t map_iso8859_8[iso_map_size];
  extern const int rev_iso8859_8_size;
  extern const iso_map_record rev_iso8859_8[];
  extern const lowerplane_t map_iso8859_9[iso_map_size];
  extern const int rev_iso8859_9_size;
  extern const iso_map_record rev_iso8859_9[];
}
#endif /*__ISO_8859__*/
