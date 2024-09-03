#ifndef BLACKJACK_H
#define BLACKJACK_H

typedef enum Action {STAND, HIT, DOUBLE, SPLIT, INSURANCE} Action;

typedef struct Hand{
    float reward;
    int done;

    int val_init;
    int val;
    int dealer_init;
    int dealer[100]; //TODO: find more appropriate max value
}Hand;

void init(int seed, int num_decks);
Hand reset(int bet);
Hand step(Action action);

void printShoe(); //FIXME: to remove from header

#endif