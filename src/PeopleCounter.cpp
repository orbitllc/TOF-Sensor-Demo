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

void PeopleCounter::loop(){                // This function is only called if there is a change in occupancy state
    static byte oldOccupancyState = 0;
    static bool atTheThreshold = false;
    static bool oldOccupancyCount = occupancyCount;

    switch (TofSensor::instance().getOccupancy()) {

      case 0:                               // No occupancy detected
        oldOccupancyState = 0;
      break;

      case 1:
        if (atTheThreshold) {
          atTheThreshold = false;
          if (oldOccupancyState == 2) {
            #if PEOPLECOUNTER_DEBUG
            Log.info("Entered the room");
            #endif
            occupancyCount++; 
          }
        }
        oldOccupancyState = 1;
      break;

      case 2:
        if (atTheThreshold) {
          atTheThreshold = false;
          if (oldOccupancyState == 1) {
            #if PEOPLECOUNTER_DEBUG
            Log.info("Left the room");
            #endif
            occupancyCount--;
          }
        }
        oldOccupancyState = 2;
      break;

      case 3:
        atTheThreshold = true;
      break;

      default:
        #if PEOPLECOUNTER_DEBUG
        Log.info("Error in occupancy state");
        #endif
      break;
    }

    #if TENFOOTDISPLAY
    if (oldOccupancyCount != occupancyCount) printBigNumbers(occupancyCount);
    #else
    if (oldOccupancyCount != occupancyCount) Log.info("Occupancy %s %i",(occupancyCount > oldOccupancyCount) ? "increased to" : "decreased to", occupancyCount);
    #endif
    oldOccupancyCount = occupancyCount;
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
  if (number < 0){
    Log.info("        ");
    Log.info("        ");    
    Log.info("        ");
    Log.info("--------");
    Log.info("        ");
    Log.info("        ");
    Log.info("        ");
  } 

  Log.info("  ");

  switch (abs(number)) {
    case 0:
      Log.info("  0000  ");
      Log.info(" 0    0 ");
      Log.info("0      0");
      Log.info("0      0");  
      Log.info("0      0");     
      Log.info(" 0    0 ");
      Log.info("  0000  ");
    break;

    case 1:
      Log.info("    11  ");
      Log.info("   1 1  ");
      Log.info("     1  ");
      Log.info("     1  ");
      Log.info("     1  ");
      Log.info("     1  ");
      Log.info("     1  ");
      Log.info("   11111");
    break;

    case 2:
      Log.info("  2222  ");
      Log.info(" 2    22");
      Log.info("     2  ");
      Log.info("   2    ");
      Log.info("  2     ");
      Log.info("22     2");
      Log.info("2222222 ");
      break;

      case 3:
      Log.info("  3333  ");
      Log.info(" 3    3 ");
      Log.info("       3");
      Log.info("   333  ");
      Log.info("       3");
      Log.info(" 3    3 ");
      Log.info("  3333  ");
      break;

      case 4:
      Log.info("4      4");
      Log.info("4      4");
      Log.info("4      4");
      Log.info("  4444  ");
      Log.info("       4");
      Log.info("       4");
      Log.info("       4");
      break;

      case 5:
      Log.info("  555555");
      Log.info(" 5      ");
      Log.info(" 555555 ");
      Log.info("      5 ");
      Log.info("       5");
      Log.info("      5 ");
      Log.info(" 555555 ");
      break;

      case 6:
      Log.info("  666666");
      Log.info(" 6      ");
      Log.info("  66666 ");
      Log.info("6      6");
      Log.info("6      6");
      Log.info(" 6    6 ");
      Log.info("  6666  ");
      break;

      case 7:
      Log.info("  777777");
      Log.info(" 7     7");
      Log.info("      7 ");
      Log.info("     7  ");
      Log.info("    7   ");
      Log.info("   7    ");
      Log.info("  7     ");
      break;

      case 8:
      Log.info("  8888  ");
      Log.info(" 8    8 ");
      Log.info("8      8");
      Log.info("  8888  ");
      Log.info("8      8");
      Log.info(" 8    8 ");
      Log.info("  8888  ");
      break;

      case 9:
      Log.info(" 99999  ");
      Log.info("9     9 ");
      Log.info("9      9");
      Log.info(" 99999  ");
      Log.info("       9");
      Log.info("      9 ");
      Log.info(" 999999 ");
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
