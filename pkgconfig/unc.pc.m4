prefix=PREFIX
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: UNC
Description: Lightweight C++ Unicode Library
Version: VERSION
Libs: -L${libdir} -lunc -luncdata
Cflags: -I${includedir}/unc
