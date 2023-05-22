// Time of Flight Sensor Class
// Author: Chip McClelland
// Date: May 2023
// License: GPL3
// This is the class for the ST Micro VL53L1X Time of Flight Sensor
// We are using the Sparkfun library which has some shortcomgings
// - It does not implement distance mode medium
// - It does not give access to the factory calibration of the optical center

#include "Particle.h"
#include "ErrorCodes.h"
#include "TofSensorConfig.h"
#include "PeopleCounterConfig.h"
#include "TofSensor.h"

uint8_t opticalCenters[2] = {58,193};  //90,194 with 4x4 worked
//uint8_t opticalCenters[2] = {58,193};
int zoneDistances[2] = {0,0};
byte occupancyState = 0;      // This is the current occupancy state (occupied or not, zone 1 (ones) and zone 2 (twos)

TofSensor *TofSensor::_instance;

// [static]
TofSensor &TofSensor::instance() {
  if (!_instance) {
      _instance = new TofSensor();
  }
  return *_instance;
}

TofSensor::TofSensor() {
}

TofSensor::~TofSensor() {
}

SFEVL53L1X myTofSensor;

void TofSensor::setup(){
  if(myTofSensor.begin() != 0){
    Log.info("Sensor error reset in 10 seconds");
    delay(10000);
    System.reset();
  }
  else Log.info("Sensor init successfully");
  
  // Here is where we set the device properties
  myTofSensor.setDistanceModeLong();
  myTofSensor.setTimingBudgetInMs(20);

  if (TofSensor::performCalibration()) Log.info("Calibration Complete");
  else {
    Log.info("Initial calibration failed - wait 10 secs and reset");
    delay(10000);
    System.reset();
  }

  // myTofSensor.setDistanceModeShort();                     // Once initialized, we are focused on the top half of the door

}

bool TofSensor::performCalibration() {
  TofSensor::loop();                  // Get the latest values
  if (occupancyState != 0){
    Log.info("Target zone not clear - will wait ten seconds and try again");
    delay(10000);
    TofSensor::loop();
    if (occupancyState != 0) return FALSE;
  }
  Log.info("Target zone is clear with zone1 at %imm and zone2 at %imm",zoneDistances[0],zoneDistances[1]);
  return TRUE;
}

int TofSensor::loop(){                         // This function will update the current distance / occupancy for each zone.  It will return true if occupancy changes                    
  byte oldOccupancyState = occupancyState;
  occupancyState = 0;
  unsigned long startedRanging;

  for (byte zone = 0; zone < 2; zone++){
    myTofSensor.stopRanging();
    myTofSensor.clearInterrupt();
    myTofSensor.setROI(zoneX,zoneY,opticalCenters[zone]);
    myTofSensor.startRanging();

    startedRanging = millis();
    while(!myTofSensor.checkForDataReady()) {
      if (millis() - startedRanging > SENSOR_TIMEOUT) {
        Log.info("Sensor Timed out");
        return SENSOR_TIMEOUT_ERROR;
      }
    }
    zoneDistances[zone] = myTofSensor.getDistance();
    #if DEBUG_COUNTER
    Log.info("Zone%d (%dx%d %d SPADs with optical center %d) = %imm",zone+1,myTofSensor.getROIX(), myTofSensor.getROIY(), myTofSensor.getSpadNb(),opticalCenters[zone],zoneDistances[zone]);
    delay(500);
    #endif

    bool occupied = ((zoneDistances[zone] < PERSON_THRESHOLD) && (zoneDistances[zone] > DOOR_THRESHOLD));
    occupancyState += occupied * (zone +1);
  }

  #if PEOPLECOUNTER_DEBUG
  if (occupancyState != oldOccupancyState) Log.info("Occupancy state changed from %d to %d (%imm / %imm)", oldOccupancyState, occupancyState, zoneDistances[0], zoneDistances[1]);
  #endif

  return (occupancyState != oldOccupancyState);     // Let us know if the occupancy state has changed
}

int TofSensor::getZone1() {
  return zoneDistances[0];
}

int TofSensor::getZone2() {
  return zoneDistances[1];
}

byte TofSensor::getOccupancy() {
  return occupancyState;
}



