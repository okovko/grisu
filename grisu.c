#include <stdio.h>
#include <math.h>

#include "grisu.h"

// convert a double to fp
fp_t double_to_fp(double dbl) {
  uint64_t bits = *(uint64_t *)&dbl;
  fp_t fp = {
    .f = ((DBL_SIG_MASK & bits) + DBL_SIG_IMPLICIT) << PRECISION_DIFF,
    .e = ((DBL_EXP_MASK & bits) >> DBL_EXP_SHIFT) - DBL_EXP_BIAS - PRECISION_DIFF,
  };
  // handle denormals
  while ((fp.f & FP_TOP_BIT) == 0) {
    fp.f <<= 1;
    fp.e++;
  }
  return fp;
}

#define MASK_32 (0x00000000FFFFFFFFul)
#define ROUND_EVEN (1u << 31)
// expanded distributive property of multiplication
// lhs * rhs = (lhs_low + lhs_high) * (rhs_low + rhs_high)
// it's a 128 bit multiplication shifted right 64 bits, with even rounding
// note that all multiplied pairs have to be 32 bit to avoid overflow
// so they are either shifted or masked to select 32 bits at a time
// and the carries are propagated from bottom up
// the product is not likely normalized
fp_t fp_multiply(fp_t lhs, fp_t rhs) {
  struct { uint64_t low, high; }
    l = {.low = lhs.f & MASK_32, .high = lhs.f >> 32},
    r = {.low = rhs.f & MASK_32, .high = rhs.f >> 32};
  struct { uint64_t low, high; } bits128_to_64 = {
    .low = ((l.low * r.low >> 32) + (l.low * r.high & MASK_32) + (l.high * r.low & MASK_32) + ROUND_EVEN) >> 32,
    .high = l.high * r.high + (l.low * r.high >> 32) + (l.high * r.low >> 32),
  };
  return (fp_t) {
    .f = bits128_to_64.low + bits128_to_64.high,
    .e = lhs.e + rhs.e + FP_PRECISION,
  };
}

// we have a maximum of 21 digits
// v % 10^7 gets the bottom 7
// (v / 10^7) % 10^7 gets the middle 7
// v / 10^14 gets the top 7
// can't use these calculations directly because they'll zero out
// the implementation is equivalent, and can be readily derived on paper
#define TEN_POW_7 (10000000)
void generate_digits(fp_t fp, uint32_t parts[3]) {
  parts[2] = (fp.f % (TEN_POW_7 >> fp.e)) << fp.e;
  parts[1] = (fp.f / (TEN_POW_7 >> fp.e)) % TEN_POW_7;
  parts[0] = (fp.f / (TEN_POW_7 >> fp.e)) / TEN_POW_7;
}

// in this minimum demo implementation, just print the result to stdout
void grisu(double dbl, char *buf) {
  fp_t fp = double_to_fp(dbl);
  int k = k_comp(fp.e + 64);
  fp_t pow10 = double_to_fp(pow(10, k));
  fp_t fp_pow10 = fp_multiply(fp, pow10);
  uint32_t parts[3] = {0};
  generate_digits(fp_pow10, parts);
  sprintf(buf, "%u%07u%07ue%d", parts[0], parts[1], parts[2], -k);
}

// better to manually enter values in source than to read from cmd args
// don't know what algorithm will be used to read the values in from cmd
// but the c spec does require that the value be read without drift
// is there such a guarantee for posix?
int main() {
  char buf[256] = {0};
  double dbl = 1000000001.01010101015252;
  grisu(dbl, buf);
  printf("%s\n", buf);
}
