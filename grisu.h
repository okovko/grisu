#ifndef __GRISU_H_
#define __GRISU_H_

#include <stdint.h>

typedef struct fp_s {
  uint64_t f;
  uint32_t e;
} fp_t;

#define DBL_SIG_PRECISION (53)
#define FP_PRECISION      (64)
#define PRECISION_DIFF    (FP_PRECISION - DBL_SIG_PRECISION)
#define FP_TOP_BIT        (0x8000000000000000ul)

#define DBL_SIGN_MASK     (0x1000000000000000ul)
#define DBL_EXP_MASK      (0x7FF0000000000000ul) // 11 bits
#define DBL_SIG_MASK      (0x000FFFFFFFFFFFFFul) // 52 bits
#define DBL_EXP_SHIFT     (52)
#define DBL_EXP_BIAS      (1075) // 1075 = 1023 + 52
#define DBL_SIG_IMPLICIT  (0x0010000000000000ul) // implicit significand top bit

#define D_1_LOG2_10 (0.30102999566398114) // 1 / log2(10)
#define CEIL(dbl) \
  ((int64_t)(dbl) == (dbl) ? (int64_t)(dbl) : \
    (dbl) > 0 ? (int64_t)(dbl) + 1 : (int64_t)(dbl))
#define k_comp(exp) \
  ((int32_t)CEIL((-(exp) + FP_PRECISION - 1) * D_1_LOG2_10))

fp_t double_to_fp(double dbl);
fp_t fp_multiply(fp_t lhs, fp_t rhs);
void generate_digits(fp_t fp, uint32_t parts[3]);
void grisu(double dbl, char *buf);

#endif // __GRISU_H_
