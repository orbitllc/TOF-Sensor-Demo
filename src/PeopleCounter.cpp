// People Counter Class
// Author: Chip McClelland
// Date: May 2023
// License: GPL3
// In this class, we look at the occpancy values and determine what the occupancy count should be 
// Note, this code is limited to a single sensor with two zones
// Note, this code assumes that Zone 1 is the inner (relative to room we are measureing occupancy for) and Zone 2 is outer

#include "Particle.h"
#include "PeopleCounterConfig.h"
#include "ErrorCodes.h"
#include "PeopleCounter.h"
#include "TofSensor.h"
#include <StackArray.h>

StackArray <int> stateStack;
StackArray <int> tempStack;

static int occupancyCount = 0;      // How many folks in the room or (if there is more than one door) - net occupancy through this door
static int occupancyLimit = DEFAULT_PEOPLE_LIMIT;

PeopleCounter *PeopleCounter::_instance;

// [static]
PeopleCounter &PeopleCounter::instance() {
    if (!_instance) {
        _instance = new PeopleCounter();
    }
    return *_instance;
}

PeopleCounter::PeopleCounter() {
}

PeopleCounter::~PeopleCounter() {
}

void PeopleCounter::setup() {
}

void PeopleCounter::loop(){                                             // This function is only called if there is a change in occupancy state
    int oldOccupancyCount = occupancyCount;
    int newOccupancyState = TofSensor::instance().getOccupancyState();
    int magicalStateMap[4] = {3, 2, 1, 0};                             // Define impossible state transitions (Ex. newOccupancyState cannot equal impossibilityMap[lastOccupancyState])
    
    switch(stateStack.count()){
      case 0:
        if(newOccupancyState == 0){                                     // First value MUST be a 0, ignore others
          stateStack.push(newOccupancyState);   
        }                                             
        break;
      case 1:
        if(newOccupancyState != 0){                                     // Second state must NOT be a 0, ignore others
          stateStack.push(newOccupancyState);                           // Push to the stack without checking for impossibilities
        }
        break;                                                           
      case 2:
      case 3:                                                           // When the stateStack has 2 or 3 items, we must identify impossible patterns and fix them.
        if(stateStack.count() == 2 && newOccupancyState == 0){          // If we receive a 0 as the third state, the detected person left the area without walking in or out ...
          while(stateStack.count() > 1){                                    // ... so reset the stateStack so it only contains 0.
            stateStack.pop();                           // NOTE: IF IT IS COMMON FOR RANDOM 0s TO COME IN RIGHT HERE, MAY NEED TO LET THIS THROUGH AND FIX IT WITH THE NEXT CHANGE
          }
        }
        tempStack.push(newOccupancyState);                              // Push the new occupancyState to the tempStack
        while(stateStack.count() > 1){                                  // Go through the stack containing prior states
          int current = stateStack.pop();                               
          int after = tempStack.peek();
          int before = stateStack.peek();
          if(magicalStateMap[before] == current){                       // If the transition from before --> current is impossible, we must have failed to detect the person at some point ...
            tempStack.push(current);                                            // ... so push current ...
            int missedState = magicalStateMap[after];                               // ... consult the magical state map to determine what state was missed ...
            tempStack.push(missedState);                                                // ... then push that.
          } else if(magicalStateMap[current] == after) {                // If the transition from current --> after is impossible, we must have failed to detect the person at some point ...
            int missedState = magicalStateMap[before];                          // ... so consult the magical state map to determine what state was missed ...
            tempStack.push(missedState);                                            // ... push the missing state ...
            tempStack.push(current);                                                    // ... then push current.
          } else {                                                      // If the transition from before --> current is possible ...
            tempStack.push(current);                                            // ... push current.
          }
        }
        while(!tempStack.isEmpty()){                                            // Then move everything in the tempStack back to the permanent stack
          stateStack.push(tempStack.pop());
        }
        break;
      case 4:
        if(newOccupancyState != 0){                                     // If the new occupancy state is NOT 0 ...
          while(stateStack.peek() != newOccupancyState){                        // ... until the top of the stack is equal to the new occupancy state ...
            stateStack.pop();                                                       // ... remove the top of the stack.
          }
        } else {                                                         // If the new occupancy state is 0 ...
          stateStack.push(newOccupancyState);                                   // ... push the final state ...
          char states[5];                                                           // ... turn it into a string by popping all values off the stack...
          snprintf(states, sizeof(states), "%i%i%i%i%i", stateStack.pop(), stateStack.pop(), stateStack.pop(), stateStack.pop(), stateStack.pop());   
          if(strcmp(states, "01320")){
            occupancyCount++;                                                           // ... then increment the count if the sequence matches the increment secuence.
          } else if(strcmp(states, "02310")) {
            occupancyCount--;                                                           // ... then decrement the count if the sequence matches the decrement secuence.
          } else {
            Log.info("[ERROR WHEN COUNTING] Impossible sequence: %s", states);          // ... then throw an error if the sequence is supposed to be impossible.
          }
        }
        break;
    }

   #if TENFOOTDISPLAY
    if (oldOccupancyCount != occupancyCount) printBigNumbers(occupancyCount);
   #else
    if (oldOccupancyCount != occupancyCount) Log.info("Occupancy %s %i",(occupancyCount > oldOccupancyCount) ? "increased to" : "decreased to", occupancyCount);
   #endif
}

int PeopleCounter::getCount(){
  Log.info("Occupancy count is %d",occupancyCount);
  return occupancyCount;

}

void PeopleCounter::setCount(int value){
  occupancyCount = value;
}

int PeopleCounter::getLimit(){
  return occupancyLimit;
}

void PeopleCounter::setLimit(int value){
  occupancyLimit = value;
}

void PeopleCounter::printBigNumbers(int number) {
  Log.info("  ");

  switch (abs(number)) {
    case 0:
      Log.info("%s  0000  ", (number < 0) ? "      " : "");
      Log.info("%s 0    0 ", (number < 0) ? "      " : "");
      Log.info("%s0      0", (number < 0) ? "      " : "");
      Log.info("%s0      0", (number < 0) ? "------" : "");  
      Log.info("%s0      0", (number < 0) ? "      " : "");     
      Log.info("%s 0    0 ", (number < 0) ? "      " : "");
      Log.info("%s  0000  ", (number < 0) ? "      " : "");
    break;

    case 1:
      Log.info("%s    11  ", (number < 0) ? "      " : "");
      Log.info("%s   1 1  ", (number < 0) ? "      " : "");
      Log.info("%s     1  ", (number < 0) ? "      " : "");
      Log.info("%s     1  ", (number < 0) ? "------" : "");
      Log.info("%s     1  ", (number < 0) ? "      " : "");
      Log.info("%s     1  ", (number < 0) ? "      " : "");
      Log.info("%s   11111", (number < 0) ? "      " : "");
    break;

    case 2:
      Log.info("%s  2222  ", (number < 0) ? "      " : "");
      Log.info("%s 2    22", (number < 0) ? "      " : "");
      Log.info("%s     2  ", (number < 0) ? "      " : "");
      Log.info("%s   2    ", (number < 0) ? "------" : "");
      Log.info("%s  2     ", (number < 0) ? "      " : "");
      Log.info("%s22     2", (number < 0) ? "      " : "");
      Log.info("%s2222222 ", (number < 0) ? "      " : "");
      break;

      case 3:
      Log.info("%s  3333  ", (number < 0) ? "      " : "");
      Log.info("%s 3    3 ", (number < 0) ? "      " : "");
      Log.info("%s       3", (number < 0) ? "      " : "");
      Log.info("%s   333  ", (number < 0) ? "------" : "");
      Log.info("%s       3", (number < 0) ? "      " : "");
      Log.info("%s 3    3 ", (number < 0) ? "      " : "");
      Log.info("%s  3333  ", (number < 0) ? "      " : "");
      break;

      case 4:
      Log.info("%s4      4", (number < 0) ? "      " : "");
      Log.info("%s4      4", (number < 0) ? "      " : "");
      Log.info("%s4      4", (number < 0) ? "      " : "");
      Log.info("%s  4444  ", (number < 0) ? "------" : "");
      Log.info("%s       4", (number < 0) ? "      " : "");
      Log.info("%s       4", (number < 0) ? "      " : "");
      Log.info("%s       4", (number < 0) ? "      " : "");
      break;

      case 5:
      Log.info("%s  555555", (number < 0) ? "      " : "");
      Log.info("%s 5      ", (number < 0) ? "      " : "");
      Log.info("%s 555555 ", (number < 0) ? "      " : "");
      Log.info("%s      5 ", (number < 0) ? "------" : "");
      Log.info("%s       5", (number < 0) ? "      " : "");
      Log.info("%s      5 ", (number < 0) ? "      " : "");
      Log.info("%s 555555 ", (number < 0) ? "      " : "");
      break;

      case 6:
      Log.info("%s  666666", (number < 0) ? "      " : "");
      Log.info("%s 6      ", (number < 0) ? "      " : "");
      Log.info("%s  66666 ", (number < 0) ? "      " : "");
      Log.info("%s6      6", (number < 0) ? "------" : "");
      Log.info("%s6      6", (number < 0) ? "      " : "");
      Log.info("%s 6    6 ", (number < 0) ? "      " : "");
      Log.info("%s  6666  ", (number < 0) ? "      " : "");
      break;

      case 7:
      Log.info("%s  777777", (number < 0) ? "      " : "");
      Log.info("%s 7     7", (number < 0) ? "      " : "");
      Log.info("%s      7 ", (number < 0) ? "      " : "");
      Log.info("%s     7  ", (number < 0) ? "------" : "");
      Log.info("%s    7   ", (number < 0) ? "      " : "");
      Log.info("%s   7    ", (number < 0) ? "      " : "");
      Log.info("%s  7     ", (number < 0) ? "      " : "");
      break;

      case 8:
      Log.info("%s  8888  ", (number < 0) ? "      " : "");
      Log.info("%s 8    8 ", (number < 0) ? "      " : "");
      Log.info("%s8      8", (number < 0) ? "      " : "");
      Log.info("%s  8888  ", (number < 0) ? "------" : "");
      Log.info("%s8      8", (number < 0) ? "      " : "");
      Log.info("%s 8    8 ", (number < 0) ? "      " : "");
      Log.info("%s  8888  ", (number < 0) ? "      " : "");
      break;

      case 9:
      Log.info("%s 99999  ", (number < 0) ? "      " : "");
      Log.info("%s9     9 ", (number < 0) ? "      " : "");
      Log.info("%s9      9", (number < 0) ? "      " : "");
      Log.info("%s 99999  ", (number < 0) ? "------" : "");
      Log.info("%s       9", (number < 0) ? "      " : "");
      Log.info("%s      9 ", (number < 0) ? "      " : "");
      Log.info("%s 999999 ", (number < 0) ? "      " : "");
      break;

      default:
      Log.info("********");
      Log.info("********");
      Log.info("********");
      Log.info("********");
      Log.info("********");
      Log.info("********");
      Log.info("********");
      break;
  }
  Log.info("  ");
}
