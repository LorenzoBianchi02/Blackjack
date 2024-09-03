#include <stdio.h>

#include "blackjack.h"

Action strat_stand17(int count);
Action strat_simple(int count, int dealer);
Action strat_basicstrategy(int count, int dealer, int ace, int num_cards);

Action basic_hard[8][10] = {
    {HIT, HIT, HIT, DOUBLE, DOUBLE, HIT, HIT, HIT, HIT, HIT}, //A,2
    {HIT, HIT, HIT, DOUBLE, DOUBLE, HIT, HIT, HIT, HIT, HIT}, //A,3
    {HIT, HIT, DOUBLE, DOUBLE, DOUBLE, HIT, HIT, HIT, HIT, HIT}, //A,4
    {HIT, HIT, DOUBLE, DOUBLE, DOUBLE, HIT, HIT, HIT, HIT, HIT}, //A,5
    {HIT, DOUBLE, DOUBLE, DOUBLE, DOUBLE, HIT, HIT, HIT, HIT, HIT}, //A,6
    {DOUBLE, DOUBLE, DOUBLE, DOUBLE, DOUBLE, STAND, STAND, HIT, HIT, HIT}, //A,7
    {STAND, STAND, STAND, STAND, DOUBLE, STAND, STAND, STAND, STAND, STAND}, //A,8
    {STAND, STAND, STAND, STAND, STAND, STAND, STAND, STAND, STAND, STAND} //A,9
    
};

Action basic_soft[10][10] = {
    {HIT, HIT, HIT, HIT, HIT, HIT, HIT, HIT, HIT, HIT}, //8
    {HIT, DOUBLE, DOUBLE, DOUBLE, DOUBLE, HIT, HIT, HIT, HIT, HIT}, //9
    {DOUBLE, DOUBLE, DOUBLE, DOUBLE, DOUBLE, DOUBLE, DOUBLE, DOUBLE, HIT, HIT}, //10
    {DOUBLE, DOUBLE, DOUBLE, DOUBLE, DOUBLE, DOUBLE, DOUBLE, DOUBLE, DOUBLE, DOUBLE}, //11
    {HIT, HIT, STAND, STAND, STAND, HIT, HIT, HIT, HIT, HIT}, //12
    {STAND, STAND, STAND, STAND, STAND, HIT, HIT, HIT, HIT, HIT}, //13
    {STAND, STAND, STAND, STAND, STAND, HIT, HIT, HIT, HIT, HIT}, //14
    {STAND, STAND, STAND, STAND, STAND, HIT, HIT, HIT, HIT, HIT}, //15
    {STAND, STAND, STAND, STAND, STAND, HIT, HIT, HIT, HIT, HIT}, //16
    {STAND, STAND, STAND, STAND, STAND, STAND, STAND, STAND, STAND, STAND} //17
    
};


int main(){
    int info_move[8];
    info_move[HIT] = 0;
    info_move[STAND] = 0;
    info_move[DOUBLE] = 0;

    init(0, 1);

    int num_episodes = 10000000;
    int tot_reward = 0;
    int win=0, loss=0, draw=0, blackjack=0, doubles = 0;

    Hand hand;
    int count=0, usable_ace=0, num_cards=0;

    for(int i=0; i<num_episodes; i++){
        // printf("START\n");
        count = 0;
        usable_ace = 0;
        num_cards = 2;

        hand = reset(1);
        count += hand.val_init + hand.val;

        // check for usable ace
        if(hand.val_init == 11)
            usable_ace++;
        if(hand.val == 11)
            usable_ace++;

        if(usable_ace == 2){
            usable_ace--;
            count-=10;
        }

        while(!hand.done){
            // Action move = strat_stand17(count);
            // Action move = strat_simple(count, hand.dealer_init);
            Action move = strat_basicstrategy(count, hand.dealer_init, usable_ace, num_cards);
            // printf("%d (%d %d %d)\n", move, count, hand.dealer_init, usable_ace);
            
            info_move[move]++;
            if(move != STAND){  //FIXME: might not be correct when adding other moves
                num_cards++;
            }

            hand = step(move);
            count += hand.val;

            if(hand.val == 11)
                usable_ace++;

            if(count > 21 && usable_ace){
                count-=10;
                usable_ace--;
            }
        }

        //check how they won
        tot_reward += hand.reward;
        if(hand.reward == 1)
            win++;
        else if(hand.reward == 1.5)
            blackjack += 1.5;
        else if(hand.reward == 2)
            doubles += 2;
        else if(hand.reward < 0)
            loss--;
        else
            draw++;



        if(i%100000 == 0)
            printf("%d\n", i);

        int cont=0, cont2=hand.dealer_init;
        while(hand.dealer[cont]){
            cont2+=hand.dealer[cont];
            cont++;
        }

        // printf("%d vs %d (%.1f)\n", count, cont2, hand.reward);
    }

    
    printf("tot: %d\nperc: %f%%\n", tot_reward, (double)tot_reward/num_episodes);
    // printf("won: %d (bj: %d), loss: %d, draw: %d     (%d)\n", win, blackjack, loss, draw, win+loss+draw);
    printf("won: %f%%, bj: %f%%, doubles: %f%%, loss: %f%%, draw: %d\n", (double)win/num_episodes, (double)blackjack/num_episodes, (double)doubles/num_episodes, (double)loss/num_episodes, draw);
    printf("hits: %d, stands: %d, doubles: %d\n", info_move[HIT], info_move[STAND], info_move[DOUBLE]);

    return 0;
}

Action strat_stand17(int count){
    if(count < 17){
        return HIT;
    }
    return STAND;
}

Action strat_simple(int count, int dealer){
    if(count >= 17){
        return STAND;
    }else if(count > 11 && dealer < 7){
        return STAND;
    }
    return HIT;
}

Action strat_basicstrategy(int count, int dealer, int ace, int num_cards){
    Action move;
    if(ace){
        move = basic_hard[count - 13][dealer - 2];
        if(move == DOUBLE && num_cards > 2){
            if(count >= 18)
                move = STAND;
            else
                move = HIT;
        }
    }else{
        if(count < 8)
            return HIT;
        if(count >= 17)
            return STAND;

        move = basic_soft[count - 8][dealer - 2];

        if(move == DOUBLE && num_cards > 2){
            move = HIT;
        }
    }

        return move;
}