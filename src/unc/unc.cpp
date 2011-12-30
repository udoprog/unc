#include <unc/unc.hpp>
#include <unc/iso8859.hpp>
#include <unc/unccase.hpp>

#define CHECK_CODEPOINTS(state, data, size, limit) \
  do {                                             \
    for (size_t i = 1; i < limit; i++) {           \
      if (i >= s) {                                \
        state.forward = i;                         \
        goto reject;                               \
      }                                            \
                                                   \
      if (!__utf8_codepoint_valid(data[i]))        \
      {                                            \
        state.forward = i;                         \
        goto reject;                               \
      }                                            \
    }                                              \
                                                   \
    state.forward = limit;                         \
  } while(0);                                      \

#include <iomanip>
#include <sstream>

namespace unc {
  /**
   * Encode into a string representing the codepoints.
   */
  void encoding_trait<codepoints>::encode(ustring& source, std::string& target)
  {
    std::stringstream ss;

    BOOST_FOREACH(codepoint_t c, source) {
      if (c > 0x7f) {
        ss << "<U+" << std::hex << std::setw(4) << std::setfill('0') << c << ">";
      } else {
        ss << char(c);
      }
    }

    target.assign(ss.str());
  }

  /**
   * Check that a code point is valid by asserting that the bytes take the
   * following form.
   * 10xxxxxx
   */
  inline bool __utf8_codepoint_valid(const uint8_t p) {
    return (p >> 6) == 2;
  }

  void utf8_fixed_decode(utf8_fixed_state& state, const uint8_t* d, size_t s) 
  {
    codepoint_t c = INVALID_CODEPOINT;

    state.forward = 1;

    if (s == 0) {
      state.forward = 0;
      goto reject;
    }

    if (d[0] >> 7 == 0) {
      c = uint32_t(d[0]);
      goto accept;
    }

    if (d[0] >> 5 == 0x6) {
      CHECK_CODEPOINTS(state, d, s, 2);

      c =
          ( (d[0] & 0x3f ) << 6 )
        | ( (d[1] & 0x3f )      );

      goto accept;
    }

    if (d[0] >> 4 == 0xe) {
      CHECK_CODEPOINTS(state, d, s, 3);

      c =
          ( (d[0] & 0x0f) << 12 )
        | ( (d[1] & 0x3f) << 6  )
        | ( (d[2] & 0x3f)       );

      goto accept;
    }

    if (d[0] >> 3 == 0x1e) {
      CHECK_CODEPOINTS(state, d, s, 4);

      c =
          ( (d[0] & 0x07) << 18 )
        | ( (d[1] & 0x3f) << 12 )
        | ( (d[2] & 0x3f) << 6  )
        | ( (d[3] & 0x3f)       );

      goto accept;
    }

    if (d[0] >> 2 == 0x3e) {
      CHECK_CODEPOINTS(state, d, s, 5);

      c =
          ( (d[0] & 0x03) << 24 )
        | ( (d[1] & 0x3f) << 18 )
        | ( (d[2] & 0x3f) << 12 )
        | ( (d[3] & 0x3f) << 6  )
        | ( (d[4] & 0x3f)       );

      goto accept;
    }

    if (d[0] >> 1 == 0x7e) {
      CHECK_CODEPOINTS(state, d, s, 6);

      c =
          ( (d[0] & 0x01) << 30 )
        | ( (d[1] & 0x3f) << 24 )
        | ( (d[2] & 0x3f) << 18 )
        | ( (d[3] & 0x3f) << 12 )
        | ( (d[4] & 0x3f) << 6  )
        | ( (d[5] & 0x3f)       );

      goto accept;
    }

    goto reject;

accept:
    switch (state.forward) {
    case 2:
      if (c <= 0x7f) {
        goto reject;
      }
      break;
    case 3:
      if (c <= 0x7ff) {
        goto reject;
      }

      /* high surrogates */
      if (c >= 0xd800 && c <= 0xdbff) {
        goto reject;
      }

      /* low surrogates */
      if (c >= 0xdc00 && c <= 0xdfff) {
        goto reject;
      }

      /* invalid code points */
      if (c == 0xfffe) {
        goto reject;
      }

      if (c == 0xffff) {
        goto reject;
      }
      break;
    case 4:
      if (c <= 0xffff) {
        goto reject;
      }
      break;
    case 5:
      if (c <= 0x1fffff) {
        goto reject;
      }
      break;
    case 6:
      if (c <= 0x3ffffff) {
        goto reject;
      }
      break;
    };

    state.info.codepoint = c;
    state.valid = true;
    return;
reject:
    state.info.codepoint = INVALID_CODEPOINT;
    state.valid = false;
    return;
  }

  void utf8_fixed_encode(utf8_fixed_state& state, uint32_t v)
  {
    state.valid = true;
    state.forward = 0;

    if (v <= 0x7f) {
      state.info.buffer[0] = char(v);
      state.forward = 1;
    }
    else if (v <= 0x07ff) {
      state.info.buffer[0] = uint8_t(0xc0 | ((v >> 6) & 0x1f));
      state.info.buffer[1] = uint8_t(0x80 | (v & 0x3f));
      state.forward = 2;
    }
    else if (v <= 0xffff) {
      state.info.buffer[0] = uint8_t(0xe0 | ((v >> 12) & 0x0f));
      state.info.buffer[1] = uint8_t(0x80 | ((v >> 6) & 0x3f));
      state.info.buffer[2] = uint8_t(0x80 | (v & 0x3f));
      state.forward = 3;
    }
    else if (v <= 0x10ffff) {
      state.info.buffer[0] = uint8_t(0xf0 | ((v >> 18) & 0x05));
      state.info.buffer[1] = uint8_t(0x80 | ((v >> 12) & 0x3f));
      state.info.buffer[2] = uint8_t(0x80 | ((v >> 6) & 0x3f));
      state.info.buffer[3] = uint8_t(0x80 | (v & 0x3f));
      state.forward = 4;
    }
    else {
      state.valid = false;
    }
  }

  /*
   * c++ efficient encoding trait for utf8.
   **/

  void encoding_trait<utf8>::decode(std::string& source, ustring& target)
  {
    const uint8_t* data = (uint8_t*)source.data();
    size_t length = source.length();
    size_t i = 0;

    utf8_fixed_state state;

    while (i < length) {
      utf8_fixed_decode(state, data + i, length - i);
      i += state.forward;
      target.push_back(state.info.codepoint);
    }
  }

  void encoding_trait<utf8>::encode(ustring& source, std::string& target)
  {
    utf8_fixed_state state;

    BOOST_FOREACH(uint32_t v, source) {
      utf8_fixed_encode(state, v);

      if (!state.valid) {
        continue;
      }

      for (size_t i = 0; i < state.forward; i++) {
        target.push_back(state.info.buffer[i]);
      }
    }
  }

  void encoding_trait<utf8>::encode_codepoint(codepoint_t& codepoint, std::string& target)
  {
    utf8_fixed_state state;

    utf8_fixed_encode(state, codepoint);

    if (!state.valid) {
      return;
    }

    for (size_t i = 0; i < state.forward; i++) {
      target.push_back(state.info.buffer[i]);
    }
  }

  int encoding_trait<utf8>::compare(std::string& a, std::string& b)
  {
    const uint8_t* data_a = (uint8_t*)a.data();
    const uint8_t* data_b = (uint8_t*)b.data();

    size_t length = std::min(a.length(), b.length());
    size_t i = 0;

    utf8_fixed_state state_a;
    utf8_fixed_state state_b;

    while (i < length) {
      utf8_fixed_decode(state_a, data_a + i, length - i);
      utf8_fixed_decode(state_b, data_b + i, length - i);

      if (!state_a.valid && state_b.valid) {
        return 1;
      }

      if (state_a.valid && !state_b.valid) {
        return -1;
      }

      if (state_a.info.codepoint > state_b.info.codepoint) {
        return -1;
      }

      if (state_a.info.codepoint < state_b.info.codepoint) {
        return 1;
      }

      i += state_a.forward;
    }

    if (a.length() > b.length()) {
      return -1;
    }

    if (a.length() < b.length()) {
      return 1;
    }

    return 0;
  }

  inline void decode_iso8859(const uint16_t* map, std::string& source, ustring& target)
  {
    BOOST_FOREACH(char c, source) {
      target.push_back(map[(uint8_t)c]);
    }
  }

  inline void encode_iso8859(const iso_map_record rev[], int max, ustring& source, std::string& target)
  {
    BOOST_FOREACH(codepoint_t c, source) {
      /* no iso8859 characters are above this codepoint */
      if (c > 0xffff) {
        continue;
      }

      /* downcast for future key check */
      uint16_t c16 = uint16_t(c);

      /* cheap hashing, 00000000 11111111 */
      int b        = c16 & 0xff;

      for (int i = 1; i <= max; i++) {
        /* hash key is an offset in the reverse lookup map */
        int k = i*b;

        /* sanity check, characters out of range will be dropped */
        if (k > max * iso_map_size) {
          break;
        }

        if (rev[k].key == c16) {
          target.push_back(rev[k].val);
          break;
        }
      }
    }
  }

  inline void unc_case_index(int idx, ustring& source, ustring& target)
  {
    BOOST_FOREACH(codepoint_t c, source) {
      if (c > unc_case_max) {
        target.push_back(c);
        continue;
      }

      const uint32_t* cases = unc_case_map[c];

      if (cases[idx] == 0x0) {
        target.push_back(c);
        continue;
      }

      target.push_back(cases[idx]);
    }
  }

  ustring lowercase(ustring& source)
  {
    ustring target;
    unc_case_index(unc_case_lowercase, source, target);
    return target;
  }

  ustring uppercase(ustring& source)
  {
    ustring target;
    unc_case_index(unc_case_uppercase, source, target);
    return target;
  }

  void encoding_trait<iso8859_1>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_1, source, target); }
  void encoding_trait<iso8859_1>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_1, rev_iso8859_1_size, source, target); }

  void encoding_trait<iso8859_2>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_2, source, target); }
  void encoding_trait<iso8859_2>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_2, rev_iso8859_2_size, source, target); }

  void encoding_trait<iso8859_3>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_3, source, target); }
  void encoding_trait<iso8859_3>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_3, rev_iso8859_3_size, source, target); }

  void encoding_trait<iso8859_4>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_4, source, target); }
  void encoding_trait<iso8859_4>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_4, rev_iso8859_4_size, source, target); }

  void encoding_trait<iso8859_5>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_5, source, target); }
  void encoding_trait<iso8859_5>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_5, rev_iso8859_5_size, source, target); }

  void encoding_trait<iso8859_6>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_6, source, target); }
  void encoding_trait<iso8859_6>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_6, rev_iso8859_6_size, source, target); }

  void encoding_trait<iso8859_7>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_7, source, target); }
  void encoding_trait<iso8859_7>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_7, rev_iso8859_7_size, source, target); }

  void encoding_trait<iso8859_8>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_8, source, target); }
  void encoding_trait<iso8859_8>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_8, rev_iso8859_8_size, source, target); }

  void encoding_trait<iso8859_9>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_9, source, target); }
  void encoding_trait<iso8859_9>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_9, rev_iso8859_9_size, source, target); }

  void encoding_trait<iso8859_10>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_10, source, target); }
  void encoding_trait<iso8859_10>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_10, rev_iso8859_10_size, source, target); }

  void encoding_trait<iso8859_11>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_11, source, target); }
  void encoding_trait<iso8859_11>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_11, rev_iso8859_11_size, source, target); }

  void encoding_trait<iso8859_13>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_13, source, target); }
  void encoding_trait<iso8859_13>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_13, rev_iso8859_13_size, source, target); }

  void encoding_trait<iso8859_14>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_14, source, target); }
  void encoding_trait<iso8859_14>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_14, rev_iso8859_14_size, source, target); }

  void encoding_trait<iso8859_15>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_15, source, target); }
  void encoding_trait<iso8859_15>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_15, rev_iso8859_15_size, source, target); }

  void encoding_trait<iso8859_16>::decode(std::string& source, ustring& target)
  { decode_iso8859(map_iso8859_16, source, target); }
  void encoding_trait<iso8859_16>::encode(ustring& source, std::string& target)
  { encode_iso8859(rev_iso8859_16, rev_iso8859_16_size, source, target); }
};
