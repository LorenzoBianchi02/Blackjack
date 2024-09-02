#ifndef STATS_H
#define STATS_H

//FIXME: !!!!
#include "common.h"

typedef struct Stats{
    long long int profit;
    long long int total_blackjacks;
    long long int total_player_win;
    long long int total_dealer_win;
    long long int num_hands;
    long long int total_draws;

    long long int cards_drawn;

    long long int got[MAX_STATE1][MAX_STATE2][MAX_STATE3];
    long long int gained[MAX_STATE1][MAX_STATE2][MAX_STATE3];
}Stats;

void update_stats(double outcome, Stats *stats, State state_log[22]);
void reset_stats(Stats *stats);
void print_stats(Stats *stats);

#endif