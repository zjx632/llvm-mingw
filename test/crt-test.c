/*
 * Copyright (c) 2018 Martin Storsjo
 *
 * This file is part of llvm-mingw.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <fenv.h>
#include <inttypes.h>
#include <stdarg.h>
#ifdef _WIN32
#include <windows.h>
#endif

int tests = 0, fails = 0;
const char *context = "";

#define TEST(x) do { \
        tests++; \
        if (!(x)) { \
            fails++; \
            printf("%s:%d: %s\"%s\" failed\n", __FILE__, __LINE__, context, #x); \
        } \
    } while (0)

#define TEST_STR(x, expect) do { \
        tests++; \
        if (strcmp(x, expect)) { \
            fails++; \
            printf("%s:%d: %sexpected \"%s\", got \"%s\"\n", __FILE__, __LINE__, context, expect, x); \
        } \
    } while (0)

#define TEST_FLT(x, expect) do { \
        tests++; \
        if (x != expect) { \
            fails++; \
            printf("%s:%d: %s%s failed, expected %f, got %f\n", __FILE__, __LINE__, context, #x, (double)expect, (double)x); \
        } \
    } while (0)

#define TEST_FLT_EXPR(x, expr) do { \
        tests++; \
        if (!(expr)) { \
            fails++; \
            printf("%s:%d: %s%s failed, got %f\n", __FILE__, __LINE__, context, #expr, (double)x); \
        } \
    } while (0)

#define TEST_FLT_NAN(x) do { \
        tests++; \
        if (!isnan(x)) { \
            fails++; \
            printf("%s:%d: %s%s failed, got %f, expected NAN\n", __FILE__, __LINE__, context, #x, (double)x); \
        } \
    } while (0)

#define TEST_FLT_ACCURACY(x, expect, accuracy) do { \
        long double val = x; \
        long double diff = fabsl(val - expect); \
        tests++; \
        if (diff > accuracy) { \
            fails++; \
            printf("%s:%d: %s%s failed, expected %f, got %f (diff %f > %f)\n", __FILE__, __LINE__, context, #x, (double)expect, (double)val, (double)diff, (double)accuracy); \
        } \
    } while (0)

#define TEST_INT(x, expect) do { \
        tests++; \
        if (x != expect) { \
            fails++; \
            printf("%s:%d: %s%s failed, expected %lld, got %lld\n", __FILE__, __LINE__, context, #x, (long long)expect, (long long)x); \
        } \
    } while (0)

#define TEST_PTR(x, expect) do { \
        tests++; \
        if (x != expect) { \
            fails++; \
            printf("%s:%d: %s%s failed, expected %p, got %p\n", __FILE__, __LINE__, context, #x, expect, x); \
        } \
    } while (0)

#define F(x) strtod(#x, NULL)
#define L(x) strtol(#x, NULL, 0)
#define UL(x) strtoul(#x, NULL, 0)
#define LL(x) strtoll(#x, NULL, 0)
#define ULL(x) strtoull(#x, NULL, 0)

int vsscanf_wrap(const char* str, const char* fmt, ...) {
    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = vsscanf(str, fmt, ap);
    va_end(ap);
    return ret;
}

int main(int argc, char* argv[]) {
    char buf[200];
    int i;
    uint64_t myconst = 0xbaadf00dcafe;

    snprintf(buf, sizeof(buf), "%f", 3.141592654);
    TEST_STR(buf, "3.141593");
    snprintf(buf, sizeof(buf), "%"PRIx64" %"PRIx64" %"PRIx64" %"PRIx64" %"PRIx64" %"PRIx64" %"PRIx64" %"PRIx64" %"PRIx64" %"PRIx64, myconst + 0, myconst + 1, myconst + 2, myconst + 3, myconst + 4, myconst + 5, myconst + 6, myconst + 7, myconst + 8, myconst + 9);
    TEST_STR(buf, "baadf00dcafe baadf00dcaff baadf00dcb00 baadf00dcb01 baadf00dcb02 baadf00dcb03 baadf00dcb04 baadf00dcb05 baadf00dcb06 baadf00dcb07");

    uint64_t val0, val1, val2, val3, val4, val5, val6, val7, val8, val9;
    if (sscanf("baadf00dcafe baadf00dcaff baadf00dcb00 baadf00dcb01 baadf00dcb02 baadf00dcb03 baadf00dcb04 baadf00dcb05 baadf00dcb06 baadf00dcb07", "%"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64, &val0, &val1, &val2, &val3, &val4, &val5, &val6, &val7, &val8, &val9) != 10) {
        fails++;
        printf("sscanf failed\n");
    } else {
        int64_t diff = 0;
        diff += llabs((int64_t)(val0 - 0 - myconst));
        diff += llabs((int64_t)(val1 - 1 - myconst));
        diff += llabs((int64_t)(val2 - 2 - myconst));
        diff += llabs((int64_t)(val3 - 3 - myconst));
        diff += llabs((int64_t)(val4 - 4 - myconst));
        diff += llabs((int64_t)(val5 - 5 - myconst));
        diff += llabs((int64_t)(val6 - 6 - myconst));
        diff += llabs((int64_t)(val7 - 7 - myconst));
        diff += llabs((int64_t)(val8 - 8 - myconst));
        diff += llabs((int64_t)(val9 - 9 - myconst));
        if (diff != 0) {
            fails++;
            printf("sscanf output failed\n");
        }
    }
    tests++;

    val0 = val1 = val2 = val3 = val4 = val5 = val6 = val7 = val8 = val9 = 0xff;
    if (vsscanf_wrap("baadf00dcafe baadf00dcaff baadf00dcb00 baadf00dcb01 baadf00dcb02 baadf00dcb03 baadf00dcb04 baadf00dcb05 baadf00dcb06 baadf00dcb07", "%"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64" %"SCNx64, &val0, &val1, &val2, &val3, &val4, &val5, &val6, &val7, &val8, &val9) != 10) {
        fails++;
        printf("vsscanf failed\n");
    } else {
        int64_t diff = 0;
        diff += llabs((int64_t)(val0 - 0 - myconst));
        diff += llabs((int64_t)(val1 - 1 - myconst));
        diff += llabs((int64_t)(val2 - 2 - myconst));
        diff += llabs((int64_t)(val3 - 3 - myconst));
        diff += llabs((int64_t)(val4 - 4 - myconst));
        diff += llabs((int64_t)(val5 - 5 - myconst));
        diff += llabs((int64_t)(val6 - 6 - myconst));
        diff += llabs((int64_t)(val7 - 7 - myconst));
        diff += llabs((int64_t)(val8 - 8 - myconst));
        diff += llabs((int64_t)(val9 - 9 - myconst));
        if (diff != 0) {
            fails++;
            printf("vsscanf output failed\n");
        }
    }
    tests++;

    TEST_FLT(floor(F(3.9)), 3.0);
#if !defined(__MINGW32__) || !defined(__arm__)
    // TODO: floor/ceil on mingw-w64 on arm truncates results to the 32 bit integer range
    TEST_FLT(floor(F(17179869184.0)), 17179869184.0);
#endif
    TEST_FLT(floor(F(-3.3)), -4.0);
    TEST_FLT(floor(F(-3.9)), -4.0);
    TEST_FLT(floor(F(INFINITY)), INFINITY);
    TEST_FLT(floor(F(-INFINITY)), -INFINITY);
    TEST_FLT_NAN(floor(F(NAN)));
    TEST_FLT_NAN(floor(F(-NAN)));
    TEST_FLT_NAN(floor(-F(NAN)));

    TEST_FLT(floorf(F(3.9)), 3.0);
    TEST_FLT(floorf(F(-3.3)), -4.0);
    TEST_FLT(floorf(F(-3.9)), -4.0);
    TEST_FLT(floorf(F(INFINITY)), INFINITY);
    TEST_FLT(floorf(F(-INFINITY)), -INFINITY);
    TEST_FLT_NAN(floorf(F(NAN)));
    TEST_FLT_NAN(floorf(F(-NAN)));
    TEST_FLT_NAN(floorf(-F(NAN)));

    TEST_FLT(floorl(F(3.9)), 3.0);
    TEST_FLT(floorl(F(-3.3)), -4.0);
    TEST_FLT(floorl(F(-3.9)), -4.0);
    TEST_FLT(floorl(F(INFINITY)), INFINITY);
    TEST_FLT(floorl(F(-INFINITY)), -INFINITY);
    TEST_FLT_NAN(floorl(F(NAN)));
    TEST_FLT_NAN(floorl(F(-NAN)));
    TEST_FLT_NAN(floorl(-F(NAN)));

    TEST_FLT(ceil(F(3.9)), 4.0);
    TEST_FLT(ceil(F(-3.3)), -3.0);
    TEST_FLT(ceil(F(-3.9)), -3.0);
    TEST_FLT(ceil(F(INFINITY)), INFINITY);
    TEST_FLT(ceil(F(-INFINITY)), -INFINITY);
    TEST_FLT_NAN(ceil(F(NAN)));
    TEST_FLT_NAN(ceil(F(-NAN)));
    TEST_FLT_NAN(ceil(-F(NAN)));

    TEST_FLT(ceilf(F(3.9)), 4.0);
    TEST_FLT(ceilf(F(-3.3)), -3.0);
    TEST_FLT(ceilf(F(-3.9)), -3.0);
    TEST_FLT(ceilf(F(INFINITY)), INFINITY);
    TEST_FLT(ceilf(F(-INFINITY)), -INFINITY);
    TEST_FLT_NAN(ceilf(F(NAN)));
    TEST_FLT_NAN(ceilf(F(-NAN)));
    TEST_FLT_NAN(ceilf(-F(NAN)));

    TEST_FLT(ceill(F(3.9)), 4.0);
    TEST_FLT(ceill(F(-3.3)), -3.0);
    TEST_FLT(ceill(F(-3.9)), -3.0);
    TEST_FLT(ceill(F(INFINITY)), INFINITY);
    TEST_FLT(ceill(F(-INFINITY)), -INFINITY);
    TEST_FLT_NAN(ceill(F(NAN)));
    TEST_FLT_NAN(ceill(F(-NAN)));
    TEST_FLT_NAN(ceill(-F(NAN)));

    TEST_FLT(trunc(F(3.9)), 3.0);
    TEST_FLT(trunc(F(-3.3)), -3.0);
    TEST_FLT(trunc(F(-3.9)), -3.0);
    TEST_FLT(trunc(F(INFINITY)), INFINITY);
    TEST_FLT(trunc(F(-INFINITY)), -INFINITY);
    TEST_FLT_NAN(trunc(F(NAN)));
    TEST_FLT_NAN(trunc(F(-NAN)));
    TEST_FLT_NAN(trunc(-F(NAN)));

    TEST_FLT(truncf(F(3.9)), 3.0);
    TEST_FLT(truncf(F(-3.3)), -3.0);
    TEST_FLT(truncf(F(-3.9)), -3.0);
    TEST_FLT(truncf(F(INFINITY)), INFINITY);
    TEST_FLT(truncf(F(-INFINITY)), -INFINITY);
    TEST_FLT_NAN(truncf(F(NAN)));
    TEST_FLT_NAN(truncf(F(-NAN)));
    TEST_FLT_NAN(truncf(-F(NAN)));

    TEST_FLT(truncl(F(3.9)), 3.0);
    TEST_FLT(truncl(F(-3.3)), -3.0);
    TEST_FLT(truncl(F(-3.9)), -3.0);
    TEST_FLT(truncl(F(INFINITY)), INFINITY);
    TEST_FLT(truncl(F(-INFINITY)), -INFINITY);
    TEST_FLT_NAN(truncl(F(NAN)));
    TEST_FLT_NAN(truncl(F(-NAN)));
    TEST_FLT_NAN(truncl(-F(NAN)));

    TEST_FLT(sqrt(F(9)), 3.0);
    TEST_FLT(sqrt(F(0.25)), 0.5);
    TEST_FLT(sqrtf(F(9)), 3.0);
    TEST_FLT(sqrtf(F(0.25)), 0.5);
    TEST_FLT(sqrtl(F(9)), 3.0);
    TEST_FLT(sqrtl(F(0.25)), 0.5);

    TEST_FLT(fma(F(2), F(3), F(4)), 10);
    TEST_FLT_NAN(fma(F(NAN), F(3), F(4)));
    TEST_FLT_NAN(fma(F(2), F(NAN), F(4)));
    TEST_FLT_NAN(fma(F(2), F(3), F(NAN)));
    TEST_FLT(fmaf(F(2), F(3), F(4)), 10);
    TEST_FLT_NAN(fmaf(F(NAN), F(3), F(4)));
    TEST_FLT_NAN(fmaf(F(2), F(NAN), F(4)));
    TEST_FLT_NAN(fmaf(F(2), F(3), F(NAN)));
    TEST_FLT(fmal(F(2), F(3), F(4)), 10);
    TEST_FLT_NAN(fmal(F(NAN), F(3), F(4)));
    TEST_FLT_NAN(fmal(F(2), F(NAN), F(4)));
    TEST_FLT_NAN(fmal(F(2), F(3), F(NAN)));

    for (i = 0; i < 2; i++) {
        if (i == 0) {
            // Use the default env in the first round here
            context = "FE_DFL_ENV ";
        } else {
            fesetround(FE_TONEAREST); // Only set it on the second round
            context = "FE_TONEAREST ";
        }

        TEST_INT(llrint(F(3.3)), 3);
        TEST_INT(llrint(F(3.6)), 4);
        TEST_INT(llrint(F(3.5)), 4);
#if !defined(__MINGW32__) || (!defined(__arm__) && !defined(__aarch64__))
        // TODO: FE_TONEAREST rounds towards nearest even number, so 4.5 rounds to 4 - mingw doesn't implement this properly yet on arm and aarch64
        TEST_INT(llrint(F(4.5)), 4);
#endif
        TEST_INT(llrint(F(-3.3)), -3);
        TEST_INT(llrint(F(-3.6)), -4);
        TEST_INT(llrint(F(-3.5)), -4);

        TEST_INT(llrintf(F(3.3)), 3);
        TEST_INT(llrintf(F(3.6)), 4);
        TEST_INT(llrintf(F(3.5)), 4);
        TEST_INT(llrintf(F(-3.3)), -3);
        TEST_INT(llrintf(F(-3.6)), -4);
        TEST_INT(llrintf(F(-3.5)), -4);

        TEST_INT(llrintl(F(3.3)), 3);
        TEST_INT(llrintl(F(3.6)), 4);
        TEST_INT(llrintl(F(3.5)), 4);
        TEST_INT(llrintl(F(-3.3)), -3);
        TEST_INT(llrintl(F(-3.6)), -4);
        TEST_INT(llrintl(F(-3.5)), -4);

        TEST_INT(lrint(F(3.3)), 3);
        TEST_INT(lrint(F(3.6)), 4);
        TEST_INT(lrint(F(3.5)), 4);
        TEST_INT(lrint(F(-3.3)), -3);
        TEST_INT(lrint(F(-3.6)), -4);
        TEST_INT(lrint(F(-3.5)), -4);

        TEST_INT(lrintf(F(3.3)), 3);
        TEST_INT(lrintf(F(3.6)), 4);
        TEST_INT(lrintf(F(3.5)), 4);
        TEST_INT(lrintf(F(-3.3)), -3);
        TEST_INT(lrintf(F(-3.6)), -4);
        TEST_INT(lrintf(F(-3.5)), -4);

        TEST_INT(lrintl(F(3.3)), 3);
        TEST_INT(lrintl(F(3.6)), 4);
        TEST_INT(lrintl(F(3.5)), 4);
        TEST_INT(lrintl(F(-3.3)), -3);
        TEST_INT(lrintl(F(-3.6)), -4);
        TEST_INT(lrintl(F(-3.5)), -4);

        TEST_FLT(rint(F(3.3)), 3.0);
        TEST_FLT(rint(F(3.6)), 4.0);
        TEST_FLT(rint(F(3.5)), 4.0);
        TEST_FLT_NAN(rint(F(NAN)));
        TEST_FLT(rint(F(-3.3)), -3.0);
        TEST_FLT(rint(F(-3.6)), -4.0);
        TEST_FLT(rint(F(-3.5)), -4.0);
        TEST_FLT_NAN(rint(F(-NAN)));

        TEST_FLT(rintf(F(3.3)), 3.0);
        TEST_FLT(rintf(F(3.6)), 4.0);
        TEST_FLT(rintf(F(3.5)), 4.0);
        TEST_FLT_NAN(rintf(F(NAN)));
        TEST_FLT(rintf(F(-3.3)), -3.0);
        TEST_FLT(rintf(F(-3.6)), -4.0);
        TEST_FLT(rintf(F(-3.5)), -4.0);
        TEST_FLT_NAN(rintf(F(-NAN)));

        TEST_FLT(rintl(F(3.3)), 3.0);
        TEST_FLT(rintl(F(3.6)), 4.0);
        TEST_FLT(rintl(F(3.5)), 4.0);
        TEST_FLT_NAN(rintl(F(NAN)));
        TEST_FLT(rintl(F(-3.3)), -3.0);
        TEST_FLT(rintl(F(-3.6)), -4.0);
        TEST_FLT(rintl(F(-3.5)), -4.0);
        TEST_FLT_NAN(rintl(F(-NAN)));

        TEST_FLT(nearbyint(F(3.3)), 3.0);
        TEST_FLT(nearbyint(F(3.6)), 4.0);
        TEST_FLT(nearbyint(F(3.5)), 4.0);
        TEST_FLT_NAN(nearbyint(F(NAN)));
        TEST_FLT(nearbyint(F(-3.3)), -3.0);
        TEST_FLT(nearbyint(F(-3.6)), -4.0);
        TEST_FLT(nearbyint(F(-3.5)), -4.0);
        TEST_FLT_NAN(nearbyint(F(-NAN)));

        TEST_FLT(nearbyintf(F(3.3)), 3.0);
        TEST_FLT(nearbyintf(F(3.6)), 4.0);
        TEST_FLT(nearbyintf(F(3.5)), 4.0);
        TEST_FLT_NAN(nearbyintf(F(NAN)));
        TEST_FLT(nearbyintf(F(-3.3)), -3.0);
        TEST_FLT(nearbyintf(F(-3.6)), -4.0);
        TEST_FLT(nearbyintf(F(-3.5)), -4.0);
        TEST_FLT_NAN(nearbyintf(F(-NAN)));

        TEST_FLT(nearbyintl(F(3.3)), 3.0);
        TEST_FLT(nearbyintl(F(3.6)), 4.0);
        TEST_FLT(nearbyintl(F(3.5)), 4.0);
        TEST_FLT_NAN(nearbyintl(F(NAN)));
        TEST_FLT(nearbyintl(F(-3.3)), -3.0);
        TEST_FLT(nearbyintl(F(-3.6)), -4.0);
        TEST_FLT(nearbyintl(F(-3.5)), -4.0);
        TEST_FLT_NAN(nearbyintl(F(-NAN)));
    }

    fesetround(FE_TOWARDZERO);
    context = "FE_TOWARDZERO ";
    TEST_INT(llrint(F(3.3)), 3);
    TEST_INT(llrint(F(3.6)), 3);
    TEST_INT(llrint(F(-3.3)), -3);
    TEST_INT(llrint(F(-3.6)), -3);

    TEST_INT(llrintf(F(3.3)), 3);
    TEST_INT(llrintf(F(3.6)), 3);
    TEST_INT(llrintf(F(-3.3)), -3);
    TEST_INT(llrintf(F(-3.6)), -3);

    TEST_INT(llrintl(F(3.3)), 3);
    TEST_INT(llrintl(F(3.6)), 3);
    TEST_INT(llrintl(F(-3.3)), -3);
    TEST_INT(llrintl(F(-3.6)), -3);

    TEST_INT(lrint(F(3.3)), 3);
    TEST_INT(lrint(F(3.6)), 3);
    TEST_INT(lrint(F(-3.3)), -3);
    TEST_INT(lrint(F(-3.6)), -3);

    TEST_INT(lrintf(F(3.3)), 3);
    TEST_INT(lrintf(F(3.6)), 3);
    TEST_INT(lrintf(F(-3.3)), -3);
    TEST_INT(lrintf(F(-3.6)), -3);

    TEST_INT(lrintl(F(3.3)), 3);
    TEST_INT(lrintl(F(3.6)), 3);
    TEST_INT(lrintl(F(-3.3)), -3);
    TEST_INT(lrintl(F(-3.6)), -3);

    TEST_FLT(rint(F(3.3)), 3.0);
    TEST_FLT(rint(F(3.6)), 3.0);
    TEST_FLT(rint(F(-3.3)), -3.0);
    TEST_FLT(rint(F(-3.6)), -3.0);

    TEST_FLT(rintf(F(3.3)), 3.0);
    TEST_FLT(rintf(F(3.6)), 3.0);
    TEST_FLT(rintf(F(-3.3)), -3.0);
    TEST_FLT(rintf(F(-3.6)), -3.0);

    TEST_FLT(rintl(F(3.3)), 3.0);
    TEST_FLT(rintl(F(3.6)), 3.0);
    TEST_FLT(rintl(F(-3.3)), -3.0);
    TEST_FLT(rintl(F(-3.6)), -3.0);

    TEST_FLT(nearbyint(F(3.3)), 3.0);
    TEST_FLT(nearbyint(F(3.6)), 3.0);
    TEST_FLT(nearbyint(F(-3.3)), -3.0);
    TEST_FLT(nearbyint(F(-3.6)), -3.0);

    TEST_FLT(nearbyintf(F(3.3)), 3.0);
    TEST_FLT(nearbyintf(F(3.6)), 3.0);
    TEST_FLT(nearbyintf(F(-3.3)), -3.0);
    TEST_FLT(nearbyintf(F(-3.6)), -3.0);

    TEST_FLT(nearbyintl(F(3.3)), 3.0);
    TEST_FLT(nearbyintl(F(3.6)), 3.0);
    TEST_FLT(nearbyintl(F(-3.3)), -3.0);
    TEST_FLT(nearbyintl(F(-3.6)), -3.0);

    fesetround(FE_DOWNWARD);
    context = "FE_DOWNWARD ";
    TEST_INT(llrint(F(3.3)), 3);
    TEST_INT(llrint(F(3.6)), 3);
    TEST_INT(llrint(F(-3.3)), -4);
    TEST_INT(llrint(F(-3.6)), -4);

    TEST_INT(llrintf(F(3.3)), 3);
    TEST_INT(llrintf(F(3.6)), 3);
    TEST_INT(llrintf(F(-3.3)), -4);
    TEST_INT(llrintf(F(-3.6)), -4);

    TEST_INT(llrintl(F(3.3)), 3);
    TEST_INT(llrintl(F(3.6)), 3);
    TEST_INT(llrintl(F(-3.3)), -4);
    TEST_INT(llrintl(F(-3.6)), -4);

    TEST_INT(lrint(F(3.3)), 3);
    TEST_INT(lrint(F(3.6)), 3);
    TEST_INT(lrint(F(-3.3)), -4);
    TEST_INT(lrint(F(-3.6)), -4);

    TEST_INT(lrintf(F(3.3)), 3);
    TEST_INT(lrintf(F(3.6)), 3);
    TEST_INT(lrintf(F(-3.3)), -4);
    TEST_INT(lrintf(F(-3.6)), -4);

    TEST_INT(lrintl(F(3.3)), 3);
    TEST_INT(lrintl(F(3.6)), 3);
    TEST_INT(lrintl(F(-3.3)), -4);
    TEST_INT(lrintl(F(-3.6)), -4);

    TEST_FLT(rint(F(3.3)), 3.0);
    TEST_FLT(rint(F(3.6)), 3.0);
    TEST_FLT(rint(F(-3.3)), -4.0);
    TEST_FLT(rint(F(-3.6)), -4.0);

    TEST_FLT(rintf(F(3.3)), 3.0);
    TEST_FLT(rintf(F(3.6)), 3.0);
    TEST_FLT(rintf(F(-3.3)), -4.0);
    TEST_FLT(rintf(F(-3.6)), -4.0);

    TEST_FLT(rintl(F(3.3)), 3.0);
    TEST_FLT(rintl(F(3.6)), 3.0);
    TEST_FLT(rintl(F(-3.3)), -4.0);
    TEST_FLT(rintl(F(-3.6)), -4.0);

    TEST_FLT(nearbyint(F(3.3)), 3.0);
    TEST_FLT(nearbyint(F(3.6)), 3.0);
    TEST_FLT(nearbyint(F(-3.3)), -4.0);
    TEST_FLT(nearbyint(F(-3.6)), -4.0);

    TEST_FLT(nearbyintf(F(3.3)), 3.0);
    TEST_FLT(nearbyintf(F(3.6)), 3.0);
    TEST_FLT(nearbyintf(F(-3.3)), -4.0);
    TEST_FLT(nearbyintf(F(-3.6)), -4.0);

    TEST_FLT(nearbyintl(F(3.3)), 3.0);
    TEST_FLT(nearbyintl(F(3.6)), 3.0);
    TEST_FLT(nearbyintl(F(-3.3)), -4.0);
    TEST_FLT(nearbyintl(F(-3.6)), -4.0);

    fesetround(FE_UPWARD);
    context = "FE_UPWARD ";
    TEST_INT(llrint(F(3.3)), 4);
    TEST_INT(llrint(F(3.6)), 4);
    TEST_INT(llrint(F(-3.3)), -3);
    TEST_INT(llrint(F(-3.6)), -3);

    TEST_INT(llrintf(F(3.3)), 4);
    TEST_INT(llrintf(F(3.6)), 4);
    TEST_INT(llrintf(F(-3.3)), -3);
    TEST_INT(llrintf(F(-3.6)), -3);

    TEST_INT(llrintl(F(3.3)), 4);
    TEST_INT(llrintl(F(3.6)), 4);
    TEST_INT(llrintl(F(-3.3)), -3);
    TEST_INT(llrintl(F(-3.6)), -3);

    TEST_INT(lrint(F(3.3)), 4);
    TEST_INT(lrint(F(3.6)), 4);
    TEST_INT(lrint(F(-3.3)), -3);
    TEST_INT(lrint(F(-3.6)), -3);

    TEST_INT(lrintf(F(3.3)), 4);
    TEST_INT(lrintf(F(3.6)), 4);
    TEST_INT(lrintf(F(-3.3)), -3);
    TEST_INT(lrintf(F(-3.6)), -3);

    TEST_INT(lrintl(F(3.3)), 4);
    TEST_INT(lrintl(F(3.6)), 4);
    TEST_INT(lrintl(F(-3.3)), -3);
    TEST_INT(lrintl(F(-3.6)), -3);

    TEST_FLT(rint(F(3.3)), 4.0);
    TEST_FLT(rint(F(3.6)), 4.0);
    TEST_FLT(rint(F(-3.3)), -3.0);
    TEST_FLT(rint(F(-3.6)), -3.0);

    TEST_FLT(rintf(F(3.3)), 4.0);
    TEST_FLT(rintf(F(3.6)), 4.0);
    TEST_FLT(rintf(F(-3.3)), -3.0);
    TEST_FLT(rintf(F(-3.6)), -3.0);

    TEST_FLT(rintl(F(3.3)), 4.0);
    TEST_FLT(rintl(F(3.6)), 4.0);
    TEST_FLT(rintl(F(-3.3)), -3.0);
    TEST_FLT(rintl(F(-3.6)), -3.0);

    TEST_FLT(nearbyint(F(3.3)), 4.0);
    TEST_FLT(nearbyint(F(3.6)), 4.0);
    TEST_FLT(nearbyint(F(-3.3)), -3.0);
    TEST_FLT(nearbyint(F(-3.6)), -3.0);

    TEST_FLT(nearbyintf(F(3.3)), 4.0);
    TEST_FLT(nearbyintf(F(3.6)), 4.0);
    TEST_FLT(nearbyintf(F(-3.3)), -3.0);
    TEST_FLT(nearbyintf(F(-3.6)), -3.0);

    TEST_FLT(nearbyintl(F(3.3)), 4.0);
    TEST_FLT(nearbyintl(F(3.6)), 4.0);
    TEST_FLT(nearbyintl(F(-3.3)), -3.0);
    TEST_FLT(nearbyintl(F(-3.6)), -3.0);

    context = "";

    TEST_FLT_ACCURACY(log2(F(1.0)), 0.0, 0.001);
    TEST_FLT_ACCURACY(log2(F(8.0)), 3.0, 0.001);
    TEST_FLT_ACCURACY(log2(F(1024.0)), 10.0, 0.001);
    TEST_FLT_ACCURACY(log2(F(1048576.0)), 20.0, 0.001);
    TEST_FLT_ACCURACY(log2(F(4294967296.0)), 32.0, 0.001);
    TEST_FLT_ACCURACY(log2(F(9.7656e-04)), -10, 0.001);
    TEST_FLT_ACCURACY(log2(F(9.5367e-07)), -20, 0.001);
    TEST_FLT_ACCURACY(log2(F(3.5527e-15)), -48, 0.001);
    TEST_FLT_ACCURACY(log2(F(7.8886e-31)), -100, 0.001);
    TEST_FLT_ACCURACY(log2(F(7.3468e-40)), -130, 0.001);
#if !defined(__MINGW32__) || !defined(__arm__)
    // on mingw-arm, this gives -inf
    TEST_FLT_ACCURACY(log2(F(9.8813e-324)), -1073, 0.001);
#endif
    TEST_FLT_ACCURACY(log2(F(1.225000)), 0.292782, 0.001);

    TEST_FLT_ACCURACY(log2f(F(1.0)), 0.0, 0.001);
    TEST_FLT_ACCURACY(log2f(F(8.0)), 3.0, 0.001);
    TEST_FLT_ACCURACY(log2f(F(1024.0)), 10.0, 0.001);
    TEST_FLT_ACCURACY(log2f(F(1048576.0)), 20.0, 0.001);
    TEST_FLT_ACCURACY(log2f(F(4294967296.0)), 32.0, 0.001);
    TEST_FLT_ACCURACY(log2f(F(9.7656e-04)), -10, 0.001);
    TEST_FLT_ACCURACY(log2f(F(9.5367e-07)), -20, 0.001);
    TEST_FLT_ACCURACY(log2f(F(3.5527e-15)), -48, 0.001);
    TEST_FLT_ACCURACY(log2f(F(7.8886e-31)), -100, 0.001);
    TEST_FLT_ACCURACY(log2f(F(7.3468e-40)), -130, 0.001);
    TEST_FLT_ACCURACY(log2f(F(7.1746e-43)), -140, 0.001);
    TEST_FLT_ACCURACY(log2f(F(1.225000)), 0.292782, 0.001); // This crashes the mingw-w64 softfloat implementation

    TEST_INT(llround(F(3.3)), 3);
    TEST_INT(llround(F(3.6)), 4);
    TEST_INT(llround(F(3.5)), 4);
    TEST_INT(llround(F(4.5)), 5);
    TEST_INT(llround(F(-3.3)), -3);
    TEST_INT(llround(F(-3.6)), -4);
    TEST_INT(llround(F(-3.5)), -4);
    TEST_INT(llround(F(-4.5)), -5);

    TEST_INT(llroundf(F(3.3)), 3);
    TEST_INT(llroundf(F(3.6)), 4);
    TEST_INT(llroundf(F(3.5)), 4);
    TEST_INT(llroundf(F(4.5)), 5);
    TEST_INT(llroundf(F(-3.3)), -3);
    TEST_INT(llroundf(F(-3.6)), -4);
    TEST_INT(llroundf(F(-3.5)), -4);
    TEST_INT(llroundf(F(-4.5)), -5);

    TEST_INT(llroundl(F(3.3)), 3);
    TEST_INT(llroundl(F(3.6)), 4);
    TEST_INT(llroundl(F(3.5)), 4);
    TEST_INT(llroundl(F(4.5)), 5);
    TEST_INT(llroundl(F(-3.3)), -3);
    TEST_INT(llroundl(F(-3.6)), -4);
    TEST_INT(llroundl(F(-3.5)), -4);
    TEST_INT(llroundl(F(-4.5)), -5);

    TEST_INT(lround(F(3.3)), 3);
    TEST_INT(lround(F(3.6)), 4);
    TEST_INT(lround(F(3.5)), 4);
    TEST_INT(lround(F(4.5)), 5);
    TEST_INT(lround(F(-3.3)), -3);
    TEST_INT(lround(F(-3.6)), -4);
    TEST_INT(lround(F(-3.5)), -4);
    TEST_INT(lround(F(-4.5)), -5);

    TEST_INT(lroundf(F(3.3)), 3);
    TEST_INT(lroundf(F(3.6)), 4);
    TEST_INT(lroundf(F(3.5)), 4);
    TEST_INT(lroundf(F(4.5)), 5);
    TEST_INT(lroundf(F(-3.3)), -3);
    TEST_INT(lroundf(F(-3.6)), -4);
    TEST_INT(lroundf(F(-3.5)), -4);
    TEST_INT(lroundf(F(-4.5)), -5);

    TEST_INT(lroundl(F(3.3)), 3);
    TEST_INT(lroundl(F(3.6)), 4);
    TEST_INT(lroundl(F(3.5)), 4);
    TEST_INT(lroundl(F(4.5)), 5);
    TEST_INT(lroundl(F(-3.3)), -3);
    TEST_INT(lroundl(F(-3.6)), -4);
    TEST_INT(lroundl(F(-3.5)), -4);
    TEST_INT(lroundl(F(-4.5)), -5);

    TEST_FLT(round(F(3.3)), 3.0);
    TEST_FLT(round(F(3.6)), 4.0);
    TEST_FLT(round(F(3.5)), 4.0);
    TEST_FLT(round(F(4.5)), 5.0);
    TEST_FLT_NAN(round(F(NAN)));
    TEST_FLT(round(F(-3.3)), -3.0);
    TEST_FLT(round(F(-3.6)), -4.0);
    TEST_FLT(round(F(-3.5)), -4.0);
    TEST_FLT(round(F(-4.5)), -5.0);
    TEST_FLT_NAN(round(F(-NAN)));

    TEST_FLT(roundf(F(3.3)), 3.0);
    TEST_FLT(roundf(F(3.6)), 4.0);
    TEST_FLT(roundf(F(3.5)), 4.0);
    TEST_FLT(roundf(F(4.5)), 5.0);
    TEST_FLT_NAN(roundf(F(NAN)));
    TEST_FLT(roundf(F(-3.3)), -3.0);
    TEST_FLT(roundf(F(-3.6)), -4.0);
    TEST_FLT(roundf(F(-3.5)), -4.0);
    TEST_FLT(roundf(F(-4.5)), -5.0);
    TEST_FLT_NAN(roundf(F(-NAN)));

    TEST_FLT(roundl(F(3.3)), 3.0);
    TEST_FLT(roundl(F(3.6)), 4.0);
    TEST_FLT(roundl(F(3.5)), 4.0);
    TEST_FLT(roundl(F(4.5)), 5.0);
    TEST_FLT_NAN(roundl(F(NAN)));
    TEST_FLT(roundl(F(-3.3)), -3.0);
    TEST_FLT(roundl(F(-3.6)), -4.0);
    TEST_FLT(roundl(F(-3.5)), -4.0);
    TEST_FLT(roundl(F(-4.5)), -5.0);
    TEST_FLT_NAN(roundl(F(-NAN)));

    TEST_FLT(pow(F(2.0), F(0.0)), 1.0);
    TEST_FLT(powf(F(2.0), F(0.0)), 1.0);
    TEST_FLT(pow(F(10.0), F(0.0)), 1.0);
    TEST_FLT(pow(F(10.0), F(1.0)), 10.0);
    TEST_FLT_ACCURACY(pow(F(10.0), F(0.5)), 3.162278, 0.01);
#if !defined(__MINGW32__) || (!defined(__arm__) && !defined(__aarch64__))
    // TODO: Missing on mingw on arm and aarch64
    TEST_FLT(powl(F(2.0), F(0.0)), 1.0);
#endif

    TEST_FLT_ACCURACY(cos(F(0.0)), 1.0, 0.01);
    TEST_FLT_ACCURACY(sin(F(0.0)), 0.0, 0.01);

#ifdef _WIN32
    TEST_FLT_ACCURACY(_copysign(F(3.125), F(1)), 3.125, 0.0001);
    TEST_FLT_ACCURACY(_copysign(F(3.125), F(-1)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(_copysign(F(-3.125), F(-1)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(_copysign(F(-3.125), F(1)), 3.125, 0.0001);
    TEST_FLT_NAN(_copysign(F(NAN), F(-1)));
    TEST_FLT(_copysign(F(INFINITY), F(1)), INFINITY);
    TEST_FLT(_copysign(F(INFINITY), F(-1)), -INFINITY);
    TEST_FLT(_copysign(F(-INFINITY), F(-1)), -INFINITY);
    TEST_FLT(_copysign(F(-INFINITY), F(1)), INFINITY);
#if !defined(__MINGW32__) || (!defined(__i386__) || __MSVCRT_VERSION__ >= 0x1400)
    // The _copysignf function is missing in msvcrt.dll on i386
    TEST_FLT_ACCURACY(_copysignf(F(3.125), F(1)), 3.125, 0.0001);
    TEST_FLT_ACCURACY(_copysignf(F(3.125), F(-1)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(_copysignf(F(-3.125), F(-1)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(_copysignf(F(-3.125), F(1)), 3.125, 0.0001);
    TEST_FLT(_copysignf(F(INFINITY), F(1)), INFINITY);
    TEST_FLT(_copysignf(F(INFINITY), F(-1)), -INFINITY);
    TEST_FLT(_copysignf(F(-INFINITY), F(-1)), -INFINITY);
    TEST_FLT(_copysignf(F(-INFINITY), F(1)), INFINITY);
    TEST_FLT_NAN(_copysignf(F(NAN), F(-1)));
#endif
    TEST_FLT_ACCURACY(_copysignl(F(3.125), F(1)), 3.125, 0.0001);
    TEST_FLT_ACCURACY(_copysignl(F(-3.125), F(-1)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(_copysignl(F(3.125), F(-1)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(_copysignl(F(-3.125), F(1)), 3.125, 0.0001);
    TEST_FLT(_copysignl(F(INFINITY), F(1)), INFINITY);
    TEST_FLT(_copysignl(F(INFINITY), F(-1)), -INFINITY);
    TEST_FLT(_copysignl(F(-INFINITY), F(-1)), -INFINITY);
    TEST_FLT(_copysignl(F(-INFINITY), F(1)), INFINITY);
    TEST_FLT_NAN(_copysignl(F(NAN), F(-1)));
    TEST_FLT_ACCURACY(copysign(F(3.125), F(1)), 3.125, 0.0001);
    TEST_FLT_ACCURACY(copysign(F(3.125), F(-1)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(copysign(F(-3.125), F(-1)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(copysign(F(-3.125), F(1)), 3.125, 0.0001);
    TEST_FLT(copysign(F(INFINITY), F(1)), INFINITY);
    TEST_FLT(copysign(F(INFINITY), F(-1)), -INFINITY);
    TEST_FLT(copysign(F(-INFINITY), F(-1)), -INFINITY);
    TEST_FLT(copysign(F(-INFINITY), F(1)), INFINITY);
    TEST_FLT_NAN(copysign(F(NAN), F(-1)));
    TEST_FLT_ACCURACY(copysignf(F(3.125), F(1)), 3.125, 0.0001);
    TEST_FLT_ACCURACY(copysignf(F(3.125), F(-1)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(copysignf(F(-3.125), F(-1)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(copysignf(F(-3.125), F(1)), 3.125, 0.0001);
    TEST_FLT(copysignf(F(INFINITY), F(1)), INFINITY);
    TEST_FLT(copysignf(F(INFINITY), F(-1)), -INFINITY);
    TEST_FLT(copysignf(F(-INFINITY), F(-1)), -INFINITY);
    TEST_FLT(copysignf(F(-INFINITY), F(1)), INFINITY);
    TEST_FLT_NAN(copysignf(F(NAN), F(-1)));
    TEST_FLT_ACCURACY(copysignl(F(3.125), F(1)), 3.125, 0.0001);
    TEST_FLT_ACCURACY(copysignl(F(-3.125), F(-1)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(copysignl(F(3.125), F(-1)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(copysignl(F(-3.125), F(1)), 3.125, 0.0001);
    TEST_FLT(copysignl(F(INFINITY), F(1)), INFINITY);
    TEST_FLT(copysignl(F(INFINITY), F(-1)), -INFINITY);
    TEST_FLT(copysignl(F(-INFINITY), F(-1)), -INFINITY);
    TEST_FLT(copysignl(F(-INFINITY), F(1)), INFINITY);
    TEST_FLT_NAN(copysignl(F(NAN), F(-1)));

    TEST_FLT_ACCURACY(_chgsignl(F(3.125)), -3.125, 0.0001);
    TEST_FLT_ACCURACY(_chgsignl(F(-3.125)), 3.125, 0.0001);
    TEST_FLT(_chgsignl(F(INFINITY)), -INFINITY);
    TEST_FLT(_chgsignl(F(-INFINITY)), INFINITY);
    TEST_FLT_NAN(_chgsignl(F(NAN)));
#endif

    TEST_INT(L(7) / L(7), 1); // __rt_sdiv
    TEST_INT(L(-7) / L(7), -1); // __rt_sdiv
    TEST_INT(L(-7) / L(-7), 1); // __rt_sdiv
    TEST_INT(L(7) / L(-7), -1); // __rt_sdiv
    TEST_INT(L(1073741824) / L(3), 357913941); // __rt_sdiv
    TEST_INT(L(0) / L(3), 0); // __rt_sdiv
    TEST_INT(L(0) / L(-3), 0); // __rt_sdiv
    TEST_INT(L(1024) / L(357913941), 0); // __rt_sdiv
    TEST_INT(L(1073741824) / L(357913941), 3); // __rt_sdiv
    TEST_INT(L(2147483647) / L(1), 2147483647); // __rt_sdiv
    TEST_INT(L(2147483647) / L(-1), -2147483647); // __rt_sdiv
    TEST_INT(L(-2147483648) / L(1), (long) -2147483648LL); // __rt_sdiv

    TEST_INT(UL(7) / L(7), 1); // __rt_udiv
    TEST_INT(UL(4294967289) / L(7), 613566755); // __rt_udiv
    TEST_INT(UL(4294967289) / L(1), 4294967289UL); // __rt_udiv
    TEST_INT(UL(1073741824) / L(3), 357913941); // __rt_udiv
    TEST_INT(UL(0) / L(3), 0); // __rt_udiv
    TEST_INT(UL(1024) / L(357913941), 0); // __rt_udiv
    TEST_INT(UL(1073741824) / L(357913941), 3); // __rt_udiv
    TEST_INT(UL(2147483647) / L(1), 2147483647); // __rt_udiv

    TEST_INT(LL(7) / 7, 1); // __rt_sdiv64
    TEST_INT(LL(-7) / 7, -1); // __rt_sdiv64
    TEST_INT(LL(-7) / -7, 1); // __rt_sdiv64
    TEST_INT(LL(7) / -7, -1); // __rt_sdiv64
    TEST_INT(LL(1073741824) / 3, 357913941); // __rt_sdiv64
    TEST_INT(LL(0) / 3, 0); // __rt_sdiv64
    TEST_INT(LL(0) / -3, 0); // __rt_sdiv64
    TEST_INT(LL(1024) / 357913941, 0); // __rt_sdiv64
    TEST_INT(LL(1073741824) / 357913941, 3); // __rt_sdiv64
    TEST_INT(LL(2147483647) / LL(1), 2147483647); // __rt_sdiv64
    TEST_INT(LL(2147483647) / LL(-1), -2147483647); // __rt_sdiv64
    TEST_INT(LL(-2147483648) / LL(1), -2147483648LL); // __rt_sdiv64
    TEST_INT(LL(0) / LL(2305843009213693952), 0); // __rt_sdiv64
    TEST_INT(LL(0) / LL(2305843009213693953), 0); // __rt_sdiv64
    TEST_INT(LL(0) / LL(2147483648), 0); // __rt_sdiv64
    TEST_INT(LL(0) / LL(4294967296), 0); // __rt_sdiv64
    TEST_INT(LL(4294967296) / LL(4294967296), 1); // __rt_sdiv64
    TEST_INT(LL(4294967295) / LL(4294967296), 0); // __rt_sdiv64

    TEST_INT(ULL(7) / 7, 1); // __rt_udiv64
    TEST_INT(ULL(4294967289) / LL(7), 613566755); // __rt_udiv64
    TEST_INT(ULL(4294967289) / LL(1), 4294967289ULL); // __rt_udiv64
    TEST_INT(ULL(1073741824) / LL(3), 357913941); // __rt_udiv64
    TEST_INT(ULL(0) / LL(3), 0); // __rt_udiv64
    TEST_INT(ULL(1024) / LL(357913941), 0); // __rt_udiv64
    TEST_INT(ULL(1073741824) / LL(357913941), 3); // __rt_udiv64
    TEST_INT(ULL(2147483647) / LL(1), 2147483647); // __rt_udiv64
    TEST_INT(ULL(18446744073709551615) / LL(1), 18446744073709551615ULL); // __rt_udiv64
    TEST_INT(ULL(0) / ULL(2305843009213693952), 0); // __rt_udiv64
    TEST_INT(ULL(0) / ULL(2305843009213693953), 0); // __rt_udiv64
    TEST_INT(ULL(0) / ULL(2147483648), 0); // __rt_udiv64
    TEST_INT(ULL(0) / ULL(4294967296), 0); // __rt_udiv64
    TEST_INT(ULL(4294967296) / ULL(4294967296), 1); // __rt_udiv64
    TEST_INT(ULL(4294967297) / ULL(8589934593), 0); // __rt_udiv64


    TEST_INT(L(7) % L(7), 0); // __rt_sdiv
    TEST_INT(L(-7) % L(7), 0); // __rt_sdiv
    TEST_INT(L(-7) % L(-7), 0); // __rt_sdiv
    TEST_INT(L(7) % L(-7), 0); // __rt_sdiv
    TEST_INT(L(1073741824) % L(3), 1); // __rt_sdiv
    TEST_INT(L(0) % L(3), 0); // __rt_sdiv
    TEST_INT(L(0) % L(-3), 0); // __rt_sdiv
    TEST_INT(L(1024) % L(357913941), 1024); // __rt_sdiv
    TEST_INT(L(1073741824) % L(357913941), 1); // __rt_sdiv
    TEST_INT(L(2147483647) % L(1), 0); // __rt_sdiv
    TEST_INT(L(2147483647) % L(-1), 0); // __rt_sdiv
    TEST_INT(L(-2147483648) % L(1), 0); // __rt_sdiv

    TEST_INT(UL(7) % L(7), 0); // __rt_udiv
    TEST_INT(UL(4294967289) % L(7), 4); // __rt_udiv
    TEST_INT(UL(4294967289) % L(1), 0); // __rt_udiv
    TEST_INT(UL(1073741824) % L(3), 1); // __rt_udiv
    TEST_INT(UL(0) % L(3), 0); // __rt_udiv
    TEST_INT(UL(1024) % L(357913941), 1024); // __rt_udiv
    TEST_INT(UL(1073741824) % L(357913941), 1); // __rt_udiv
    TEST_INT(UL(2147483647) % L(1), 0); // __rt_udiv

    TEST_INT(LL(7) % 7, 0); // __rt_sdiv64
    TEST_INT(LL(-7) % 7, 0); // __rt_sdiv64
    TEST_INT(LL(-7) % -7, 0); // __rt_sdiv64
    TEST_INT(LL(7) % -7, 0); // __rt_sdiv64
    TEST_INT(LL(1073741824) % 3, 1); // __rt_sdiv64
    TEST_INT(LL(0) % 3, 0); // __rt_sdiv64
    TEST_INT(LL(0) % -3, 0); // __rt_sdiv64
    TEST_INT(LL(1024) % 357913941, 1024); // __rt_sdiv64
    TEST_INT(LL(1073741824) % 357913941, 1); // __rt_sdiv64
    TEST_INT(LL(2147483647) % LL(1), 0); // __rt_sdiv64
    TEST_INT(LL(2147483647) % LL(-1), 0); // __rt_sdiv64
    TEST_INT(LL(-2147483648) % LL(1), 0); // __rt_sdiv64
    TEST_INT(LL(0) % LL(2305843009213693952), 0); // __rt_sdiv64
    TEST_INT(LL(0) % LL(2305843009213693953), 0); // __rt_sdiv64
    TEST_INT(LL(0) % LL(2147483648), 0); // __rt_sdiv64
    TEST_INT(LL(0) % LL(4294967296), 0); // __rt_sdiv64
    TEST_INT(LL(4294967296) % LL(4294967296), 0); // __rt_sdiv64
    TEST_INT(LL(4294967295) % LL(4294967296), 4294967295LL); // __rt_sdiv64

    TEST_INT(ULL(7) % 7, 0); // __rt_udiv64
    TEST_INT(ULL(4294967289) % LL(7), 4); // __rt_udiv64
    TEST_INT(ULL(4294967289) % LL(1), 0); // __rt_udiv64
    TEST_INT(ULL(1073741824) % LL(3), 1); // __rt_udiv64
    TEST_INT(ULL(0) % LL(3), 0); // __rt_udiv64
    TEST_INT(ULL(1024) % LL(357913941), 1024); // __rt_udiv64
    TEST_INT(ULL(1073741824) % LL(357913941), 1); // __rt_udiv64
    TEST_INT(ULL(2147483647) % LL(1), 0); // __rt_udiv64
    TEST_INT(ULL(18446744073709551615) % LL(1), 0); // __rt_udiv64
    TEST_INT(ULL(0) % ULL(2305843009213693952), 0); // __rt_udiv64
    TEST_INT(ULL(0) % ULL(2305843009213693953), 0); // __rt_udiv64
    TEST_INT(ULL(0) % ULL(2147483648), 0); // __rt_udiv64
    TEST_INT(ULL(0) % ULL(4294967296), 0); // __rt_udiv64
    TEST_INT(ULL(4294967296) % ULL(4294967296), 0); // __rt_udiv64
    TEST_INT(ULL(4294967297) % ULL(8589934593), 4294967297ULL); // __rt_udiv64

    TEST_INT((unsigned long long)F(4.2), 4);
    TEST_INT((signed long long)F(4.2), 4);
    TEST_INT((unsigned long long)F(123456789012345678), 123456789012345680ULL);
    TEST_INT((signed long long)F(123456789012345678), 123456789012345680ULL);
    TEST_INT((signed long long)F(-123456789012345), -123456789012345LL);

    TEST_INT((unsigned long long)(float)F(4.2), 4);
    TEST_INT((signed long long)(float)F(4.2), 4);
    TEST_INT((unsigned long long)(float)F(274877906944), 274877906944ULL);
    TEST_INT((signed long long)(float)F(274877906944), 274877906944ULL);
    TEST_INT((signed long long)(float)F(-274877906944), -274877906944LL);

    TEST_FLT((double)LL(4), 4.0);
    TEST_FLT((float)LL(4), 4.0);
    TEST_FLT((double)LL(123456789012345), 123456789012345.0);
    TEST_FLT((double)LL(-123456789012345), -123456789012345.0);
    TEST_FLT((float)LL(274877906944), 274877906944.0);
    TEST_FLT((float)LL(-274877906944), -274877906944.0);

    TEST_FLT((double)ULL(4), 4.0);
    TEST_FLT((float)ULL(4), 4.0);
    TEST_FLT((double)ULL(17293822569102704640), 17293822569102704640.0);
    TEST_FLT((float)ULL(17293822569102704640), 17293822569102704640.0);

#ifdef _WIN32
    long value = 0;
    __int64 ret;
    __int64 value64 = 0;
    void *ptr = NULL;
    void *ptr1 = &value;
    void *ptr2 = &value64;
    void *ret_ptr;
#define TEST_FUNC(expr, var, expected, expected_ret) do { \
        ret = expr; \
        TEST_INT(var, expected); \
        TEST_INT(ret, expected_ret); \
        var = expected; \
    } while (0)
#define TEST_FUNC_PTR(expr, var, expected, expected_ret) do { \
        ret_ptr = expr; \
        TEST_PTR(var, expected); \
        TEST_PTR(ret_ptr, expected_ret); \
        var = expected; \
    } while (0)
    TEST_FUNC(InterlockedBitTestAndSet(&value, 0), value, 1, 0);
    TEST_FUNC(InterlockedBitTestAndSet(&value, 2), value, 5, 0);
    TEST_FUNC(InterlockedBitTestAndSet(&value, 2), value, 5, 1);
    TEST_FUNC(InterlockedBitTestAndReset(&value, 2), value, 1, 1);
    TEST_FUNC(InterlockedBitTestAndReset(&value, 2), value, 1, 0);
    TEST_FUNC(InterlockedBitTestAndReset(&value, 0), value, 0, 1);
#ifdef _WIN64
    TEST_FUNC(InterlockedBitTestAndSet64(&value64, 0), value64, 1, 0);
    TEST_FUNC(InterlockedBitTestAndSet64(&value64, 2), value64, 5, 0);
    TEST_FUNC(InterlockedBitTestAndSet64(&value64, 2), value64, 5, 1);
    TEST_FUNC(InterlockedBitTestAndSet64(&value64, 40), value64, 0x10000000005, 0);
    TEST_FUNC(InterlockedBitTestAndReset64(&value64, 40), value64, 5, 1);
    TEST_FUNC(InterlockedBitTestAndReset64(&value64, 2), value64, 1, 1);
    TEST_FUNC(InterlockedBitTestAndReset64(&value64, 2), value64, 1, 0);
    TEST_FUNC(InterlockedBitTestAndReset64(&value64, 0), value64, 0, 1);
#endif
    TEST_FUNC(InterlockedIncrement(&value), value, 1, 1);
    TEST_FUNC(InterlockedDecrement(&value), value, 0, 0);
    TEST_FUNC(InterlockedAdd(&value, 7), value, 7, 7);
    TEST_FUNC(InterlockedAdd(&value, -2), value, 5, 5);
    TEST_FUNC(InterlockedAdd64(&value64, 7), value64, 7, 7);
    TEST_FUNC(InterlockedAdd64(&value64, 0x10000000000), value64, 0x10000000007, 0x10000000007);
    TEST_FUNC(InterlockedIncrement64(&value64), value64, 0x10000000008, 0x10000000008);
    TEST_FUNC(InterlockedDecrement64(&value64), value64, 0x10000000007, 0x10000000007);
    TEST_FUNC(InterlockedAdd64(&value64, -0x10000000002), value64, 5, 5);
    // Exchange functions return the previous value
    TEST_FUNC(InterlockedExchangeAdd(&value, 1), value, 6, 5);
    TEST_FUNC(InterlockedExchange(&value, 2), value, 2, 6);
    TEST_FUNC(InterlockedCompareExchange(&value, 7, 1), value, 2, 2);
    TEST_FUNC(InterlockedCompareExchange(&value, 5, 2), value, 5, 2);
    TEST_FUNC_PTR(InterlockedExchangePointer(&ptr, ptr1), ptr, ptr1, NULL);
    TEST_FUNC_PTR(InterlockedExchangePointer(&ptr, ptr2), ptr, ptr2, ptr1);
    TEST_FUNC_PTR(InterlockedCompareExchangePointer(&ptr, NULL, ptr1), ptr, ptr2, ptr2);
    TEST_FUNC_PTR(InterlockedCompareExchangePointer(&ptr, NULL, ptr2), ptr, NULL, ptr2);
    TEST_FUNC(InterlockedExchangeAdd64(&value64, 0x10000000000), value64, 0x10000000005, 5);
    TEST_FUNC(InterlockedExchange64(&value64, 0x10000000000), value64, 0x10000000000, 0x10000000005);
    TEST_FUNC(InterlockedCompareExchange64(&value64, 7, 1), value64, 0x10000000000, 0x10000000000);
    TEST_FUNC(InterlockedCompareExchange64(&value64, 0x20000000005, 0x10000000000), value64, 0x20000000005, 0x10000000000);
    // Logical operations returns the previous value
    TEST_FUNC(InterlockedOr(&value, 2), value, 7, 5);
    TEST_FUNC(InterlockedOr(&value, 2), value, 7, 7);
    TEST_FUNC(InterlockedAnd(&value, 2), value, 2, 7);
    TEST_FUNC(InterlockedAnd(&value, 2), value, 2, 2);
    TEST_FUNC(InterlockedXor(&value, 2), value, 0, 2);
    TEST_FUNC(InterlockedXor(&value, 2), value, 2, 0);
    TEST_FUNC(InterlockedXor(&value, 2), value, 0, 2);
    TEST_FUNC(InterlockedOr64(&value64, 2), value64, 0x20000000007, 0x20000000005);
    TEST_FUNC(InterlockedOr64(&value64, 0x10000000000), value64, 0x30000000007, 0x20000000007);
    TEST_FUNC(InterlockedAnd64(&value64, 0x20000000000), value64, 0x20000000000, 0x30000000007);
    TEST_FUNC(InterlockedAnd64(&value64, 0x20000000000), value64, 0x20000000000, 0x20000000000);
    TEST_FUNC(InterlockedXor64(&value64, 0x20000000000), value64, 0, 0x20000000000);
    TEST_FUNC(InterlockedXor64(&value64, 0x20000000000), value64, 0x20000000000, 0);
    TEST_FUNC(InterlockedXor64(&value64, 0x20000000000), value64, 0, 0x20000000000);

    unsigned long idx = 42;
    // If no bit is set, idx is set to an undefined value.
    TEST_INT(BitScanForward(&idx, UL(0)), 0);
    TEST_FUNC(BitScanForward(&idx, UL(1)), idx, 0, 1);
    TEST_FUNC(BitScanForward(&idx, UL(0x80000000)), idx, 31, 1);
    TEST_FUNC(BitScanForward(&idx, UL(0x80000001)), idx, 0, 1);
    TEST_INT(BitScanReverse(&idx, UL(0)), 0);
    TEST_FUNC(BitScanReverse(&idx, UL(1)), idx, 0, 1);
    TEST_FUNC(BitScanReverse(&idx, UL(0x80000000)), idx, 31, 1);
    TEST_FUNC(BitScanReverse(&idx, UL(0x80000001)), idx, 31, 1);
#if !defined(_M_ARM) && !defined(__arm__) && !defined(__i386__)
    // These seem to be unavailable on 32 bit arm even in MSVC. They're also missing
    // on i386 mingw.
    TEST_INT(BitScanForward64(&idx, UL(0)), 0);
    TEST_FUNC(BitScanForward64(&idx, UL(1)), idx, 0, 1);
    TEST_FUNC(BitScanForward64(&idx, UL(0x80000000)), idx, 31, 1);
    TEST_FUNC(BitScanForward64(&idx, UL(0x80000001)), idx, 0, 1);
    TEST_FUNC(BitScanForward64(&idx, ULL(0x8000000000000000)), idx, 63, 1);
    TEST_INT(BitScanReverse64(&idx, UL(0)), 0);
    TEST_FUNC(BitScanReverse64(&idx, UL(1)), idx, 0, 1);
    TEST_FUNC(BitScanReverse64(&idx, UL(0x80000000)), idx, 31, 1);
    TEST_FUNC(BitScanReverse64(&idx, UL(0x80000001)), idx, 31, 1);
    TEST_FUNC(BitScanReverse64(&idx, ULL(0x8000000000000000)), idx, 63, 1);
#endif

    // Test intrinsics versions. Not all combinations are available.
    TEST_FUNC(_interlockedbittestandset(&value, 0), value, 1, 0);
    TEST_FUNC(_interlockedbittestandset(&value, 2), value, 5, 0);
    TEST_FUNC(_interlockedbittestandset(&value, 2), value, 5, 1);
    TEST_FUNC(_interlockedbittestandreset(&value, 2), value, 1, 1);
    TEST_FUNC(_interlockedbittestandreset(&value, 2), value, 1, 0);
    TEST_FUNC(_interlockedbittestandreset(&value, 0), value, 0, 1);
    TEST_FUNC(_InterlockedIncrement(&value), value, 1, 1);
    TEST_FUNC(_InterlockedDecrement(&value), value, 0, 0);
#ifdef _WIN64
    TEST_FUNC(_interlockedbittestandset64(&value64, 0), value64, 1, 0);
    TEST_FUNC(_interlockedbittestandset64(&value64, 2), value64, 5, 0);
    TEST_FUNC(_interlockedbittestandset64(&value64, 2), value64, 5, 1);
    TEST_FUNC(_interlockedbittestandset64(&value64, 40), value64, 0x10000000005, 0);
    TEST_FUNC(_interlockedbittestandset64(&value64, 41), value64, 0x30000000005, 0);
    TEST_FUNC(_interlockedbittestandreset64(&value64, 40), value64, 0x20000000005, 1);
    TEST_FUNC(_interlockedbittestandreset64(&value64, 2), value64, 0x20000000001, 1);
    TEST_FUNC(_interlockedbittestandreset64(&value64, 2), value64, 0x20000000001, 0);
    TEST_FUNC(_interlockedbittestandreset64(&value64, 0), value64, 0x20000000000, 1);
    TEST_FUNC(_InterlockedIncrement64(&value64), value64, 0x20000000001, 0x20000000001);
    TEST_FUNC(_InterlockedDecrement64(&value64), value64, 0x20000000000, 0x20000000000);
    TEST_FUNC(_interlockedbittestandreset64(&value64, 41), value64, 0, 1);
#endif
    TEST_FUNC(_InterlockedExchangeAdd(&value, 1), value, 1, 0);
    TEST_FUNC(_InterlockedExchange(&value, 2), value, 2, 1);
    TEST_FUNC(_InterlockedCompareExchange(&value, 7, 1), value, 2, 2);
    TEST_FUNC(_InterlockedCompareExchange(&value, 0, 2), value, 0, 2);
    TEST_FUNC_PTR(_InterlockedExchangePointer(&ptr, ptr1), ptr, ptr1, NULL);
    TEST_FUNC_PTR(_InterlockedExchangePointer(&ptr, ptr2), ptr, ptr2, ptr1);
    TEST_FUNC_PTR(_InterlockedCompareExchangePointer(&ptr, NULL, ptr1), ptr, ptr2, ptr2);
    TEST_FUNC_PTR(_InterlockedCompareExchangePointer(&ptr, NULL, ptr2), ptr, NULL, ptr2);
#ifdef _WIN64
    TEST_FUNC(_InterlockedExchangeAdd64(&value64, 0x20000000000), value64, 0x20000000000, 0);
    TEST_FUNC(_InterlockedExchange64(&value64, 0x10000000000), value64, 0x10000000000, 0x20000000000);
    TEST_FUNC(_InterlockedCompareExchange64(&value64, 7, 1), value64, 0x10000000000, 0x10000000000);
    TEST_FUNC(_InterlockedCompareExchange64(&value64, 0x20000000000, 0x10000000000), value64, 0x20000000000, 0x10000000000);
#endif
    TEST_FUNC(_InterlockedOr(&value, 2), value, 2, 0);
    TEST_FUNC(_InterlockedOr(&value, 5), value, 7, 2);
    TEST_FUNC(_InterlockedAnd(&value, 2), value, 2, 7);
    TEST_FUNC(_InterlockedAnd(&value, 2), value, 2, 2);
    TEST_FUNC(_InterlockedXor(&value, 2), value, 0, 2);
    TEST_FUNC(_InterlockedXor(&value, 2), value, 2, 0);
    TEST_FUNC(_InterlockedXor(&value, 2), value, 0, 2);
#ifdef _WIN64
    TEST_FUNC(_InterlockedOr64(&value64, 0x10000000000), value64, 0x30000000000, 0x20000000000);
    TEST_FUNC(_InterlockedAnd64(&value64, 0x10000000000), value64, 0x10000000000, 0x30000000000);
    TEST_FUNC(_InterlockedXor64(&value64, 0x10000000000), value64, 0, 0x10000000000);
    TEST_FUNC(_InterlockedXor64(&value64, 0x10000000000), value64, 0x10000000000, 0);
    TEST_FUNC(_InterlockedXor64(&value64, 0x10000000000), value64, 0, 0x10000000000);
#endif

    TEST_INT(_BitScanForward(&idx, UL(0)), 0);
    TEST_FUNC(_BitScanForward(&idx, UL(1)), idx, 0, 1);
    TEST_FUNC(_BitScanForward(&idx, UL(0x80000000)), idx, 31, 1);
    TEST_FUNC(_BitScanForward(&idx, UL(0x80000001)), idx, 0, 1);
    TEST_INT(_BitScanReverse(&idx, UL(0)), 0);
    TEST_FUNC(_BitScanReverse(&idx, UL(1)), idx, 0, 1);
    TEST_FUNC(_BitScanReverse(&idx, UL(0x80000000)), idx, 31, 1);
    TEST_FUNC(_BitScanReverse(&idx, UL(0x80000001)), idx, 31, 1);
#ifdef _WIN64
    TEST_INT(_BitScanForward64(&idx, UL(0)), 0);
    TEST_FUNC(_BitScanForward64(&idx, UL(1)), idx, 0, 1);
    TEST_FUNC(_BitScanForward64(&idx, UL(0x80000000)), idx, 31, 1);
    TEST_FUNC(_BitScanForward64(&idx, UL(0x80000001)), idx, 0, 1);
    TEST_FUNC(_BitScanForward64(&idx, ULL(0x8000000000000000)), idx, 63, 1);
    TEST_INT(_BitScanReverse64(&idx, UL(0)), 0);
    TEST_FUNC(_BitScanReverse64(&idx, UL(1)), idx, 0, 1);
    TEST_FUNC(_BitScanReverse64(&idx, UL(0x80000000)), idx, 31, 1);
    TEST_FUNC(_BitScanReverse64(&idx, UL(0x80000001)), idx, 31, 1);
    TEST_FUNC(_BitScanReverse64(&idx, ULL(0x8000000000000000)), idx, 63, 1);
#endif
#endif

    printf("%d tests, %d failures\n", tests, fails);
    return fails > 0;
}
