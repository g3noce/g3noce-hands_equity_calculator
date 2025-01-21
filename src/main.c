#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <sys/random.h>
#include "deck.h"
#include "hand_evaluation.h"
#include "lookup_tables.h"

#define NUM_PLAYERS 2
#define NUM_SIMULATIONS 10000000
#define NUM_POSSIBLE_HANDS 169
#define MATRIX_SIZE 13  // Added missing constant

// Define rank symbols for matrix headers
const char RANK_SYMBOLS[13] = {'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2'};

typedef struct {
    char name[6];
    long total_points;
    int occurrences;
    double average_points;
} HandStats;

typedef struct {
    Card hand[PLAYER_HAND_SIZE];
    uint32_t score;
    int position;
    int points;
} Player;

// Function prototypes
static int get_rank_index(uint32_t value);
static int get_suit_index(uint32_t value);
static char get_rank_char(int rank_index);
static void get_hand_name(Card card1, Card card2, char* name);
static void initialize_hand_stats(HandStats* stats);
static int compare_players(const void* a, const void* b);
static void deal_cards(const Card* deck, Player* players, Card* community_cards);
static void evaluate_hands(Player* players, const Card* community_cards);
static void assign_points(Player* players);
static void update_stats(HandStats* stats, const Player* players);
static bool save_matrix_to_csv(const HandStats* stats, const char* filename);

// Implementation of utility functions
static int get_rank_index(uint32_t value) {
    uint32_t rank_bits = value >> 16;
    int index = 0;
    while (rank_bits >>= 1) index++;
    return index;
}

static int get_suit_index(uint32_t value) {
    uint32_t suit_bits = (value >> 12) & 0xF;
    int index = 0;
    while (suit_bits >>= 1) index++;
    return index;
}

static char get_rank_char(int rank_index) {
    const char* ranks = "23456789TJQKA";
    return ranks[rank_index];
}

static void get_hand_name(Card card1, Card card2, char* name) {
    if (!name) return;
    
    int rank1 = get_rank_index(card1.value);
    int rank2 = get_rank_index(card2.value);
    int suit1 = get_suit_index(card1.value);
    int suit2 = get_suit_index(card2.value);
    
    // Ensure higher rank is first
    if (rank2 > rank1) {
        int temp_rank = rank1;
        rank1 = rank2;
        rank2 = temp_rank;
    }
    
    char rank1_char = get_rank_char(rank1);
    char rank2_char = get_rank_char(rank2);
    
    if (rank1 == rank2) {
        snprintf(name, 6, "%c%c", rank1_char, rank2_char);
    } else {
        snprintf(name, 6, "%c%c%c", rank1_char, rank2_char, 
                (suit1 == suit2) ? 's' : 'o');
    }
}

static void initialize_hand_stats(HandStats* stats) {
    if (!stats) return;
    
    const char* ranks = "AKQJT98765432";
    int index = 0;
    
    // Pocket pairs
    for (int i = 0; ranks[i]; i++) {
        snprintf(stats[index].name, sizeof(stats[index].name), "%c%c", ranks[i], ranks[i]);
        stats[index].total_points = 0;
        stats[index].occurrences = 0;
        stats[index].average_points = 0.0;
        index++;
    }
    
    // Non-paired hands
    for (int i = 0; ranks[i]; i++) {
        for (int j = i + 1; ranks[j]; j++) {
            // Suited
            snprintf(stats[index].name, sizeof(stats[index].name), "%c%cs", ranks[i], ranks[j]);
            stats[index].total_points = 0;
            stats[index].occurrences = 0;
            stats[index].average_points = 0.0;
            index++;
            
            // Offsuit
            snprintf(stats[index].name, sizeof(stats[index].name), "%c%co", ranks[i], ranks[j]);
            stats[index].total_points = 0;
            stats[index].occurrences = 0;
            stats[index].average_points = 0.0;
            index++;
        }
    }
}

static int compare_players(const void* a, const void* b) {
    const Player* p1 = (const Player*)a;
    const Player* p2 = (const Player*)b;
    return (p1->score > p2->score) - (p1->score < p2->score);
}

static void deal_cards(const Card* deck, Player* players, Card* community_cards) {
    if (!deck || !players || !community_cards) return;
    
    int card_index = 0;
    
    for (int i = 0; i < NUM_PLAYERS; i++) {
        for (int j = 0; j < PLAYER_HAND_SIZE; j++) {
            players[i].hand[j] = deck[card_index++];
        }
    }
    
    for (int i = 0; i < COMMUNITY_CARDS; i++) {
        community_cards[i] = deck[card_index++];
    }
}

static void evaluate_hands(Player* players, const Card* community_cards) {
    if (!players || !community_cards) return;
    
    for (int i = 0; i < NUM_PLAYERS; i++) {
        Card all_cards[TOTAL_HAND_SIZE];
        memcpy(all_cards, players[i].hand, PLAYER_HAND_SIZE * sizeof(Card));
        memcpy(all_cards + PLAYER_HAND_SIZE, community_cards, COMMUNITY_CARDS * sizeof(Card));
        players[i].score = eval7(all_cards);
        players[i].position = i;
    }
}

static void assign_points(Player* players) {
    if (!players) return;
    
    players[0].points = 1;  // Winner gets 1 point
    for (int i = 1; i < NUM_PLAYERS; i++) {
        players[i].points = 0;  // Others get 0 points
    }
}

static void update_stats(HandStats* stats, const Player* players) {
    if (!stats || !players) return;
    
    for (int p = 0; p < NUM_PLAYERS; p++) {
        char hand_name[6];
        get_hand_name(players[p].hand[0], players[p].hand[1], hand_name);
        
        for (int i = 0; i < NUM_POSSIBLE_HANDS; i++) {
            if (strcmp(stats[i].name, hand_name) == 0) {
                stats[i].total_points += players[p].points;
                stats[i].occurrences++;
                break;
            }
        }
    }
}

static bool save_matrix_to_csv(const HandStats* stats, const char* filename) {
    if (!stats || !filename) return false;
    
    FILE* f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Error opening file %s for writing\n", filename);
        return false;
    }

    const char* ranks = "AKQJT98765432";
    HandStats* matrix[MATRIX_SIZE][MATRIX_SIZE] = {0};

    // Create mapping of hands to matrix positions
    for (int i = 0; i < NUM_POSSIBLE_HANDS; i++) {
        if (stats[i].occurrences > 0) {
            char hand[6];
            strncpy(hand, stats[i].name, sizeof(hand) - 1);
            hand[sizeof(hand) - 1] = '\0';
            
            char rank1 = hand[0];
            char rank2 = hand[1];
            char* pos1 = strchr(ranks, rank1);
            char* pos2 = strchr(ranks, rank2);
            
            if (pos1 && pos2) {
                int row = pos1 - ranks;
                int col = pos2 - ranks;
                
                if (row == col) {  // Paired hands
                    matrix[row][col] = (HandStats*)&stats[i];
                } else if (hand[2] == 's') {  // Suited hands
                    matrix[row][col] = (HandStats*)&stats[i];
                } else if (hand[2] == 'o') {  // Offsuit hands
                    matrix[col][row] = (HandStats*)&stats[i];
                }
            }
        }
    }

    // Write CSV content
    fprintf(f, ",");  // Corner cell
    for (int i = 0; i < MATRIX_SIZE; i++) {
        fprintf(f, "%c,", ranks[i]);
    }
    fprintf(f, "\n");

    for (int i = 0; i < MATRIX_SIZE; i++) {
        fprintf(f, "%c,", ranks[i]);  // Row header
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (matrix[i][j]) {
                fprintf(f, "%.3f,", matrix[i][j]->average_points);
            } else {
                fprintf(f, ",");
            }
        }
        fprintf(f, "\n");
    }

    fclose(f);
    return true;
}

int main(void) {
    Card* deck = NULL;
    HandStats* stats = NULL;
    struct timespec start, end;
    double elapsed_time;
    
    // Allocation mémoire
    stats = calloc(NUM_POSSIBLE_HANDS, sizeof(HandStats));
    if (!stats) {
        fprintf(stderr, "Failed to allocate memory for stats\n");
        return EXIT_FAILURE;
    }
    
    if (!initialize_deck(&deck)) {
        fprintf(stderr, "Failed to initialize deck\n");
        free(stats);
        return EXIT_FAILURE;
    }
    
    // Initialisation du générateur de nombres aléatoires
    unsigned int seed;
    if (getrandom(&seed, sizeof(seed), 0) == -1) {
        // Fallback sur time() si getrandom échoue
        seed = (unsigned int)time(NULL);
    }
    srand(seed);
    
    initialize_hand_stats(stats);
    
    Player players[NUM_PLAYERS] = {0};
    Card community_cards[COMMUNITY_CARDS];
    
    printf("Running %d simulations...\n", NUM_SIMULATIONS);
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        fprintf(stderr, "Failed to get start time\n");
        free(deck);
        free(stats);
        return EXIT_FAILURE;
    }
    
    // Boucle principale de simulation
    for (int sim = 0; sim < NUM_SIMULATIONS; sim++) {
        shuffle_deck(deck);
        deal_cards(deck, players, community_cards);
        evaluate_hands(players, community_cards);
        qsort(players, NUM_PLAYERS, sizeof(Player), compare_players);
        assign_points(players);
        update_stats(stats, players);
        
        if (sim % (NUM_SIMULATIONS/10) == 0) {
            printf("Progress: %d%%\n", (sim * 100) / NUM_SIMULATIONS);
            fflush(stdout);
        }
    }
    
    // Calcul des moyennes finales
    for (int i = 0; i < NUM_POSSIBLE_HANDS; i++) {
        if (stats[i].occurrences > 0) {
            stats[i].average_points = (double)stats[i].total_points / stats[i].occurrences;
        }
    }
    
    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        fprintf(stderr, "Failed to get end time\n");
        elapsed_time = 0.0;
    } else {
        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    }
    printf("Elapsed time: %.2f seconds\n", elapsed_time);

    // Sauvegarde des résultats
    const char* filename = "hands_equity.csv";
    if (!save_matrix_to_csv(stats, filename)) {
        fprintf(stderr, "Failed to save results to %s\n", filename);
    } else {
        printf("Simulation complete. Results saved to %s\n", filename);
    }

    // Nettoyage
    free(deck);
    free(stats);
    return EXIT_SUCCESS;
}