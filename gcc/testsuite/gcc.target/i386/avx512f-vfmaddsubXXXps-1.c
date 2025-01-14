/* { dg-do compile } */
/* { dg-options "-mavx512f -O2" } */
/* { dg-final { scan-assembler-times "vfmaddsub...ps\[ \\t\]+\[^\n\]*%zmm\[0-9\]\[^\{\]" 8 } } */
/* { dg-final { scan-assembler-times "vfmaddsub...ps\[ \\t\]+\[^\n\]*%zmm\[0-9\]\{%k\[1-7\]\}\[^\{\]" 4 } } */
/* { dg-final { scan-assembler-times "vfmaddsub231ps\[ \\t\]+\[^\n\]*%zmm\[0-9\]\{%k\[1-7\]\}\[^\{\]" 2 } } */
/* { dg-final { scan-assembler-times "vfmaddsub...ps\[ \\t\]+\[^\n\]*%zmm\[0-9\]\{%k\[1-7\]\}\{z\}" 2 } } */
/* { dg-final { scan-assembler-times "vfmaddsub...ps\[ \\t\]+\[^\n\]*\{rn-sae\}\[^\n\]*%zmm\[0-9\]\[^\{\]" 1 } } */
/* { dg-final { scan-assembler-times "vfmaddsub...ps\[ \\t\]+\[^\n\]*\{rd-sae\}\[^\n\]*%zmm\[0-9\]\{%k\[1-7\]\}\[^\{\]" 1 } } */
/* { dg-final { scan-assembler-times "vfmaddsub231ps\[ \\t\]+\[^\n\]*\{ru-sae\}\[^\n\]*%zmm\[0-9\]\{%k\[1-7\]\}\[^\{\]" 1 } } */
/* { dg-final { scan-assembler-times "vfmaddsub...ps\[ \\t\]+\[^\n\]*\{rz-sae\}\[^\n\]*%zmm\[0-9\]\{%k\[1-7\]\}\{z\}" 1 } } */

#include <immintrin.h>

volatile __m512 x1, x2, x3;
volatile __mmask16 m;

void extern
avx512f_test (void)
{
  x1 = _mm512_fmaddsub_ps (x1, x2, x3);
  x1 = _mm512_mask_fmaddsub_ps (x1, m, x2, x3);
  x3 = _mm512_mask3_fmaddsub_ps (x1, x2, x3, m);
  x1 = _mm512_maskz_fmaddsub_ps (m, x1, x2, x3);
  x1 = _mm512_fmaddsub_round_ps (x1, x2, x3, _MM_FROUND_TO_NEAREST_INT);
  x1 = _mm512_mask_fmaddsub_round_ps (x1, m, x2, x3, _MM_FROUND_TO_NEG_INF);
  x3 = _mm512_mask3_fmaddsub_round_ps (x1, x2, x3, m, _MM_FROUND_TO_POS_INF);
  x1 = _mm512_maskz_fmaddsub_round_ps (m, x1, x2, x3, _MM_FROUND_TO_ZERO);
}
