#include <stdio.h>

#include "blackjack.h"

Action strat_stand17(int count);
Action strat_simple(int count, int dealer);

int main(){

    init(0, 1);

    int num_episodes = 1000000;
    int tot_reward = 0;
    int win=0, loss=0, draw=0, blackjack=0;

    Hand hand;
    int count = 0, usable_ace = 0;

    for(int i=0; i<num_episodes; i++){
        count = 0;
        usable_ace = 0;

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

        //printf("START: %d vs %d\n", count, hand.dealer_init);

        while(!hand.done){
            hand = step(strat_stand17(count));
            count += hand.val;
            if(hand.val == 11)
                usable_ace++;

            if(count > 21 && usable_ace){
                count-=10;
                usable_ace--;
            }
        }

        tot_reward += hand.reward;
        if(hand.reward > 0){
            if(hand.reward == 1.5)
                blackjack++;
            win++;
        }else if(hand.reward < 0)
            loss++;
        else
            draw++;



        if(i%100000 == 0)
            printf("%d\n", i);

        //printf("%.1f: %d vs %d ", hand.reward, count, hand.dealer_init);
        int cont=0, cont2=hand.dealer_init;
        while(hand.dealer[cont]){
            //printf("%d ", hand.dealer[cont]);
            cont2+=hand.dealer[cont];
            cont++;
        }
        //printf("(%d)\n\n", cont2);
    }

    
    printf("tot: %d\nperc: %f%%\n", tot_reward, (double)tot_reward/num_episodes);
    printf("won: %d (bj: %d), loss: %d, draw: %d     (%d)\n", win, blackjack, loss, draw, win+loss+draw);
    printf("won: %f%% (bj: %f%%), loss: %f%%, draw: %f%%     (%f%%)\n", (double)win/num_episodes, (double)blackjack/num_episodes, (double)loss/num_episodes, (double)draw/num_episodes, (double)(win+loss+draw)/num_episodes);

    return 0;
}

Action strat_stand17(int count){
    if(count < 17){
        return HIT;
    }
    return STAND;
}

Action strat_simple(int count, int dealer){
    if(count >= 17)
        return STAND;
    else if(count < )
}