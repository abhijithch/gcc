/* { dg-do compile } */
/* { dg-options "-mavx512er -O2" } */
/* { dg-final { scan-assembler-times "vexp2ps\[ \\t\]+\[^\n\]*%zmm\[0-9\]\[\\n\]" 2 } } */
/* { dg-final { scan-assembler-times "vexp2ps\[ \\t\]+\[^\n\]*%zmm\[0-9\]\{%k\[1-7\]\}\[^\{\]" 2 } } */
/* { dg-final { scan-assembler-times "vexp2ps\[ \\t\]+\[^\n\]*%zmm\[0-9\]\{%k\[1-7\]\}\{z\}" 2 } } */
/* { dg-final { scan-assembler-times "vexp2ps\[ \\t\]+\[^\n\]*\{rn-sae\}\[^\n\]*%zmm\[0-9\]" 1 } } */
/* { dg-final { scan-assembler-times "vexp2ps\[ \\t\]+\[^\n\]*\{ru-sae\}\[^\n\]*%zmm\[0-9\]\{%k\[1-7\]\}\[^\{\]" 1 } } */
/* { dg-final { scan-assembler-times "vexp2ps\[ \\t\]+\[^\n\]*\{rz-sae\}\[^\n\]*%zmm\[0-9\]\{%k\[1-7\]\}\{z\}" 1 } } */

#include <immintrin.h>

volatile __m512 x;
volatile __mmask16 m;

void extern
avx512er_test (void)
{
  x = _mm512_exp2a23_ps (x);
  x = _mm512_mask_exp2a23_ps (x, m, x);
  x = _mm512_maskz_exp2a23_ps (m, x);
  x = _mm512_exp2a23_round_ps (x, _MM_FROUND_TO_NEAREST_INT);
  x = _mm512_mask_exp2a23_round_ps (x, m, x, _MM_FROUND_TO_POS_INF);
  x = _mm512_maskz_exp2a23_round_ps (m, x, _MM_FROUND_TO_ZERO);
}
