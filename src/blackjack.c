#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "blackjack.h"

typedef struct Shoe{
    int size;   // number of decks
    int cut;    // position of cut off card
    int drawn;  // cards drawn

    int cards[];
}Shoe;

void shuffleShoe();
int draw();

int deck[] = {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

int dealer2; // dealer_init hidden card
int count_p;   // player value count
int count_d;    // dealer count
int usable_ace_p;     // wether player has a usable ace
int usable_ace_d;     // wether dealer_init has a usable ace
int player_bet;

Hand ep;
Shoe env;

void init(int seed, int n){
    if(!seed){
        seed = time(NULL);
        srand(seed);
    }else
        srand(seed);

    printf("seed: %d\n\n", seed);

    n *= 4;
    env.size = n * 13;

    //memcpy could be better
    for(int i=0; i<n; i++){
        for(int j=0; j<sizeof(deck)/sizeof(int); j++){
            env.cards[i*13+j] = deck[j];
        }
    }

    shuffleShoe();
}

Hand reset(int bet){
    if(env.drawn >= env.cut)
        shuffleShoe();

    player_bet = bet;
    usable_ace_d = 0;
    usable_ace_p = 0;

    ep.reward = 0;
    ep.done = 0;

    ep.val_init = draw();
    ep.dealer_init = draw();
    ep.val = draw();
    dealer2 = draw();
    ep.dealer[0] = 0;

    count_p = ep.val_init + ep.val;
    count_d = ep.dealer_init + dealer2;

    // check for usable player ace
    if(ep.val_init == 11)
        usable_ace_p++;
    if(ep.val == 11)
        usable_ace_p++;

    if(usable_ace_p == 2){
        usable_ace_p--;
        count_p-=10;
    }

    // check for usable dealer ace
    if(ep.dealer_init == 11)
        usable_ace_d++;
    if(dealer2 == 11)
        usable_ace_d++;

    if(usable_ace_d == 2){
        usable_ace_d--;
        count_d -= 10;
    }

    // check for dealer_init blackjack
    if(count_d == 21){
        //printf("BJ dealer\n");
        ep.done = 1;
        if(count_p != 21){
            ep.reward = -bet;
        }
    }

    // check for player blackjack
    else if(count_p == 21){
        //printf("BJ player\n");
        ep.reward = bet * 1.5;
        ep.done = 1;
    }

    return ep;
}

void printShoe(){
    printf("\nCards remaining: %d\nCards till cut: %d\n", env.size - env.drawn, env.cut - env.drawn);
    for(int i=env.drawn; i<env.size; i++){
        if(i == env.cut)
            printf("CUT(%d) ", i);
        printf("%d ", env.cards[i]);
    }
    printf("\n\n");
}

void shuffleShoe(){
    int tmp, val;
    for(int i=env.size; i>0; i--){
        tmp = rand() % i;
        val = env.cards[tmp];
        env.cards[tmp] = env.cards[i-1];
        env.cards[i-1] = val;
    }

    env.cut = (rand() % (env.size / 10)) + (env.size * 7 / 10);
    env.drawn = 0;
}


//FIXME: using functions might be a bit smarter
Hand step(Action action){
    switch (action){
        case HIT:
            //player hits
            ep.val = draw();
            count_p += ep.val;
            if(ep.val == 11)
                usable_ace_p++;

            //if player hits to 21 he has to stand
            if(count_p == 21)
                goto stand; //MUAHAHAHA, your mortal oppinion does not matter
            break;

        case STAND:
            ep.val = 0; //if player got to stand without standing, they should still know what card they got
            stand:

            int d_cards = 2;
            ep.done = 1;
            ep.dealer[0] = dealer2;
            ep.dealer[1] = 0;

            //dealer hits, cards got go to ep.dealer
            while(count_d < 17){    //TODO: 
                dealer2 = draw();
                ep.dealer[d_cards-1] = dealer2;
                count_d += dealer2;
                d_cards++;

                if(dealer2 == 11)
                    usable_ace_d++;
                if(count_d > 21 && usable_ace_d){
                    count_d-=10;
                    usable_ace_d--;
                }
            }

            ep.dealer[d_cards] = 0;

            //check who won
            if(count_d > 21)
                ep.reward = player_bet;
            else if(count_d > count_p)
                ep.reward = -player_bet;
            else if(count_d < count_p)
                ep.reward = player_bet;

            break;

        case DOUBLE:
            //doubles the bet
            player_bet *= 2;

            //hits
            ep.val = draw();
            count_p += ep.val;
            if(ep.val == 11)
                usable_ace_p++;

            //player has to stand
            goto stand;

            break;


        //TODO: add actions
    }

    //check if end of episode
    if(count_p > 21){
        if(usable_ace_p){
            count_p -= 10;
            usable_ace_p--;
        }else{
            ep.reward = -player_bet;
            ep.done = 1;
            ep.dealer[0] = dealer2;
            ep.dealer[1] = 0;
        }
    }

    return ep;
}


int draw(){
    return env.cards[env.drawn++];
}