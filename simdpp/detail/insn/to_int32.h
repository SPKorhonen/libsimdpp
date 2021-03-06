/*  Copyright (C) 2013-2014  Povilas Kanapickas <povilas@radix.lt>

    Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
            http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LIBSIMDPP_SIMDPP_DETAIL_INSN_TO_INT32_H
#define LIBSIMDPP_SIMDPP_DETAIL_INSN_TO_INT32_H

#ifndef LIBSIMDPP_SIMD_H
    #error "This file must be included through simd.h"
#endif

#include <simdpp/types.h>
#include <simdpp/core/combine.h>
#include <simdpp/detail/not_implemented.h>
#include <simdpp/core/i_shift_r.h>
#include <simdpp/core/move_l.h>
#include <simdpp/core/zip_hi.h>
#include <simdpp/core/zip_lo.h>
#include <simdpp/core/unzip_lo.h>
#include <simdpp/core/detail/subvec_insert.h>

namespace simdpp {
namespace SIMDPP_ARCH_NAMESPACE {
namespace detail {
namespace insn {

SIMDPP_INL int32x8 i_to_int32(const int16x8& a)
{
#if SIMDPP_USE_NULL
    int32x8 r;
    for (unsigned i = 0; i < 8; i++) {
        r.vec(i/4).el(i%4) = int32_t(a.vec(0).el(i));
    }
    return r;
#elif SIMDPP_USE_AVX2
    return  _mm256_cvtepi16_epi32(a);
#elif SIMDPP_USE_SSE4_1
    int32x8 r;
    r.vec(0) = _mm_cvtepi16_epi32(a);
    r.vec(1) = _mm_cvtepi16_epi32(move8_l<4>(a).eval());
    return r;
#elif SIMDPP_USE_SSE2 || SIMDPP_USE_MSA
    int16x8 sign = shift_r<15>(a);
    int32x4 lo, hi;
    lo = zip8_lo(a, sign);
    hi = zip8_hi(a, sign);
    return combine(lo, hi);
#elif SIMDPP_USE_NEON
    int32x8 r;
    r.vec(0) = vmovl_s16(vget_low_s16(a.vec(0)));
    r.vec(1) = vmovl_s16(vget_high_s16(a.vec(1)));
    return r;
#elif SIMDPP_USE_ALTIVEC
    int32x4 b0, b1;
    b0 = vec_unpackh((__vector int16_t)a.vec(0));
    b1 = vec_unpackl((__vector int16_t)a.vec(0));
    return combine(b0, b1);
#endif
}

#if SIMDPP_USE_AVX2
SIMDPP_INL int32<16> i_to_int32(const int16<16>& a)
{
#if SIMDPP_USE_AVX512F
    return _mm512_cvtepi16_epi32(a);
#else
    int32<8> r0, r1;
    int16<8> a0, a1;
    split(a, a0, a1);
    r0 = _mm256_cvtepi16_epi32(a0);
    r1 = _mm256_cvtepi16_epi32(a1);
    return combine(r0, r1);
#endif
}
#endif

#if SIMDPP_USE_AVX512BW
SIMDPP_INL int32<32> i_to_int32(const int16<32>& a)
{
    int32<16> r0, r1;
    int16<16> a0, a1;
    split(a, a0, a1);
    r0 = _mm512_cvtepi16_epi32(a0);
    r1 = _mm512_cvtepi16_epi32(a1);
    return combine(r0, r1);
}
#endif

template<unsigned N> SIMDPP_INL
int32<N> i_to_int32(const int16<N>& a)
{
    int32<N> r;
    for (unsigned i = 0; i < a.vec_length; ++i) {
        detail::subvec_insert(r, i_to_int32(a.vec(i)), i);
    }
    return r;
}

// -----------------------------------------------------------------------------

SIMDPP_INL uint32<8> i_to_uint32(const uint16<8>& a)
{
#if SIMDPP_USE_NULL
    uint32<8> r;
    for (unsigned i = 0; i < 8; i++) {
        r.vec(i/4).el(i%4) = uint32_t(a.vec(0).el(i));
    }
    return r;
#elif SIMDPP_USE_AVX2
    return  _mm256_cvtepu16_epi32(a);
#elif SIMDPP_USE_SSE4_1
    uint32<8> r;
    r.vec(0) = _mm_cvtepu16_epi32(a);
    r.vec(1) = _mm_cvtepu16_epi32(move8_l<4>(a).eval());
    return r;
#elif SIMDPP_USE_SSE2 || SIMDPP_USE_MSA || SIMDPP_USE_ALTIVEC
    uint16<8> zero = make_zero();
    uint32<8> r;
    r.vec(0) = zip8_lo(a, zero);
    r.vec(1) = zip8_hi(a, zero);
    return r;
#elif SIMDPP_USE_NEON
    uint32<8> r;
    r.vec(0) = vmovl_u16(vget_low_u16(a.vec(0)));
    r.vec(1) = vmovl_u16(vget_high_u16(a.vec(1)));
    return r;
#endif
}

#if SIMDPP_USE_AVX2
SIMDPP_INL uint32<16> i_to_uint32(const uint16<16>& a)
{
#if SIMDPP_USE_AVX512F
    return _mm512_cvtepu16_epi32(a);
#else
    uint32<16> r;
    uint16<8> a0, a1;
    split(a, a0, a1);
    r.vec(0) = _mm256_cvtepu16_epi32(a0);
    r.vec(1) = _mm256_cvtepu16_epi32(a1);
    return r;
#endif
}
#endif

#if SIMDPP_USE_AVX512BW
SIMDPP_INL uint32<32> i_to_uint32(const uint16<32>& a)
{
    uint32<32> r;
    uint16<16> a0, a1;
    split(a, a0, a1);
    r.vec(0) = _mm512_cvtepu16_epi32(a0);
    r.vec(1) = _mm512_cvtepu16_epi32(a1);
    return r;
}
#endif

template<unsigned N> SIMDPP_INL
uint32<N> i_to_uint32(const uint16<N>& a)
{
    uint32<N> r;
    for (unsigned i = 0; i < a.vec_length; ++i) {
        detail::subvec_insert(r, i_to_uint32(a.vec(i)), i);
    }
    return r;
}

// -----------------------------------------------------------------------------

SIMDPP_INL int32x4 i_to_int32(const float32x4& a)
{
#if SIMDPP_USE_NULL
    int32x4 r;
    for (unsigned i = 0; i < a.length; i++) {
        r.el(i) = int32_t(a.el(i));
    }
    return r;
#elif SIMDPP_USE_SSE2
    return _mm_cvttps_epi32(a);
#elif SIMDPP_USE_NEON && !SIMDPP_USE_NEON_FLT_SP
    detail::mem_block<float32x4> mf(a);
    detail::mem_block<int32x4> mi;
    mi[0] = int(mf[0]);
    mi[1] = int(mf[1]);
    mi[2] = int(mf[2]);
    mi[3] = int(mf[3]);
    return mi;
#elif SIMDPP_USE_NEON_FLT_SP
    return vcvtq_s32_f32(a);
#elif SIMDPP_USE_MSA
    return __msa_ftrunc_s_w(a);
#elif SIMDPP_USE_ALTIVEC
    return vec_cts((__vector float)a, 0);
#endif
}

#if SIMDPP_USE_AVX
SIMDPP_INL int32x8 i_to_int32(const float32x8& a)
{
#if SIMDPP_USE_AVX2
    return _mm256_cvttps_epi32(a);
#else
    __m256i r = _mm256_cvttps_epi32(a);
    uint32<4> r1, r2;
    r1 = _mm256_castsi256_si128(r);
    r2 = _mm256_extractf128_si256(r, 1);
    return combine(r1, r2);
#endif
}
#endif

#if SIMDPP_USE_AVX512F
SIMDPP_INL uint32<16> i_to_int32(const float32<16>& a)
{
    return _mm512_cvttps_epi32(a);
}
#endif

template<unsigned N> SIMDPP_INL
int32<N> i_to_int32(const float32<N>& a)
{
    int32<N> r;
    for (unsigned i = 0; i < a.vec_length; ++i) {
        detail::subvec_insert(r, i_to_int32(a.vec(i)), i);
    }
    return r;
}

// -----------------------------------------------------------------------------

SIMDPP_INL int32x4 i_to_int32(const float64x4& a)
{
#if SIMDPP_USE_SSE2
    int32x4 r, r1, r2;
    float64x2 a1, a2;
    split(a, a1, a2);
    r1 = _mm_cvttpd_epi32(a1);
    r2 = _mm_cvttpd_epi32(a2);
    r = zip2_lo(int64<2>(r1), int64<2>(r2));
    return r;
#elif SIMDPP_USE_NEON64
    int64<2> r1, r2;
    r1 = vcvtq_s64_f64(a.vec(0));
    r2 = vcvtq_s64_f64(a.vec(1));
    // FIXME: saturation
    int32<4> r = vcombine_s32(vqmovn_s64(r1), vqmovn_s64(r2));
    return r;
#elif SIMDPP_USE_VSX_206
    int32<4> r, r1, r2;
    r1 = (__vector int32_t) vec_cts((__vector double)a.vec(0), 0);
    r2 = (__vector int32_t) vec_cts((__vector double)a.vec(1), 0);
    r = unzip4_lo(r1, r2);
    return r;
#elif SIMDPP_USE_MSA
    int64<2> r1, r2;
    r1 = __msa_ftrunc_s_d(a.vec(0));
    r2 = __msa_ftrunc_s_d(a.vec(1));
    return unzip4_lo((int32<4>)r1, (int32<4>)r2);
#elif SIMDPP_USE_NULL || SIMDPP_USE_NEON32 || SIMDPP_USE_ALTIVEC
    detail::mem_block<int32x4> r;
    r[0] = int32_t(a.vec(0).el(0));
    r[1] = int32_t(a.vec(0).el(1));
    r[2] = int32_t(a.vec(1).el(0));
    r[3] = int32_t(a.vec(1).el(1));
    return r;
#else
    return SIMDPP_NOT_IMPLEMENTED1(a);
#endif
}

#if SIMDPP_USE_AVX
SIMDPP_INL int32<8> i_to_int32(const float64<8>& a)
{
#if SIMDPP_USE_AVX512F
    return _mm512_cvtpd_epi32(a);
#else
    int32<4> r1, r2;
    r1 = _mm256_cvtpd_epi32(a.vec(0));
    r2 = _mm256_cvtpd_epi32(a.vec(1));
    return combine(r1, r2);
#endif
}
#endif

#if SIMDPP_USE_AVX512F
SIMDPP_INL int32<16> i_to_int32(const float64<16>& a)
{
    int32<8> r0, r1;
    r0 = _mm512_cvtpd_epi32(a.vec(0));
    r1 = _mm512_cvtpd_epi32(a.vec(1));
    return combine(r0, r1);
}
#endif

template<unsigned N> SIMDPP_INL
int32<N> i_to_int32(const float64<N>& a)
{
    int32<N> r;
    for (unsigned i = 0; i < r.vec_length; ++i) {
        r.vec(i) = i_to_int32(detail::subvec_extract<int32<N>::base_length>(a, i));
    }
    return r;
}

} // namespace insn
} // namespace detail
} // namespace SIMDPP_ARCH_NAMESPACE
} // namespace simdpp

#endif


