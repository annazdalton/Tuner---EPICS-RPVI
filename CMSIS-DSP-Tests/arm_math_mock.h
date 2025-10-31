/**
 * arm_math_mock.h - Mock CMSIS-DSP implementation for native (PC) platform testing
 * 
 * This header provides simplified implementations of CMSIS-DSP functions
 * that work on Windows/Linux for testing purposes. On Teensy, the real
 * ARM-optimized CMSIS-DSP library from PlatformIO is used.
 * 
 * This is so we can test on any device with an intel processor (Like Windows PCs) and keep our code written in C
 * On the Teensy: it will automatically use the real ARM library from CMSIS-DSP library 
*/

#ifndef ARM_MATH_MOCK_H
#define ARM_MATH_MOCK_H

#include <stdint.h>
#include <math.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ARM CMSIS-DSP type definitions */
typedef float float32_t;
typedef int32_t q31_t;
typedef int16_t q15_t;

typedef struct {
    float32_t real;
    float32_t imag;
} arm_complex_f32_t;

typedef enum {
    ARM_MATH_SUCCESS = 0,
    ARM_MATH_ARGUMENT_ERROR = -1,
    ARM_MATH_LENGTH_ERROR = -2
} arm_status;

/* ============================================================================
 * Vector Operations
 * ============================================================================ */

/**
 * Element-wise addition of two vectors
 * result[n] = srcA[n] + srcB[n]
 */
static inline void arm_add_f32(
    const float32_t * pSrcA,
    const float32_t * pSrcB,
    float32_t * pDst,
    uint32_t blockSize)
{
    for (uint32_t i = 0; i < blockSize; i++) {
        pDst[i] = pSrcA[i] + pSrcB[i];
    }
}

/**
 * Element-wise subtraction of two vectors
 * result[n] = srcA[n] - srcB[n]
 */
static inline void arm_sub_f32(
    const float32_t * pSrcA,
    const float32_t * pSrcB,
    float32_t * pDst,
    uint32_t blockSize)
{
    for (uint32_t i = 0; i < blockSize; i++) {
        pDst[i] = pSrcA[i] - pSrcB[i];
    }
}

/**
 * Element-wise multiplication of two vectors
 * result[n] = srcA[n] * srcB[n]
 */
static inline void arm_mult_f32(
    const float32_t * pSrcA,
    const float32_t * pSrcB,
    float32_t * pDst,
    uint32_t blockSize)
{
    for (uint32_t i = 0; i < blockSize; i++) {
        pDst[i] = pSrcA[i] * pSrcB[i];
    }
}

/**
 * Dot product of two vectors
 * result = sum(srcA[n] * srcB[n])
 */
static inline void arm_dot_prod_f32(
    const float32_t * pSrcA,
    const float32_t * pSrcB,
    uint32_t blockSize,
    float32_t * result)
{
    float32_t sum = 0.0f;
    for (uint32_t i = 0; i < blockSize; i++) {
        sum += pSrcA[i] * pSrcB[i];
    }
    *result = sum;
}

/**
 * Multiply a vector by a scalar
 * result[n] = src[n] * scale
 */
static inline void arm_scale_f32(
    const float32_t * pSrc,
    float32_t scale,
    float32_t * pDst,
    uint32_t blockSize)
{
    for (uint32_t i = 0; i < blockSize; i++) {
        pDst[i] = pSrc[i] * scale;
    }
}

/**
 * Absolute value of each element
 * result[n] = |src[n]|
 */
static inline void arm_abs_f32(
    const float32_t * pSrc,
    float32_t * pDst,
    uint32_t blockSize)
{
    for (uint32_t i = 0; i < blockSize; i++) {
        pDst[i] = fabsf(pSrc[i]);
    }
}

/* ============================================================================
 * Statistical Operations
 * ============================================================================ */

/**
 * Mean (average) of a vector
 */
static inline void arm_mean_f32(
    const float32_t * pSrc,
    uint32_t blockSize,
    float32_t * pResult)
{
    float32_t sum = 0.0f;
    for (uint32_t i = 0; i < blockSize; i++) {
        sum += pSrc[i];
    }
    *pResult = sum / (float32_t)blockSize;
}

/**
 * Variance of a vector
 */
static inline void arm_var_f32(
    const float32_t * pSrc,
    uint32_t blockSize,
    float32_t * pResult)
{
    float32_t mean = 0.0f;
    float32_t sum_sq_diff = 0.0f;
    
    /* Calculate mean */
    for (uint32_t i = 0; i < blockSize; i++) {
        mean += pSrc[i];
    }
    mean /= (float32_t)blockSize;
    
    /* Calculate sum of squared differences */
    for (uint32_t i = 0; i < blockSize; i++) {
        float32_t diff = pSrc[i] - mean;
        sum_sq_diff += diff * diff;
    }
    
    *pResult = sum_sq_diff / (float32_t)blockSize;
}

/**
 * Standard deviation of a vector
 */
static inline void arm_std_f32(
    const float32_t * pSrc,
    uint32_t blockSize,
    float32_t * pResult)
{
    float32_t variance;
    arm_var_f32(pSrc, blockSize, &variance);
    *pResult = sqrtf(variance);
}

/**
 * Minimum value in a vector
 */
static inline void arm_min_f32(
    const float32_t * pSrc,
    uint32_t blockSize,
    float32_t * pResult,
    uint32_t * pIndex)
{
    float32_t min_val = pSrc[0];
    uint32_t min_idx = 0;
    
    for (uint32_t i = 1; i < blockSize; i++) {
        if (pSrc[i] < min_val) {
            min_val = pSrc[i];
            min_idx = i;
        }
    }
    
    *pResult = min_val;
    if (pIndex != NULL) {
        *pIndex = min_idx;
    }
}

/**
 * Maximum value in a vector
 */
static inline void arm_max_f32(
    const float32_t * pSrc,
    uint32_t blockSize,
    float32_t * pResult,
    uint32_t * pIndex)
{
    float32_t max_val = pSrc[0];
    uint32_t max_idx = 0;
    
    for (uint32_t i = 1; i < blockSize; i++) {
        if (pSrc[i] > max_val) {
            max_val = pSrc[i];
            max_idx = i;
        }
    }
    
    *pResult = max_val;
    if (pIndex != NULL) {
        *pIndex = max_idx;
    }
}

/* ============================================================================
 * Complex Number Operations
 * ============================================================================ */

/**
 * Complex conjugate
 * result[n].real = src[n].real
 * result[n].imag = -src[n].imag
 */
static inline void arm_cmplx_conj_f32(
    const arm_complex_f32_t * pSrc,
    arm_complex_f32_t * pDst,
    uint32_t numSamples)
{
    for (uint32_t i = 0; i < numSamples; i++) {
        pDst[i].real = pSrc[i].real;
        pDst[i].imag = -pSrc[i].imag;
    }
}

/**
 * Complex magnitude
 * result[n] = sqrt(real^2 + imag^2)
 */
static inline void arm_cmplx_mag_f32(
    const arm_complex_f32_t * pSrc,
    float32_t * pDst,
    uint32_t numSamples)
{
    for (uint32_t i = 0; i < numSamples; i++) {
        float32_t real = pSrc[i].real;
        float32_t imag = pSrc[i].imag;
        pDst[i] = sqrtf(real * real + imag * imag);
    }
}

/* ============================================================================
 * Window Functions
 * ============================================================================ */

/**
 * Hann (Hanning) window generation
 */
static inline void arm_hann_f32(
    float32_t * pDst,
    uint32_t blockSize)
{
    for (uint32_t n = 0; n < blockSize; n++) {
        float32_t angle = 2.0f * 3.14159265359f * n / (float32_t)(blockSize - 1);
        pDst[n] = 0.5f - 0.5f * cosf(angle);
    }
}

/**
 * Hamming window generation
 */
static inline void arm_hamming_f32(
    float32_t * pDst,
    uint32_t blockSize)
{
    for (uint32_t n = 0; n < blockSize; n++) {
        float32_t angle = 2.0f * 3.14159265359f * n / (float32_t)(blockSize - 1);
        pDst[n] = 0.54f - 0.46f * cosf(angle);
    }
}

/* ============================================================================
 * Copy Operations
 * ============================================================================ */

/**
 * Copy a vector
 */
static inline void arm_copy_f32(
    const float32_t * pSrc,
    float32_t * pDst,
    uint32_t blockSize)
{
    memcpy(pDst, pSrc, blockSize * sizeof(float32_t));
}

/* ============================================================================
 * FFT Stub (Minimal Implementation)
 * Note: For full FFT testing on native platform, use a dedicated FFT library
 * ============================================================================ */

typedef struct {
    uint16_t fftLen;
    uint8_t ifftFlag;
    uint8_t bitReverseFlag;
    float32_t *pTwiddle;
    uint16_t *pBitRevTable;
    uint16_t twidCoefModifier;
} arm_rfft_fast_instance_f32;

typedef struct {
    uint16_t fftLen;
} arm_rfft_instance_f32;

typedef struct {
    uint16_t fftLen;
    uint8_t ifftFlag;
    uint8_t bitReverseFlag;
    float32_t *pTwiddle;
    uint16_t *pBitRevTable;
    uint16_t twidCoefModifier;
} arm_cfft_instance_f32;

/**
 * Complex magnitude squared
 */
static inline void arm_cmplx_mag_squared_f32(
    const arm_complex_f32_t * pSrc,
    float32_t * pDst,
    uint32_t numSamples)
{
    for (uint32_t i = 0; i < numSamples; i++) {
        float32_t real = pSrc[i].real;
        float32_t imag = pSrc[i].imag;
        pDst[i] = real * real + imag * imag;
    }
}

/* ============================================================================
 * FFT Functions
 * ============================================================================ */

/**
 * Real FFT initialization
 */
static inline arm_status arm_rfft_init_f32(
    arm_rfft_instance_f32 * S,
    uint32_t fftLen,
    uint8_t ifftFlag,
    uint8_t bitReverseFlag)
{
    S->fftLen = fftLen;
    return ARM_MATH_SUCCESS;
}

/**
 * Real FFT (simplified DFT-based implementation for testing)
 */
static inline void arm_rfft_f32(
    arm_rfft_instance_f32 * S,
    float32_t * pSrc,
    float32_t * pDst)
{
    uint32_t N = S->fftLen;
    
    /* Output format: [real[0], real[1], ..., real[N/2], imag[N/2-1], ..., imag[1]] */
    for (uint32_t k = 0; k <= N/2; k++) {
        float32_t real_part = 0.0f;
        float32_t imag_part = 0.0f;
        
        for (uint32_t n = 0; n < N; n++) {
            float32_t angle = -2.0f * 3.14159265359f * k * n / (float32_t)N;
            real_part += pSrc[n] * cosf(angle);
            imag_part += pSrc[n] * sinf(angle);
        }
        
        pDst[k] = real_part;
        if (k > 0 && k < N/2) {
            pDst[N - k] = imag_part;
        }
    }
}

/**
 * Initialzes the complex FFT
 */
static inline arm_status arm_cfft_init_f32(
    arm_cfft_instance_f32 * S,
    uint32_t fftLen)
{
    S->fftLen = fftLen;
    return ARM_MATH_SUCCESS;
}

/**
 * Will execute the complex FFT (simplified DFT-based implementation for testing)
 */
static inline void arm_cfft_f32(
    arm_cfft_instance_f32 * S,
    float32_t * p,
    uint8_t ifftFlag,
    uint8_t bitReverseFlag)
{
    uint32_t N = S->fftLen;
    
    for (uint32_t k = 0; k < N; k++) {
        float32_t real_sum = 0.0f;
        float32_t imag_sum = 0.0f;
        
        float32_t factor = ifftFlag ? 1.0f : -1.0f;
        
        for (uint32_t n = 0; n < N; n++) {
            float32_t angle = factor * 2.0f * 3.14159265359f * k * n / (float32_t)N;
            real_sum += p[2*n] * cosf(angle) - p[2*n+1] * sinf(angle);
            imag_sum += p[2*n] * sinf(angle) + p[2*n+1] * cosf(angle);
        }
        
        p[2*k] = real_sum;
        p[2*k+1] = imag_sum;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* ARM_MATH_MOCK_H */
