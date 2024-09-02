#ifndef COMMON_H
#define COMMON_H

#define MAX_STATE1 21   // player values
#define MAX_STATE2 12   // dealer values
#define MAX_STATE3 2    // usable ace in hand
#define MAX_STATE4 3    // player actions

typedef struct State{
    int player;
    int dealer;
    int ace;
    int action;
}State;

#endif