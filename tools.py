#!/usr/bin/env python
#
# Auxilliary tools for creating the different unicode databases.
#
# These are invoked from the Makefile.
#
import sys

# populate to add runnable command
commands = dict()

def tool(**kw):
  def tdec(f):
    global commands
    def dec(args):
      if f.__code__.co_argcount != len(args):
        print >>sys.stderr, "expected %d arguments"%(f.__code__.co_argcount)
        return 1
      return f(*args)
    commands[kw.get("name", f.func_name)] = dec
    return dec
  return tdec

@tool(name="unicodedata")
def unicodedata(datafile, src, hdr, inc, ns):
  src_fp=open(src, 'w')
  hdr_fp=open(hdr, 'w')

  def readfile(path):
    fp = open(path)

    for line in fp:
      if line.strip() == "":
        continue
      if line.startswith("#"):
        continue
      line = line.strip()
      yield line.split(";")

  mx = 0x10ffff
  mv = 0

  ll = list([(0,0)] * mx)

  for i,line in enumerate(readfile(datafile)):
    codep = int(line[0], 16)
    upper = line[12]
    lower = line[13]

    if upper == "":
      upper = 0x0
    else:
      upper = int(upper, 16)
      mv = i

    if lower == "":
      lower = 0x0
    else:
      lower = int(lower, 16)
      mv = i

    if codep > mx:
      raise RuntimeError, "Breached maximum code point: " + str(codep)

    ll[codep] = (upper, lower)

  print >>src_fp, "#include \"%s\""%(inc)
  print >>src_fp, ""
  print >>src_fp, "namespace %s {"%(ns)
  print >>src_fp, "  const uint32_t unc_case_max = %d;"%(mv)
  print >>src_fp, "  const uint32_t unc_case_map[][2] = {"
  for i,(upc,loc) in enumerate(ll):
    if i > mv: break
    if i != mv:
      print >>src_fp, "    {0x%04x, 0x%04x},"%(upc,loc)
    else:
      print >>src_fp, "    {0x%04x, 0x%04x}"%(upc,loc)
  print >>src_fp, "  };"
  print >>src_fp, "}"

  print >>hdr_fp, "#ifndef __UNC_CASE__"
  print >>hdr_fp, "#define __UNC_CASE__"
  print >>hdr_fp, "#include \"stdint.h\""
  print >>hdr_fp, ""
  print >>hdr_fp, "namespace %s {"%(ns)
  print >>hdr_fp, "  extern const uint32_t unc_case_max;"
  print >>hdr_fp, "  static const int unc_case_uppercase = 0;"
  print >>hdr_fp, "  static const int unc_case_lowercase = 1;"
  print >>hdr_fp, "  extern const uint32_t unc_case_map[][2];"
  print >>hdr_fp, "}"
  print >>hdr_fp, "#endif /*__UNC_CASE__*/"

  return 0

@tool(name="specs")
def specs(src, hdr, inc, ns):
  files = {
    'iso8859_1': "specs/8859-1.TXT",
    'iso8859_2': "specs/8859-2.TXT",
    'iso8859_3': "specs/8859-3.TXT",
    'iso8859_4': "specs/8859-4.TXT",
    'iso8859_5': "specs/8859-5.TXT",
    'iso8859_6': "specs/8859-6.TXT",
    'iso8859_7': "specs/8859-7.TXT",
    'iso8859_8': "specs/8859-8.TXT",
    'iso8859_9': "specs/8859-9.TXT",
    'iso8859_10': "specs/8859-10.TXT",
    'iso8859_11': "specs/8859-11.TXT",
    'iso8859_13': "specs/8859-13.TXT",
    'iso8859_14': "specs/8859-14.TXT",
    'iso8859_15': "specs/8859-15.TXT",
    'iso8859_16': "specs/8859-16.TXT"
  };

  src_fp=open(src, 'w')
  hdr_fp=open(hdr, 'w')

  def readfile(path, total=0xff):
    fp = open(path)

    for line in fp:
      if line.strip() == "":
        continue
      if line.startswith("#"):
        continue
      k, v = line.split("\t")[0:2]
      k = int(k, 16)
      v = int(v, 16)
      if k > total:
        raise RuntimeError, "total not big enough"
      yield k,v

  print >>src_fp, "#include \"%s\""%(inc)
  print >>src_fp, ""
  print >>src_fp, "namespace %s {"%(ns)

  for k,f in sorted(files.items()):
    m = dict([(i,0) for i in range(0x100)])
    for kk,vv in readfile(f):
      m[kk] = vv
    print >>src_fp, "  const uint16_t map_%s[iso_map_size] = {"%(k)
    r = sorted(m.items())
    print >>src_fp, "    " +\
      ",\n    ".join(["  " +\
        ", ".join(["0x%04X"%(tc)\
          for (sc, tc) in r[i:i+8]])\
            for i in range(0, 0xff, 8)]);

    print >>src_fp, "  };"

  for k,f in sorted(files.items()):
    mx = 0
    m = dict([(i,[]) for i in range(0x100)])
    for kk,vv in readfile(f):
      a = m[vv % 0x100]
      a.append((kk,vv))
      mx = max(len(a), mx)

    for kk,vv in list(m.items()):
      m[kk] = vv + [(0,0)] * (mx - len(vv))

    nm = list()
    for i in range(mx):
      for kk,vv in sorted(m.items()):
        nm.append(vv[i])

    print >>src_fp, "  const int rev_%s_size = %d;"%(k,mx)
    print >>src_fp, "  const iso_map_record rev_%s[] = {"%(k)
    r = sorted(m.items())
    print >>src_fp, "    " + \
        ",\n    ".join([",".join(["{0x%04x,0x%04x}"%t for t in nm[i:i+4]]) for i in range(i, len(nm), 4)]);

    print >>src_fp, "  };"

  print >>src_fp, "}"

  print >>hdr_fp, "#ifndef __ISO_8859__"
  print >>hdr_fp, "#define __ISO_8859__"
  print >>hdr_fp, "#include \"stdint.h\""
  print >>hdr_fp, ""
  print >>hdr_fp, "namespace %s {"%(ns)
  print >>hdr_fp, "  typedef uint16_t lowerplane_t ;"
  print >>hdr_fp, "  struct iso_map_record {"
  print >>hdr_fp, "    lowerplane_t key;"
  print >>hdr_fp, "    lowerplane_t val;"
  print >>hdr_fp, "  };"
  print >>hdr_fp, "  /* map segment size, the map size is"
  print >>hdr_fp, "   * (iso_map_size * rev_<name>_size) */"
  print >>hdr_fp, "  static const int iso_map_size = 0x100;"
  for k,f in sorted(files.items()):
    print >>hdr_fp, "  extern const lowerplane_t map_%s[iso_map_size];"%(k)
    print >>hdr_fp, "  extern const int rev_%s_size;"%(k)
    print >>hdr_fp, "  extern const iso_map_record rev_%s[];"%(k)
  print >>hdr_fp, "}"
  print >>hdr_fp, "#endif /*__ISO_8859__*/"
  return 0

@tool()
def gentestdata(path):
  fp = open(path, 'w')

  try:
    for i in xrange(0,0x11000):
      fp.write(unichr(i).encode("utf-8"))
  finally:
    fp.close()

  return 0

if __name__ == "__main__":
  if len(sys.argv) < 2:
    sys.exit(1);

  cmd = commands.get(sys.argv[1], None)

  if cmd is None:
    print "no such tool:", sys.argv[1]
    sys.exit(1)

  sys.exit(cmd(sys.argv[2:]))
