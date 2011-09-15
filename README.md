UNC
===

UNC is a lightweight and simple unicode library.

I use it because i sometimes need something that just works for a specific set
of purposes.

Currently the following operations are supported.

 - UTF-8 encoding/decoding (thoroughly tested);
 - ISO-8859-X encoding/decoding (not so thorougly tested);
 - Non-localized string compare (compare by codepoint);
 - Non-localized uppercase/lowercase operations.

UNC handles errors by either dropping characters, or marking them as invalid.

Usage
=====

    #include <iostream>
    #include <string>
    #include <unc/unc.hpp>

    using namespace unc;
    using namespace std;

    int main() {
      /* the utf-8 encoded string "åäö" */
      string s("\xc3\xa5\xc3\xa4\xc3\xb6");
      ustring us = decode<utf8>(s);
      cout << us.size() << endl;                    // prints: 3
      ustring ucase = uppercase(us);
      cout << encode<utf8>(ucase) << endl;          // prints: ÅÄÖ
      cout << encode<codepoints>(ucase) << endl;    // prints: <U+00c5><U+00c4><U+00d6>
    }

If this is all you need, you are welcome to use UNC, it will only introduce
around 300K (compared to many megabytes for ICU).

But beware that ICU in contrast to unc is fully featured, and has been tested
a lot more.

Build/Install
=============
    #> make
    #> sudo make install

How to link the library
=======================
Currently the only built version is the one statically compiled, it is linked
by doing something similar to the following (if your system supports pkg-config and
has bash).

    #> g++ myapp.cpp $(pkg-config --libs --cflags unc) -o myapp

Windows
-------
Use mingw in an msys environment, it should contain everything you need in
order to compile. It might be necessary to install python and run the make
command like the following.

    #> make PYTHON=python.exe 

The script tools.py is used to parse the unicode files and generate the
necessary databases. It requires python.

Databases
=========
Currently, the following databases are generated.

 - ISO-8859 mapping in gen/iso8869.cpp;
 - Case code mapping in gen/unccase.cpp (for uppercase/lowercase).
