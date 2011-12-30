#include <unc/unc.hpp>
#define BOOST_TEST_MODULE Utf8Tests
#include <boost/test/unit_test.hpp>

/**
 * This test suite is based on the following documents.
 * - http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt
 */

#define CHECK_FIRST(str, ref)                       \
  {                                                 \
    string __s(str);                                \
    ustring __u = decode<utf8>(__s); \
    BOOST_REQUIRE(__u.size() == 1);                 \
    BOOST_CHECK(__u.at(0) == ref);                  \
  }

#define CHECK_ALL(str, total, ref)                  \
  {                                                 \
    string __s(str);                                \
    ustring __u = decode<utf8>(__s); \
    BOOST_REQUIRE(__u.size() == total);             \
    for (int i = 0; i < total; i++) {               \
      BOOST_CHECK(__u.at(0) == ref);                \
    }                                               \
  }

using namespace unc;
using namespace std;

BOOST_AUTO_TEST_CASE(test_simple)
{
  string s("\xc3\xa5\xc3\xa4\xc3\xb6");
  ustring u = decode<utf8>(s);
  BOOST_CHECK(u.length() == 3);
  BOOST_CHECK(u.at(0) == 229);
  BOOST_CHECK(u.at(1) == 228);
  BOOST_CHECK(u.at(2) == 246);
}

// 2.1  First possible sequence of a certain length
BOOST_AUTO_TEST_CASE(test_1)
{
  string s("\xce\xba\xe1\xbd\xb9\xcf\x83\xce\xbc\xce\xb5");
  ustring u = decode<utf8>(s);
  BOOST_CHECK(u.size() == 5);
  BOOST_CHECK(u.at(0) == 954);
  BOOST_CHECK(u.at(1) == 8057);
  BOOST_CHECK(u.at(2) == 963);
  BOOST_CHECK(u.at(3) == 956);
  BOOST_CHECK(u.at(4) == 949);
}

// 2.2  Last possible sequence of a certain length
BOOST_AUTO_TEST_CASE(test_2_2)
{
  CHECK_FIRST("\x7f", 0x7f);
  CHECK_FIRST("\xdf\xbf", 0x7ff);
  CHECK_FIRST("\xef\xbf\xbf", INVALID_CODEPOINT);
  CHECK_FIRST("\xf7\xbf\xbf\xbf", 0x001fffff);
  CHECK_FIRST("\xfb\xbf\xbf\xbf\xbf", 0x03ffffff);
  CHECK_FIRST("\xfd\xbf\xbf\xbf\xbf\xbf", 0x7fffffff);
}

// 2.3  Other boundary conditions
BOOST_AUTO_TEST_CASE(test_2_3)
{
  CHECK_FIRST("\xed\x9f\xbf", 0xd7ff);
  CHECK_FIRST("\xee\x80\x80", 0xe000);
  CHECK_FIRST("\xef\xbf\xbd", 0xfffd);
  CHECK_FIRST("\xf4\x8f\xbf\xbf", 0x10ffff);
  CHECK_FIRST("\xf4\x90\x80\x80", 0x110000);
}

// 3.1  Unexpected continuation bytes
BOOST_AUTO_TEST_CASE(test_3_1)
{
  CHECK_FIRST("\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xbf", INVALID_CODEPOINT);
  CHECK_ALL("\x80\xbf", 2, INVALID_CODEPOINT);
  CHECK_ALL("\x80\xbf\x80", 3, INVALID_CODEPOINT);
  CHECK_ALL("\x80\xbf\x80\xbf", 4, INVALID_CODEPOINT);
  CHECK_ALL("\x80\xbf\x80\xbf\x80", 5, INVALID_CODEPOINT);
  CHECK_ALL("\x80\xbf\x80\xbf\x80\xbf", 6, INVALID_CODEPOINT);
  CHECK_ALL(
      "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
      "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
      "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
      "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf",
      64,
      INVALID_CODEPOINT
  );
}

// 3.2  Lonely start characters
BOOST_AUTO_TEST_CASE(test_3_2)
{
  // 3.2.1  All 32 first bytes of 2-byte sequences (0xc0-0xdf),
  //        each followed by a space character:
  {
    const char* s = 
      "\xc0\x20\xc1\x20\xc2\x20\xc3\x20\xc4\x20\xc5\x20\xc6\x20\xc7\x20"
      "\xc8\x20\xc9\x20\xca\x20\xcb\x20\xcc\x20\xcd\x20\xce\x20\xcf\x20"
      "\xd0\x20\xd1\x20\xd2\x20\xd3\x20\xd4\x20\xd5\x20\xd6\x20\xd7\x20"
      "\xd8\x20\xd9\x20\xda\x20\xdb\x20\xdc\x20\xdd\x20\xde\x20\xdf\x20";

    ustring u = decode<utf8>(s);
    BOOST_REQUIRE(u.size() == 64);

    for (int i = 0; i < 64; i += 2) {
      BOOST_CHECK(u.at(i) == INVALID_CODEPOINT);
      BOOST_CHECK(u.at(i+1) == 0x20);
    }
  }

  // 3.2.2  All 16 first bytes of 3-byte sequences (0xe0-0xef),
  //        each followed by a space character:
  {
    const char* s = 
      "\xe0\x20\xe1\x20\xe2\x20\xe3\x20\xe4\x20\xe5\x20\xe6\x20\xe7\x20"
      "\xe8\x20\xe9\x20\xea\x20\xeb\x20\xec\x20\xed\x20\xee\x20\xef\x20";

    ustring u = decode<utf8>(s);
    BOOST_REQUIRE(u.size() == 32);

    for (int i = 0; i < 32; i += 2) {
      BOOST_CHECK(u.at(i) == INVALID_CODEPOINT);
      BOOST_CHECK(u.at(i+1) == 0x20);
    }
  }
  // 3.2.3  All 8 first bytes of 4-byte sequences (0xf0-0xf7),
  //        each followed by a space character:
  {
    const char* s = 
      "\xf0\x20\xf1\x20\xf2\x20\xf3\x20\xf4\x20\xf5\x20\xf6\x20\xf7\x20";

    ustring u = decode<utf8>(s);
    BOOST_REQUIRE(u.size() == 16);

    for (int i = 0; i < 16; i += 2) {
      BOOST_CHECK(u.at(i) == INVALID_CODEPOINT);
      BOOST_CHECK(u.at(i+1) == 0x20);
    }
  }
  // 3.2.4  All 4 first bytes of 5-byte sequences (0xf8-0xfb),
  //        each followed by a space character:
  {
    const char* s = 
      "\xf8\x20\xf9\x20\xfa\x20\xfb\x20";

    ustring u = decode<utf8>(s);
    BOOST_REQUIRE(u.size() == 8);

    for (int i = 0; i < 8; i += 2) {
      BOOST_CHECK(u.at(i) == INVALID_CODEPOINT);
      BOOST_CHECK(u.at(i+1) == 0x20);
    }
  }
  // 3.2.5  All 2 first bytes of 6-byte sequences (0xfc-0xfd),
  //        each followed by a space character:
  {
    const char* s = 
      "\xfc\x20\xfd\x20";

    ustring u = decode<utf8>(s);
    BOOST_REQUIRE(u.size() == 4);

    for (int i = 0; i < 4; i += 2) {
      BOOST_CHECK(u.at(i) == INVALID_CODEPOINT);
      BOOST_CHECK(u.at(i+1) == 0x20);
    }
  }
}

// 3.3  Sequences with last continuation byte missing
BOOST_AUTO_TEST_CASE(test_3_3)
{
  CHECK_FIRST("\xc0", INVALID_CODEPOINT);
  CHECK_FIRST("\xe0\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xf0\x80\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xf8\x80\x80\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xfc\x80\x80\x80\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xdf", INVALID_CODEPOINT);
  CHECK_FIRST("\xef\xbf", INVALID_CODEPOINT);
  CHECK_FIRST("\xf7\xbf\xbf", INVALID_CODEPOINT);
  CHECK_FIRST("\xfb\xbf\xbf\xbf", INVALID_CODEPOINT);
  CHECK_FIRST("\xfd\xbf\xbf\xbf\xbf", INVALID_CODEPOINT);
}

// 3.4  Concatenation of incomplete sequences
BOOST_AUTO_TEST_CASE(test_3_4)
{
  CHECK_ALL("\xc0\xe0\x80\xf0\x80\x80\xf8\x80\x80\x80\xfc\x80\x80\x80\x80\xdf"
           "\xef\xbf\xf7\xbf\xbf\xfb\xbf\xbf\xbf\xfd\xbf\xbf\xbf\xbf",
           10,
           INVALID_CODEPOINT);

}

// 3.5  Impossible bytes
BOOST_AUTO_TEST_CASE(test_3_5)
{
  CHECK_FIRST("\xfe", INVALID_CODEPOINT);
  CHECK_FIRST("\xff", INVALID_CODEPOINT);
  CHECK_ALL("\xfe\xfe\xff\xff", 4, INVALID_CODEPOINT);
}

// 4.1  Examples of an overlong ASCII character
BOOST_AUTO_TEST_CASE(test_4_1)
{
  CHECK_FIRST("\xc0\xaf", INVALID_CODEPOINT);
  CHECK_FIRST("\xe0\x80\xaf", INVALID_CODEPOINT);
  CHECK_FIRST("\xf0\x80\x80\xaf", INVALID_CODEPOINT);
  CHECK_FIRST("\xf8\x80\x80\x80\xaf", INVALID_CODEPOINT);
  CHECK_FIRST("\xfc\x80\x80\x80\x80\xaf", INVALID_CODEPOINT);
}

// 4.2  Maximum overlong sequences
BOOST_AUTO_TEST_CASE(test_4_2)
{
  CHECK_FIRST("\xc1\xbf", INVALID_CODEPOINT);
  CHECK_FIRST("\xe0\x9f\xbf", INVALID_CODEPOINT);
  CHECK_FIRST("\xf0\x8f\xbf\xbf", INVALID_CODEPOINT);
  CHECK_FIRST("\xf8\x87\xbf\xbf\xbf", INVALID_CODEPOINT);
  CHECK_FIRST("\xfc\x83\xbf\xbf\xbf\xbf", INVALID_CODEPOINT);
}

// 4.3  Overlong representation of the NUL character
BOOST_AUTO_TEST_CASE(test_4_3)
{
  CHECK_FIRST("\xc0\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xe0\x80\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xf0\x80\x80\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xf8\x80\x80\x80\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xfc\x80\x80\x80\x80\x80", INVALID_CODEPOINT);
}

// 5.1 Single UTF-16 surrogates
BOOST_AUTO_TEST_CASE(test_5_1)
{
  CHECK_FIRST("\xed\xa0\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xed\xad\xbf", INVALID_CODEPOINT);
  CHECK_FIRST("\xed\xae\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xed\xaf\xbf", INVALID_CODEPOINT);
  CHECK_FIRST("\xed\xb0\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xed\xbe\x80", INVALID_CODEPOINT);
  CHECK_FIRST("\xed\xbf\xbf", INVALID_CODEPOINT);
}

// 5.2 Paired UTF-16 surrogates
BOOST_AUTO_TEST_CASE(test_5_2)
{
  CHECK_ALL("\xed\xa0\x80\xed\xb0\x80", 2, INVALID_CODEPOINT);
  CHECK_ALL("\xed\xa0\x80\xed\xbf\xbf", 2, INVALID_CODEPOINT);
  CHECK_ALL("\xed\xad\xbf\xed\xb0\x80", 2, INVALID_CODEPOINT);
  CHECK_ALL("\xed\xad\xbf\xed\xbf\xbf", 2, INVALID_CODEPOINT);
  CHECK_ALL("\xed\xae\x80\xed\xb0\x80", 2, INVALID_CODEPOINT);
  CHECK_ALL("\xed\xae\x80\xed\xbf\xbf", 2, INVALID_CODEPOINT);
  CHECK_ALL("\xed\xaf\xbf\xed\xb0\x80", 2, INVALID_CODEPOINT);
  CHECK_ALL("\xed\xaf\xbf\xed\xbf\xbf", 2, INVALID_CODEPOINT);
}

// 5.3 Other illegal code positions
BOOST_AUTO_TEST_CASE(test_5_3)
{
  CHECK_FIRST("\xef\xbf\xbe", INVALID_CODEPOINT);
  CHECK_FIRST("\xef\xbf\xbe", INVALID_CODEPOINT);
}

BOOST_AUTO_TEST_CASE(test_compare)
{
  std::string a("abd");
  std::string b("abc");
  std::string inv("ab\xef\xbf\xbe");

  BOOST_CHECK(compare<utf8>(a, b) == -1);
  BOOST_CHECK(compare<utf8>(b, a) == 1);
  BOOST_CHECK(compare<utf8>(a, a) == 0);
  BOOST_CHECK(compare<utf8>(a, inv) == -1);
  BOOST_CHECK(compare<utf8>(inv, a) == 1);
}

BOOST_AUTO_TEST_CASE(test_lowercase)
{
  string s("\xc3\xa5\xc3\xa4\xc3\xb6");
  ustring us = decode<utf8>(s);
  ustring u = lowercase(us);
  BOOST_CHECK(u.length() == 3);
  BOOST_CHECK(u.at(0) == 229);
  BOOST_CHECK(u.at(1) == 228);
  BOOST_CHECK(u.at(2) == 246);
  ustring u2 = uppercase(u);
  BOOST_CHECK(u2.at(0) == 0xc5);
  BOOST_CHECK(u2.at(1) == 0xc4);
  BOOST_CHECK(u2.at(2) == 0xd6);
}

