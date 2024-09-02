#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "shoe.h"
#include "stats.h"
#include "common.h"

#define TMP_FILE "tmp.txt"

#define MAX_STATE_LOG 64

typedef double **** qtable_t;

typedef enum Action {STAND, HIT, SPLIT} Action;

double rand_d(){
    return (double)rand() / (double)RAND_MAX ;
}

//FIXME: not used everywhere
//returns the index of the highest double in arr
int max(double *arr, int size){
    double tmp_val = arr[0];
    int tmp_i = 0;
    for(int i=1; i<size; i++){
        if(arr[i] > tmp_val){
            tmp_val = arr[i];
            tmp_i = i;
        }
    }

    return tmp_i;
}

void train(qtable_t Qtable, Shoe *shoe, long long int num_hands, Stats *stats);
double playHand(Shoe *shoe, qtable_t Qtable, Stats *stats);
int player_action(State *state, qtable_t Qtable);
int dealer_action(Hand *hand);

double winner(Hand *player, Hand *dealer);
void reward(qtable_t Qtable, State state_log[MAX_STATE_LOG], double outcome, Stats *stats);

void print_table(qtable_t Qtable);    


double lr = 0.0025; //learning rate
double er = 0.06; //explore new action rate

int main(int argc, char *argv[]){
    srand(time(0));

    FILE *wr = NULL, *rd = NULL;
    char tmp_file[30];
    long long int num_hands_sim = 1000000, num_hands_train = 0;
    Stats stats;
    reset_stats(&stats);

    //INPUT//
    //help
    if(argc == 1 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")){
        printf(" HELP:\n");
        printf("\tUsage: ./blackjack NUM_HANDS [OPTION]\n\n");
        printf("\twith no NUM_HANDS, NUM_HANDS = %lld\n\n", num_hands_sim);
        
        printf("\t-tr + n\ttrain for n hands before simulating\n");
        printf("\t-rd + \"rd.txt\"\tstart training from rd.txt qtable, default: random\n");
        printf("\t-wr + \"wr.txt\"\twrite qtable to wr.txt\n");

        exit(0);
    }
    //num hands
    num_hands_sim = atoi(argv[1]);

    
    //options
    for(int i=2; i<argc; i+=2){
        if(!strcmp(argv[i], "-tr")){
            num_hands_train = atoi(argv[i+1]);
        }
        if(!strcmp(argv[i], "-rd")){
            rd = fopen(argv[i+1], "r");
            if(!rd){
                printf("rd file not found\n");
                exit(EXIT_FAILURE);
            }
        }
        if(!strcmp(argv[i], "-wr")){
            wr = fopen(TMP_FILE, "w");
            strcpy(tmp_file, argv[i+1]);
            if(!wr){
                printf("wr file not found\n");
                exit(EXIT_FAILURE);
            }
        }
    }


    //QTALBE
    qtable_t Qtable = (qtable_t)malloc(MAX_STATE1 * sizeof(double ***));
    for(int i=0; i<MAX_STATE1; i++){
        Qtable[i] = (double ***)malloc(MAX_STATE2 * sizeof(double **));
        for(int j=0; j<MAX_STATE2; j++){
            Qtable[i][j] = (double **)malloc(MAX_STATE3 * sizeof(double *));
            for(int h=0; h<MAX_STATE3; h++){
                Qtable[i][j][h] = (double *)malloc(MAX_STATE4 *sizeof(double));
                for(int k=0; k<MAX_STATE4; k++){
                    //random val from -1 to 1   TODO: test bigger numbers
                    Qtable[i][j][h][k] = 2*rand_d()-1;
                    // Qtable[i][j][h][k] = -1;
                }
            }
        }
    }

    Shoe *shoe = newShoe(6);
    shuffleShoe(shoe);

    if(rd){
        for(int i=0; i<MAX_STATE1; i++){
            for(int j=0; j<MAX_STATE2; j++){
                for(int h=0; h<MAX_STATE3; h++){
                    for(int k=0; k<MAX_STATE4; k++){
                        fscanf(rd, "%lf ", &Qtable[i][j][h][k]);
                    }
                }
            }
        }
        
        fclose(rd);
    }

    //training
    printf("TRAINING\n");
    train(Qtable, shoe, num_hands_train, &stats);

    lr = 0;
    er = 0;
    reset_stats(&stats);

    //simulation
    printf("SIMULATING\n");
    train(Qtable, shoe, num_hands_sim, &stats);

    if(wr){
        for(int i=0; i<MAX_STATE1; i++){
            for(int j=0; j<MAX_STATE2; j++){
                for(int h=0; h<MAX_STATE3; h++){
                    for(int k=0; k<MAX_STATE4; k++){
                        fprintf(wr, "%f ", Qtable[i][j][h][k]);
                    }
                }
            }
        }

        fprintf(wr, "\n%f\n", (double)stats.profit/num_hands_sim);
        fclose(wr);

        rename(TMP_FILE, tmp_file);
    }

    print_table(Qtable);
    print_stats(&stats);

    if(wr)


    return 0;
}

void train(qtable_t Qtable, Shoe *shoe, long long int num_hands, Stats *stats){
    printf("----------\n");

    for(long long int i=0; i<num_hands; i++){
        if(!(i%(num_hands/10))){
            printf("+");
            fflush(stdout);
        }

        if(shoe->drawn>=shoe->cut)
            shuffleShoe(shoe);
                
        playHand(shoe, Qtable, stats);
        stats->num_hands++; 
    }
    printf("\n\n");
}

//FIXME: divide in functions
double playHand(Shoe *shoe, qtable_t Qtable, Stats *stats){
    State state_log[MAX_STATE_LOG]; //log of visited states
    State state;

    Hand player, dealer;
    dealer.val = 0;
    player.val = 0;
    player.num_hands = 0;
    dealer.num_hands = 0;
    player.ace = 0;
    dealer.ace = 0;

    draw(shoe, &player);
    draw(shoe, &dealer);

    int num_splits = 0;
    int init_player_val = player.val;

    Action action = HIT;
    int num_actions = 0;

    //player's turn
    do{
        player.val = init_player_val;
        while(action != STAND){
            draw(shoe, &player);
            if(player.val >= 21){
                break;
            }

            state = (State){.player = player.val, .dealer = dealer.val, .ace = player.ace};

            num_actions++;

            action = player_action(&state, Qtable);
            state.action = action;
            state_log[num_actions-1] = state;

            if(action == SPLIT){
                player.val /= 2;
                num_splits++;
            }
        }
    }while(num_splits--);

    state_log[num_actions].player = -1;

    //dealer's turn
    do{
        draw(shoe, &dealer);
    }while(dealer.val < 21 && dealer_action(&dealer) == HIT);


    //TODO: FIXME: ogni state deve avere un valore che indica il numero di split a cui appartiene per dare gli outcome corrette di ogni split
    double outcome = winner(&player, &dealer);

    update_stats(outcome, stats, state_log);
    reward(Qtable, state_log, outcome, stats);

    stats->profit += outcome;
}

int player_action(State *state, qtable_t Qtable){
    int action;

    //take current best action
    double best_q, current_q;
    if(rand_d() > er){
        //searches best move
        for(int i=0; i<MAX_STATE4; i++){
            current_q = Qtable[state->player][state->dealer][state->ace][i];
            if(i == 0 ||  current_q > best_q){
                best_q = current_q;
                action = i;
            }
        }
    }
    //take a random action
    else{
        action = rand()%2;
    }

    return action;
}

int dealer_action(Hand *hand){
    if(hand->val < 17)
        return HIT;

    if(hand->val == 17 && hand->ace)    //H17, comment for S17
        return HIT;

    return STAND;
}


double winner(Hand *player, Hand *dealer){
    //player has blackjack
    if(player->val == 21 && player->num_hands == 2){
        if(dealer->val == 21 && dealer->num_hands == 2)
            return 0;
        else
            return 1.5;
    }

    //dealer has blackjack
    if(dealer->val == 21 && dealer->num_hands == 2)
        return -1;

    //player busts
    if(player->val > 21)
        return -1;

    //dealer busts
    if(dealer->val > 21)
        return 1;
    
    //push
    if(player->val==dealer->val)
        return 0;

    // rest
    return player->val > dealer->val ? 1 : -1;
}

void reward(qtable_t Qtable, State state_log[MAX_STATE_LOG], double outcome, Stats *stats){
    State state;
    //last + 1 state has player val at -1
    for(int i=0; state_log[i].player >= 0; i++){
        state = state_log[i];
        //FIXME: state_log always has same dealer value (duplicates)
        //TODO: rounding?
        Qtable[state.player][state.dealer][state.ace][state.action] += lr*(outcome - Qtable[state.player][state.dealer][state.ace][state.action]);
    }
}

void print_table(qtable_t Qtable){
    printf("HARD VALUES:\n");
    printf("\t");
    for(int i=2; i<MAX_STATE2; i++){
        printf("  %2d   ", i);
    }
    printf("\n");

    for(int i=MAX_STATE1-1; i>3; i--){
        printf(" %2d    ", i);
        for(int j=2; j<12; j++){
            switch(max(Qtable[i][j][0], MAX_STATE4-1)){
                case 0:
                    printf(" STAND ");
                    break;
                case 1:
                    printf("  HIT  ");
                    break;
                case 2:
                    printf(" SPLIT ");
                    break;
            }
        }
        printf("\n");
    }
    printf("\n");

    printf("SOFT VALUES:\n");
    printf("\t");
    for(int i=2; i<MAX_STATE2; i++){
        printf("  %2d   ", i);
    }
    printf("\n");

    for(int i=MAX_STATE1-1; i>12; i--){
        printf(" %2d    ", i);
        for(int j=2; j<12; j++){
            switch(max(Qtable[i][j][1], MAX_STATE4-1)){
                case 0:
                    printf(" STAND ");
                    break;
                case 1:
                    printf("  HIT  ");
                    break;
                case 2:
                    printf(" SPLIT ");
                    break;
            }
        }
        printf("\n");
    }

    printf("\n");
}