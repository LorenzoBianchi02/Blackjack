#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "shoe.h"

int deck[] = {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

Shoe *newShoe(int n){
    n *= 4;
    Shoe *shoe = (Shoe *)malloc(sizeof(Shoe) + sizeof(int[n * 13]));
    shoe->size = n * 13;
    shoe->cut = 0;
    
    for(int i=0; i<n; i++)
        memcpy(shoe->cards + i*13, deck, sizeof(deck));

    return shoe;
}

void printShoe(Shoe *shoe){
    for(int i=0; i<shoe->size; i++){
        printf("%d ", shoe->cards[i]);
    }
    printf("\n");
}

void shuffleShoe(Shoe *shoe){
    int tmp, val;
    for(int i=shoe->size; i>0; i--){
        tmp = rand() % i;
        val = shoe->cards[tmp];
        shoe->cards[tmp] = shoe->cards[i-1];
        shoe->cards[i-1] = val;
    }

    shoe->cut = (rand() % (shoe->size/2)) + shoe->size/4;
    shoe->drawn = 0;

    // printf("new shoe: size: %d\tcut: %d\n", shoe->size, shoe-> cut);
}

void draw(Shoe *shoe, Hand *hand){
    //draws card
    int card = shoe->cards[shoe->drawn];
    shoe->drawn++;
    hand->num_hands++;

    //adds value
    hand->val += card;
    //checks if ace
    if(card == 11)
        hand->ace = 1;
    //if busts but has ace
    if(hand->val > 21 && hand->ace){
        hand->val -= 10;
        hand->ace = 0;
    }
}