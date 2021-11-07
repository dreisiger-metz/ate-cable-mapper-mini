// =============================================================================
// Filename         : Cable_Mapper.ino
// Version          : 
//
// Original Author  : Peter Metz
// Date Created     : 29-Oct-2021
//
// Revision History : 29-Oct-2021: Initial version
//
// Purpose          : 
//                     
// Licensing        : Copyright (C) 2021, Peter Metz
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
// details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Notes:
//   - For Arduino Nano v3.0, use ATmega328P / AVRISP mkII
//   - For the Nano clones, use 
// =============================================================================
//#include <Parser.h>


// define DEBUG to enable logging of state changes, etc., to the Serial port;
// note that this approximately triples the size of the resulting code
#define ENABLE_DEBUGGING

#ifdef ENABLE_DEBUGGING
#define LogToSerial(level, message) __LogToSerial(level, message)
#else
#define LogToSerial(level, message)
#endif

enum LogLevel { FATAL, ERROR, WARNING, INFO, VERBOSE, DEBUG };

LogLevel LoggingLevel = INFO;

void __LogToSerial(LogLevel level, String message) {
  if (level <= LoggingLevel)
    Serial.println(message);
}




struct PinLabelPair {
  const byte  pin;
  const char *label;
};


// don't use pin 13 / LED or, on ATmega328-based systems, pins A6 or A7
const PinLabelPair PinMappings[] = {
  { 3,  "A1" }, { 2,  "A2" }, { A5, "A3" }, { A0, "A4" }, { A1, "A5" },
  { A2, "A6" }, { A3, "A7" }, { A4, "A8" }, 
  
  { 12, "B1" }, { 11, "B2" }, { 10, "B3" }, { 9,  "B4" }, { 8,  "B5" },
  { 7,  "B6" }, { 6,  "B7" }, { 5,  "B8" }, { 4,  "B9" },
  
  { 0,  NULL }
};






// =============================================================================
// Function         : map_cable()
//
// Note             : 
// =============================================================================
void map_cable() {
  byte i, j, num;
  String mapping;

  i = 0;
  while (PinMappings[i].label != NULL) {
    // all interface pins have mode INPUT_PULLUP by 'default', so we want to
    // set the 'injection' pin to OUTPUT and LOW and then see what the other
    // (still) input pins show
    pinMode(PinMappings[i].pin, OUTPUT);
    digitalWrite(PinMappings[i].pin, LOW);
    mapping = "";
    
    j = 0;
    num = 0;
    while (PinMappings[j].label != NULL) {
      if ((i != j) && (digitalRead(PinMappings[j].pin) == LOW)) {
        if (mapping != "") {
          mapping += ", ";
        }
        mapping += "\"" + String(PinMappings[j].label) + "\"";
        num++;
      }
      
      j++;
    }

    if (num == 1)
      Serial.println("  {\"" + String(PinMappings[i].label) + "\": " + mapping + "}, ");
    else if (num >= 2)
      Serial.println("  {\"" + String(PinMappings[i].label) + "\": [" + mapping + "] }, ");
    // what about the lack of a comma in the non-terminating case? --- hard to 
    // figure out as we don't know if the next one has any connections...

    // and since we're now done with this pin, return it to mode INPUT_PULLUP
    pinMode(PinMappings[i].pin, INPUT_PULLUP);
    i++;
  }
}




// =============================================================================
// Function         : setup()
//
// Note             : This is the standard Arduino-defined setup function
// =============================================================================
void setup() {
  byte i;

  Serial.begin(115200);

  i = 0;
  while (PinMappings[i].label != NULL) {
    LogToSerial(VERBOSE, "%CABLE_MAPPER-V-INIT, setting interface pin " + String(PinMappings[i].label) + " (" + String(PinMappings[i].pin) + ") to mode INPUT_PULLUP");
    pinMode(PinMappings[i].pin, INPUT_PULLUP);
    
    i++;
  }

  LogToSerial(INFO, "%CABLE_MAPPER-I-INIT, cable mapper initialised");
}


// =============================================================================
// Function         : loop()
//
// Note             : This is the standard Arduino-defined loop function
// =============================================================================
void loop() {
   
  if (Serial.available() > 0) {
    Serial.read();
    Serial.println("[ ");
    map_cable();
    Serial.println("  { } \n]\n");
  }
}
