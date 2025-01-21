#ifndef DECK_H
#define DECK_H

#include <stdint.h>
#include <stdbool.h>

#define DECK_SIZE 52
#define COMMUNITY_CARDS 5
#define PLAYER_HAND_SIZE 2

typedef enum {
    LOSE,
    TIE,
    WIN
} Result;

typedef struct {
    uint32_t value;
} Card;

bool initialize_deck(Card** deck);
void shuffle_deck(Card* deck);

#endif // DECK_H