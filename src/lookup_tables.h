#ifndef LOOKUP_TABLES_H
#define LOOKUP_TABLES_H

#include <stdint.h>

#define NUM_SUITS 4
#define NUM_RANKS 13
#define FLUSHES_SIZE 8192
#define UNIQUE_5_SIZE 8192
#define HASH_ADJUST_SIZE 512
#define HASH_VALUES_SIZE 8192

extern const uint32_t FLUSHES[FLUSHES_SIZE];
extern const uint32_t UNIQUE_5[UNIQUE_5_SIZE];
extern const uint32_t HASH_ADJUST[HASH_ADJUST_SIZE];
extern const uint32_t HASH_VALUES[HASH_VALUES_SIZE];

extern const uint32_t SUITS[NUM_SUITS];
extern const uint32_t RANKS[NUM_RANKS];
extern const uint32_t PRIMES[NUM_RANKS];

#endif // LOOKUP_TABLES_H