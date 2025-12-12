#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define Random(i) ((rand() % (i)) + 1)
#define isEven(src) (!((src) & 0x1))
#define hasFactor(src, n) ((((src) != (n)) && ((src) % (n) == 0)))
#define BitIsSet(src, b) (((src) & (1 << (b))) != 0)

#define CHECKTIMES 20

#if defined(BUDDYUINT64)
typedef BUDDYUINT64 UINT64;
#define BUILTIN64
#elif defined(__GNUC__) || defined(__KCC)
typedef long long UINT64;
#define BUILTIN64
#elif defined(_MSV_VER)
typedef unsigned _int64 UINT64;
#define BUILTIN64
#else
typedef struct __UINT64 {
  unsigned int hi;
  unsigned int lo;
} UINT64;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define GETCARRY(a, b) (((a) + (b)) < MAX((a), (b)) ? 1 : 0)
#endif

#ifndef BUILTIN64
/*************************************************************************
   64 bit unsigned int arithmetics
*************************************************************************/

static UINT64 u64_mul(unsigned int x, unsigned int y) {
  UINT64 res;
  unsigned int yh = 0;
  unsigned int yl = y;
  int i;

  res.lo = res.hi = 0;

  for (i = 0; i < 32; ++i) {
    if (x & 0x1) {
      unsigned int carry = GETCARRY(res.lo, yl);
      res.lo += yl;
      res.hi += yh + carry;
    }

    yh = (yh << 1) | (yl & 0x80000000 ? 1 : 0);
    yl = (yl << 1);

    x >>= 1;
  }

  return res;
}

static void u64_shl(UINT64 *a, unsigned int *carryOut) {
  *carryOut = (*carryOut << 1) | (a->hi & 0x80000000 ? 0x1 : 0x0);
  a->hi = (a->hi << 1) | (a->lo & 0x80000000 ? 0x1 : 0x0);
  a->lo = (a->lo << 1);
}

static unsigned int u64_mod(UINT64 dividend, unsigned int divisor) {
  unsigned int remainder = 0;
  int i;

  u64_shl(&dividend, &remainder);

  for (i = 0; i < 64; ++i) {
    if (remainder >= divisor)
      remainder -= divisor;

    u64_shl(&dividend, &remainder);
  }

  return remainder >> 1;
}
#endif /* BUILTIN64 */

#ifdef BUILTIN64
#define u64_mulmod(a, b, c)                                                    \
  ((unsigned int)(((UINT64)a * (UINT64)b) % (UINT64)c));
#else
#define u64_mulmod(a, b, c) u64_mod(u64_mul((a), (b)), (c));
#endif

/*************************************************************************
  Miller Rabin check
*************************************************************************/

static unsigned int numberOfBits(unsigned int src) {
  unsigned int b;

  if (src == 0)
    return 0;

  for (b = (sizeof(unsigned int) * 8) - 1; b > 0; --b)
    if (BitIsSet(src, b))
      return b + 1;

  return 1;
}

static int isWitness(unsigned int witness, unsigned int src) {
  unsigned int bitNum = numberOfBits(src - 1) - 1;
  unsigned int d = 1;
  int i;

  for (i = bitNum; i >= 0; --i) {
    unsigned int x = d;

    d = u64_mulmod(d, d, src);

    if (d == 1 && x != 1 && x != src - 1)
      return 1;

    if (BitIsSet(src - 1, i))
      d = u64_mulmod(d, witness, src);
  }

  return d != 1;
}

static int isMillerRabinPrime(unsigned int src) {
  int n;

  for (n = 0; n < CHECKTIMES; ++n) {
    unsigned int witness = Random(src - 1);

    if (isWitness(witness, src))
      return 0;
  }

  return 1;
}

/*************************************************************************
  Basic prime searching stuff
*************************************************************************/

static int hasEasyFactors(unsigned int src) {
  return hasFactor(src, 3) || hasFactor(src, 5) || hasFactor(src, 7) ||
         hasFactor(src, 11) || hasFactor(src, 13);
}

static int isPrime(unsigned int src) {
  if (hasEasyFactors(src))
    return 0;

  return isMillerRabinPrime(src);
}

/*************************************************************************
  External interface
*************************************************************************/

inline unsigned int bdd_prime_gte(unsigned int src) {
  if (isEven(src))
    ++src;

  while (!isPrime(src))
    src += 2;

  return src;
}

inline unsigned int bdd_prime_lte(unsigned int src) {
  if (isEven(src))
    --src;

  while (!isPrime(src))
    src -= 2;

  return src;
}
