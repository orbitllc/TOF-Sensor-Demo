// Time of Flight Sensor Class
// Author: Chip McClelland
// Date: May 2023
// License: GPL3
// This is the class for the ST Micro VL53L1X Time of Flight Sensor
// We are using the Sparkfun library which has some shortcomgings
// - It does not implement distance mode medium
// - It does not give access to the factory calibration of the optical center

#ifndef __TOFSENSOR_H
#define __TOFSENSOR_H

#include "Particle.h"
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X

/**
 * This class is a singleton; you do not create one as a global, on the stack, or with new.
 * 
 * From global application setup you must call:
 * TofSensor::instance().setup();
 * 
 * From global application loop you must call:
 * TofSensor::instance().loop();
 */
class TofSensor {
public:
    /**
     * @brief Gets the singleton instance of this class, allocating it if necessary
     * 
     * Use TofSensor::instance() to instantiate the singleton.
     */
    static TofSensor &instance();

    /**
     * @brief Perform setup operations; call this from global application setup()
     * 
     * You typically use TofSensor::instance().setup();
     */
    void setup();

    /**
     * @brief Perform application loop operations; call this from global application loop()
     * This function will test for any change in occupancy in zone1 or zone2 and return true or false if there is a change
     * 
     * You typically use TofSensor::instance().update();
     */
    int loop();

    /**
     * @brief These functions will return the current distance measurement in mm for each of the zones.
     * 
     * These functions do not trigger an update, they simply return the current value
    */
    int getZone1();

    /**
     * @brief These functions will return the current distance measurement in mm for each of the zones.
     * 
     * These functions do not trigger an update, they simply return the current value
    */
    int getZone2();

    /**
     * @brief Function to return the current occupancy state
     * 
     * @details Uses BCD to assign the value (1 occupied/ 0 not occupied) / (zone1 - ones, zone2 - twos)
     * Some examples value = 1 (zone1 occupied, zone2 not occupied), value = 3 - (both zones occupied)
    */
    int getOccupancyState();

    /**
     * @brief This function is called as part of the startup process to ensure the sensor does not see any obstructions
     * 
     * Protected as this should only be called from the TofSensor setup process.
     * 
    */
    bool performCalibration();


protected:
    /**
     * @brief The constructor is protected because the class is a singleton
     * 
     * Use TofSensor::instance() to instantiate the singleton.
     */
    TofSensor();

    /**
     * @brief The destructor is protected because the class is a singleton and cannot be deleted
     */
    virtual ~TofSensor();

    /**
     * This class is a singleton and cannot be copied
     */
    TofSensor(const TofSensor&) = delete;

    /**
     * This class is a singleton and cannot be copied
     */
    TofSensor& operator=(const TofSensor&) = delete;

    /**
     * @brief Singleton instance of this class
     * 
     * The object pointer to this class is stored here. It's NULL at system boot.
     */
    static TofSensor *_instance;

    SFEVL53L1X myTofSensor;                 // Only called from this class

};
#endif  /* __TOFSENSOR_H */
