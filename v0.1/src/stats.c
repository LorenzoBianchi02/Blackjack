#include <stdio.h>

#include "stats.h"

void color(double val);

void update_stats(double outcome, Stats *stats, State state_log[22]){
    if(outcome == 1)
        stats->total_player_win++;
    else if(outcome == -1)
        stats->total_dealer_win++;
    else if(outcome  == 1.5){
        stats->total_blackjacks++;
        stats->total_player_win++;
    }
    else if(outcome == 0)
        stats->total_draws++;

    State state;
    //last + 1 state has player val at -1
    for(int i=0; state_log[i].player >= 0; i++){
        state = state_log[i];
        //TODO: rounding?
        stats->got[state.player][state.dealer][state.ace]++;
        stats->gained[state.player][state.dealer][state.ace]+=outcome;
        stats->cards_drawn++;
    }

}

void reset_stats(Stats *stats){
    stats->profit = 0;
    stats->total_blackjacks = 0;
    stats->total_dealer_win = 0;
    stats->total_draws = 0;
    stats->total_player_win = 0;
    stats->num_hands = 0;

    stats->cards_drawn = 0;

    for(int i=0; i<MAX_STATE1; i++){
        for(int j=0; j<MAX_STATE2; j++){
            for(int h=0; h<MAX_STATE3; h++){
                stats->got[i][j][h] = 0;
                stats->gained[i][j][h] = 0;
            }
        }
    }
}

void print_stats(Stats *stats){
    double tmp, tmp2=0;

    printf("\"profit\": %lld\taverage_profit: %f\n", stats->profit, (double)stats->profit/stats->num_hands);
    printf("player_win: %lld\tdealer_win: %lld\tdraws: %lld\tblackjacks: %lld\n", stats->total_player_win, stats->total_dealer_win, stats->total_draws, stats->total_blackjacks);

    printf("HARD VALUES:\n");
    printf("\t");
    for(int i=2; i<MAX_STATE2; i++){
        printf("   %2d    ", i);
    }
    printf("    TOTAL ");
    printf("\n");

    for(int i=MAX_STATE1-1; i>3; i--){
        printf(" %2d    ", i);
        tmp = 0;
        for(int j=2; j<12; j++){
            color((double)stats->gained[i][j][0]/(double)stats->got[i][j][0]);
            printf(" %+.3f  ", (double)stats->gained[i][j][0]/(double)stats->got[i][j][0]);
            tmp += (double)stats->gained[i][j][0]/(double)stats->got[i][j][0];
        }
        color(tmp);
        printf("    %+.3f  ", tmp);
        color(0);
        printf("\n");

        tmp2 += tmp;
    }
    color(tmp2);
    printf("\t\t\t\t\t\t\t\t\t\t\t\t    %+2.3f\n", tmp2);
    color(0);

    printf("\n");

    tmp2=0;

    printf("SOFT VALUES:\n");
    printf("\t");
    for(int i=2; i<MAX_STATE2; i++){
        printf("   %2d    ", i);
    }
    printf("    TOTAL ");
    printf("\n");

    for(int i=MAX_STATE1-1; i>12; i--){
        printf(" %2d    ", i);
        tmp = 0;
        for(int j=2; j<12; j++){
            color((double)stats->gained[i][j][0]/(double)stats->got[i][j][0]);
            printf(" %+.3f  ", (double)stats->gained[i][j][1]/(double)stats->got[i][j][1]);
            tmp += (double)stats->gained[i][j][1]/(double)stats->got[i][j][1];
        }
        color(tmp);
        printf("    %+.3f  ", tmp);
        color(0);
        printf("\n");

        tmp2 += tmp;
    }

    color(tmp2);
    printf("\t\t\t\t\t\t\t\t\t\t\t\t     %+2.3f\n", tmp2);
    color(0);

    printf("\n");
}

void color(double val){
    if(val > 0)
        printf("\033[32m");
    else if(val < 0)
        printf("\033[31m");
    else
        printf("\033[39m");
}