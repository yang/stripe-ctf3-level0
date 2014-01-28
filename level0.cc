#include <ctime>
#include <ctype.h>
#include <unistd.h>
#include <cstdio>
#include <string.h>
#include <unordered_set>
#include "cmph.h"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <iostream>
#include <sys/timeb.h>
//#include "xxhash-read-only/xxhash.h"
//#include "xxhash-read-only/xxhash.c"
using namespace std;
#define ftime(x) (void*)0
//#define ftime(x) x = usec()
int elapsed(const timeb &end, const timeb &start) {
  return  (int) (1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
}
unsigned long usec() {
  timeval t;
  gettimeofday(&t, 0);
  return 1000000 * t.tv_sec + t.tv_usec;
}
int elapsed_(unsigned long end, unsigned long start) {
  return end - start;
}
enum { DICTLEN = 2249138 };
enum { INLEN = 224913 };
enum { OUTLEN = 224913 };
enum { MAPSIZE = 210680 };
#include "test/data/lower.h"
char *dictbuf = (char *) test_data_lower_txt;
char inbuf[INLEN];
char outbuf[OUTLEN];
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#include "test/data/packed.h"
uint32_t *markers = (uint32_t *) test_data_packed;
// uint32_t *markers = ; // [MAPSIZE];
struct strview { uint32_t data; };
inline bool is_input(const strview& v) { return v.data & 0x80000000U; }
inline uint8_t vlen(const strview& v) { return (v.data & 0x7f000000U) >> 24U; }
inline uint32_t vstart(const strview& v) { return (v.data & 0x00ffffffU); }
inline int memcmpr(const char *a, const char *b, size_t n) {
  for (size_t i = 0; i < n; i++) {
    if (a[i] != b[i]) {
      return 1;
    }
  }
  return 0;
}
//inline unsigned int std::hash(const strview& v) {
//  return XXH32(buf[start(v.data)], len(v.data), 0);
//}
int main(int argc, char **argv) {
  //int dictf = open("test/data/lower.txt", O_RDONLY);
  ////cout << dictf << endl;
  //size_t dictcount = read(dictf, dictbuf, DICTLEN);
  ////cout << dictcount << endl;
  //dictbuf[dictcount] = '\0';
  //close(dictf);
  //if (dictcount < DICTLEN) {
  //  perror("read dict");
  //  return 1;
  //}

  //int mapf = open("test/data/packed", O_RDONLY);
  ////cout << mapf << endl;
  //size_t mapcount = read(mapf, markers, MAPSIZE * sizeof(uint32_t));
  ////cout << mapcount << endl;
  //close(mapf);
  //if (mapcount < MAPSIZE * sizeof(uint32_t)) {
  //  perror("read map");
  //  return 1;
  //}
  //for (int i = 0; i < 100; i++) {
  //  cout << (int) markers[i] << ' ';
  //}
  //cout << endl;

  double t0, t1;
  t0 = usec();
  size_t incount = 0;
  if (argc > 2)
    incount = read(open(argv[2], O_RDONLY), inbuf, INLEN);
  else {
    size_t inread;
    while ((inread = read(STDIN_FILENO, inbuf + incount, INLEN - incount)) != 0) {
      incount += inread;
    }
  }
  inbuf[incount] = '\0';
  t1 = usec();
  // cout << "reading " << elapsed_(t1, t0) << endl;

  t0 = usec();
  FILE *mphf = fopen("test/data/lower.txt.mph", "r");
  cmph_t *hasher = cmph_load(mphf);
  char *start = inbuf;
  char lowered[99];
  char *outcur = outbuf;
  while (true) {
    char *end = strpbrk(start, " \n");
    bool stop = end == NULL;
    if (end == NULL) end = inbuf + incount;
    size_t len = end - start;
    if (len == 0) break;
    for (size_t i = 0; i < len; i++) {
      lowered[i] = tolower(start[i]);
    }
    unsigned int hashed = cmph_search(hasher, lowered, (cmph_uint32) len);
    // unsigned int hashed = XXH32(lowered, len, 0);

    if (true) {
      strview v = { markers[hashed] };
      //cout << "hash " << hashed << endl;
      //cout << "query start " << (start - inbuf) << " len " << len << ": " << string(start, len) << endl;
      //cout << "match start " << vstart(v) << " len " << (int) vlen(v) << ": " << string(dictbuf + vstart(v), vlen(v)) << endl;
      bool hit = len == vlen(v) && memcmpr(lowered, dictbuf + vstart(v), len) == 0;
      if (!hit) *outcur++ = '<';
      //memcpy(outcur, start, len);
      while (start != end) *outcur++ = *start++;
      //outcur += len;
      if (!hit) *outcur++ = '>';
      if (stop) {
        break;
      }
    }

    //size_t skip = end + 1 - (start + len);
    //memcpy(outcur, end, skip);
    *outcur++ = *end;
    start = end + 1;
  }
  t1 = usec();
  // cout << "processing " << elapsed_(t1, t0) << endl;

  //*outcur++ = '\0';
  t0 = usec();
  size_t outlen = outcur - outbuf;
  write(STDOUT_FILENO, outbuf, outlen);
  cmph_destroy(hasher);
  t1 = usec();
  // cout << "printing " << elapsed_(t1, t0) << endl;

  return 0;
}
// import sys, threading
// ds = 
// def f(i):
//   def g():
//     ds[i] = set(open(sys.argv[1]).read().split('\n'))
// ts = [threading.Thread(f).start() for i in xrange(4)]
// for t in ts: t.join()
// import re
// contents = sys.stdin.read()
// sys.stdout.write(re.sub(r'[^ \n]+', lambda m: '<'+m.group()+'>' if m.group().lower() not in d else m.group(), contents))
