/*
  3-Axis CNC Motion-Control Reference Sketch
  For an Arduino + external STEP/DIR stepper drivers (e.g., TB6600/DM-series).

  IMPORTANT:
  - This is an illustrative reference implementation prepared for the portfolio.
  - It is NOT represented as the original source code from the candidate's
    university CNC project, and it has NOT been tested on that machine.
  - NEMA 23 motors must be driven by suitable external stepper drivers.
  - Verify wiring, motor current, microstepping, travel limits, emergency stop,
    spindle safety, and steps/mm before connecting real hardware.
  - This example has no homing routine, limit-switch handling, or emergency-stop
    circuit. Do not use it as production machine-control firmware.

  Serial command format (relative movement in millimetres):
      X10 Y0 Z0
      X-2.5 Y4 Z0
      X0 Y0 Z1

  Send one command per line at 115200 baud.
  Install the "AccelStepper" library from the Arduino Library Manager.
*/

#include <AccelStepper.h>
#include <stdlib.h>
#include <string.h>

// Example Arduino Uno pin assignment; adapt to the actual controller/wiring.
const uint8_t X_STEP_PIN = 2;
const uint8_t X_DIR_PIN  = 3;
const uint8_t Y_STEP_PIN = 5;
const uint8_t Y_DIR_PIN  = 6;
const uint8_t Z_STEP_PIN = 8;
const uint8_t Z_DIR_PIN  = 9;

const uint8_t X_ENABLE_PIN = 10;
const uint8_t Y_ENABLE_PIN = 11;
const uint8_t Z_ENABLE_PIN = 12;

// EXAMPLE calibration only. Calculate from motor steps/rev, microstepping,
// and screw/belt travel. These values are not asserted for the candidate's machine.
const float X_STEPS_PER_MM = 80.0;
const float Y_STEPS_PER_MM = 80.0;
const float Z_STEPS_PER_MM = 400.0;

const float MAX_SPEED_STEPS_S = 1200.0;
const float ACCEL_STEPS_S2    = 500.0;

AccelStepper xAxis(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper yAxis(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
AccelStepper zAxis(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN);

char inputLine[80];
uint8_t inputIndex = 0;

void configureAxis(AccelStepper &axis, uint8_t enablePin) {
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW); // Common active-LOW enable; check driver manual.
  axis.setMaxSpeed(MAX_SPEED_STEPS_S);
  axis.setAcceleration(ACCEL_STEPS_S2);
}

void printHelp() {
  Serial.println(F("Enter relative movement in mm, e.g. X10 Y-2 Z0"));
  Serial.println(F("This reference sketch has no homing or limit-switch protection."));
}

void processCommand(char *line) {
  float dx = 0.0, dy = 0.0, dz = 0.0;
  bool hasAxis = false;

  char *token = strtok(line, " ,\t");
  while (token != NULL) {
    char axisName = token[0];
    char *endPtr = NULL;
    float value = strtof(token + 1, &endPtr);

    if (endPtr == token + 1 || *endPtr != '\0') {
      Serial.print(F("Invalid token: "));
      Serial.println(token);
      return;
    }

    switch (axisName) {
      case 'X': case 'x': dx = value; hasAxis = true; break;
      case 'Y': case 'y': dy = value; hasAxis = true; break;
      case 'Z': case 'z': dz = value; hasAxis = true; break;
      default:
        Serial.println(F("Unknown axis. Use X, Y, and/or Z."));
        return;
    }
    token = strtok(NULL, " ,\t");
  }

  if (!hasAxis) {
    printHelp();
    return;
  }

  long xTarget = xAxis.currentPosition() + lround(dx * X_STEPS_PER_MM);
  long yTarget = yAxis.currentPosition() + lround(dy * Y_STEPS_PER_MM);
  long zTarget = zAxis.currentPosition() + lround(dz * Z_STEPS_PER_MM);

  xAxis.moveTo(xTarget);
  yAxis.moveTo(yTarget);
  zAxis.moveTo(zTarget);

  Serial.println(F("Movement command accepted (relative coordinates)."));
}

void setup() {
  Serial.begin(115200);

  configureAxis(xAxis, X_ENABLE_PIN);
  configureAxis(yAxis, Y_ENABLE_PIN);
  configureAxis(zAxis, Z_ENABLE_PIN);

  Serial.println(F("CNC 3-axis motion-control reference sketch ready."));
  printHelp();
}

void loop() {
  // Non-blocking motion updates allow all three axes to move concurrently.
  xAxis.run();
  yAxis.run();
  zAxis.run();

  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (c == '\n' || c == '\r') {
      if (inputIndex > 0) {
        inputLine[inputIndex] = '\0';
        processCommand(inputLine);
        inputIndex = 0;
      }
    } else if (inputIndex < sizeof(inputLine) - 1) {
      inputLine[inputIndex++] = c;
    } else {
      inputIndex = 0;
      Serial.println(F("Input too long; command discarded."));
    }
  }
}
