// input control


#ifndef __inputCtrl__
#define __inputCtrl__

#include <stdio.h>

enum Events { fireEv, pauseEv, quitEv, delEv, entEv, noEv };
enum States { mvLeftSt, mvRightSt, noSt };

typedef enum {false,true} bool;

typedef enum Events inputEvent;
typedef enum States inputStates;

void initCtrl();
inputEvent getEvent(); // which event happended
bool isState(inputStates); // is a state
void closeCtrl(); // close input control

#endif /* __inputCtrl__ */
