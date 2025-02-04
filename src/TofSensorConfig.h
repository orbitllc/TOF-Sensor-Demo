#ifndef TOFSENSOR_CONFIG_H
#define TOFSENSOR_CONFIG_H

/***   Mounting Parameters   ***/
#define PERSON_THRESHOLD 12                        // Readings that are PERSON_THRESHOLD above (or below) the baseline will trigger an occupancy change
#define NUM_CALIBRATION_LOOPS 20                   // How many samples to take during calibration.


/***   Debugging   ***/
#define DEBUG_COUNTER 0
#define SENSOR_TIMEOUT 500

// Detection zone dimensions and optical centers
#define COLUMNS_OF_SPADS 8                         // This is the width (accross the door with the sensor long axis perpendicular to the threshold) of the active SPADS
#define ROWS_OF_SPADS    6                         // This is the depth (Through the door - when sensor mounted on the inside doorframe)

// Will focus on the SPAD array of 6 rows and 8 columns
#define FRONT_ZONE_CENTER     159
#define BACK_ZONE_CENTER      239



/** The TofSensor has a receiver array consisting of 16X16 Single Photon Diodes. 
 *  By default, all of them are used but you can reduce the receiver to a region of interest (ROI).
 *  It has to be at least 4X4 in size, but can be bigger and have uneven sidelenghts.
 *  It is defined by the sidelengths in x and y direction and its center.
 *  To set the center, set the pad that is to the right and above the exact center of the region you'd like to measure as your opticalCenter
 *  Here is the function call: setROI(uint8_t x, uint8_t y, uint8_t opticalCenter); 
 *
 ***   Here is the Table of Optical Centers   ***
  * Pin 1
  * 128,136,144,152,160,168,176,184,  192,200,208,216,224,232,240,248
  * 129,137,145,153,161,169,177,185,  193,201,209,217,225,233,241,249
  * 130,138,146,154,162,170,178,186,  194,202,210,218,226,234,242,250
  * 131,139,147,155,163,171,179,187,  195,203,211,219,227,235,243,251
  * 132,140,148,156,164,172,180,188,  196,204,212,220,228,236,244,252
  * 133,141,149,157,165,173,181,189,  197,205,213,221,229,237,245,253
  * 134,142,150,158,166,174,182,190,  198,206,214,222,230,238,246,254
  * 135,143,151,159,|167|,175,183,191,  199,207,215,223,|231|,239,247,255
  
  * 127,119,111,103, 95, 87, 79, 71,  63, 55, 47, 39, 31, 23, 15, 7
  * 126,118,110,102, 94, 86, 78, 70,  62, 54, 46, 38, 30, 22, 14, 6
  * 125,117,109,101, 93, 85, 77, 69,  61, 53, 45, 37, 29, 21, 13, 5
  * 124,116,108,100, 92, 84, 76, 68,  60, 52, 44, 36, 28, 20, 12, 4
  * 123,115,107, 99, 91, 83, 75, 67,  59, 51, 43, 35, 27, 19, 11, 3
  * 122,114,106, 98, 90, 82, 74, 66,  58, 50, 42, 34, 26, 18, 10, 2
  * 121,113,105, 97, 89, 81, 73, 65,  57, 49, 41, 33, 25, 17, 9, 1
  * 120,112,104, 96, 88, 80, 72, 64,  56, 48, 40, 32, 24, 16, 8, 0
  */
#endif