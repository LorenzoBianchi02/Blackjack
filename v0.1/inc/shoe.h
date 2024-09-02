#ifndef SHOE_H
#define SHOE_H

extern int deck[13];

typedef struct Shoe{
    int size;   // number of decks
    int cut;    // position of cut off card
    int drawn;  // cards drawn

    int cards[];
}Shoe;

typedef struct Hand{
    int val;
    int ace;
    int num_hands;
}Hand;

Shoe *newShoe(int num_decks);
void printShoe(Shoe *shoe);
void shuffleShoe(Shoe *shoe);
void draw(Shoe *shoe, Hand *hand);

#endif