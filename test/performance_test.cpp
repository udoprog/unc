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

#include <fstream>
#include <boost/progress.hpp>

using namespace unc;
using namespace std;

BOOST_AUTO_TEST_CASE(test_simple)
{
  ifstream fs("test/testdata.blob");

  string s;

  char buffer[1024 * 1024];

  while (!fs.eof()) {
    fs.read(buffer, 1024 * 1024);
    s.append(buffer, fs.gcount());
  }

  double total;
  int counter;

  while (counter++ < 100) {
    boost::timer t;
    ustring u;
    decode<utf8>(s, u);
    total += t.elapsed();
  }

  std::cout << "Average " << (total / counter) << " seconds" << std::endl;
}
