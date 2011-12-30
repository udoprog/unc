#ifndef __UNC_HPP__
#define __UNC_HPP__

#include <string>
#include <vector>
#include <stdint.h>
#include <iostream>

#include <boost/foreach.hpp>
#include <exception>

namespace unc {
  typedef uint32_t codepoint_t;
  typedef std::basic_string<codepoint_t> ustring;

  const static codepoint_t INVALID_CODEPOINT = 0xffffffff;

  typedef enum encoding_t {
    codepoints,
    utf8,
    iso8859_1,
    iso8859_2,
    iso8859_3,
    iso8859_4,
    iso8859_5,
    iso8859_6,
    iso8859_7,
    iso8859_8,
    iso8859_9,
    iso8859_10,
    iso8859_11,
    iso8859_13,
    iso8859_14,
    iso8859_15,
    iso8859_16
  } encoding;

  struct utf8_fixed_state {
    /**
     * How many bytes to forward the reading.
     */
    size_t     forward;
    /**
     * The extracted codepoint.
     */
    union info_t {
      uint32_t codepoint;
      char     buffer[4];
    } info;
    /**
     * If the codepoint is valid.
     */
    bool       valid;
  };

  /**
   * Decode a fixed set of bytes one at a time.
   */
  void utf8_fixed_decode(utf8_fixed_state&, const uint8_t*, size_t);
  void utf8_fixed_encode(utf8_fixed_state&, uint32_t);

  template<encoding E>
  struct encoding_trait {
  };

  template<>
  struct encoding_trait<utf8> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
    static void encode_codepoint(codepoint_t&, std::string&);
    static int  compare(std::string&, std::string&);
  };

  template<>
  struct encoding_trait<codepoints> {
    static void encode(ustring&,     std::string&);
  };

  template<>
  struct encoding_trait<iso8859_1> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_2> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_3> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_4> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_5> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_6> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_7> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_8> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_9> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_10> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_11> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_13> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_14> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_15> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<>
  struct encoding_trait<iso8859_16> {
    static void decode(std::string&, ustring&);
    static void encode(ustring&, std::string&);
  };

  template<encoding E>
  ustring decode(std::string s)
  {
    ustring t;
    encoding_trait<E>::decode(s, t);
    return t;
  }

  template<encoding E>
  void decode(std::string s, ustring& t) {
    encoding_trait<E>::decode(s, t);
  };

  template<encoding E>
  std::string encode(ustring& s)
  {
    std::string t;
    encoding_trait<E>::encode(s, t);
    return t;
  }

  template<encoding E>
  void encode(ustring& s, std::string& t) {
    encoding_trait<E>::encode(s, t);
  };

  template<encoding E>
  void encode_codepoint(codepoint_t& codepoint, std::string& t) {
    encoding_trait<E>::encode_codepoint(codepoint, t);
  };

  template<encoding E>
  int compare(std::string& a, std::string& b) {
    return encoding_trait<E>::compare(a, b);
  };

  ustring lowercase(ustring&);
  ustring uppercase(ustring&);
};

#endif /* __NONSTD_HPP__ */
