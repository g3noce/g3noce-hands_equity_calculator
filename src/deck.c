#include "deck.h"
#include "lookup_tables.h"
#include <stdlib.h>
#include <string.h>

bool initialize_deck(Card** deck) {
    if (!deck) return false;
    
    *deck = (Card*)malloc(DECK_SIZE * sizeof(Card));
    if (!*deck) return false;
    
    int index = 0;
    for (int i = 0; i < NUM_RANKS; i++) {
        for (int j = 0; j < NUM_SUITS; j++) {
            (*deck)[index].value = RANKS[i] | SUITS[j] | PRIMES[i];
            index++;
        }
    }
    return true;
}

void shuffle_deck(Card* deck) {
    for (int i = DECK_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}