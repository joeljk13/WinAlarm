#ifndef MAIN_H_
#define MAIN_H_ 1

#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <stddef.h>

#define IMPLIES(a,b) (!(a) || (b))

#define ASSERT(b,m) assert(((m), (b)))

#ifdef NDEBUG

#define ASSUME_UNREACHABLE() __assume(0)

#define ASSUME(b) __assume(b)

#else

// Don't actually tell the compiler to assume anything so that the assertions
// aren't optimized out by accident

#define ASSUME_UNREACHABLE() do { \
	ASSERT(0, "assertion failed in ASSUME_UNREACHABLE"); \
} while (0)

#define ASSUME(b) do { \
	ASSERT((b), "assertion failed in ASSUME"); \
} while (0)

#endif

#define TODO_(m) ASSERT(0, #m)

#define TODO(m) TODO_(TODO - m)

#endif