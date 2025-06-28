#include "utils/config.h"
#include "logger/logger_manager.h"
#include "logger/data_logger.h"
#include "can/can_manager.h"
#include "sensors/gps_manager.h"
#include "sensors/imu_manager.h"
#include "net/ftp_server.h"
#include "utils/led_status.h"
#include "include/data_structs.h"

elapsedMillis startButtonTimer = 0;
elapsedMillis stopButtonTimer = 0;
bool lastStartState = HIGH;
bool lastStopState  = HIGH;

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(BUTTON_START_PIN, INPUT_PULLUP);
  pinMode(BUTTON_STOP_PIN, INPUT_PULLUP);

  initLEDs();

  loggerManager.begin();
  dataLogger.begin();
  canManager.begin();
  gpsManager.begin(Wire2);
  imuManager.begin(Wire);
  ftpServer.begin();

  Serial.println("Logger system initialized");
}

void loop() {
  loggerManager.loop(); 
  dataLogger.loop();     
  canManager.poll();    
  gpsManager.poll();    
  imuManager.poll();     
  ftpServer.poll();       
  updateLEDs();           

  bool startPressed = digitalRead(BUTTON_START_PIN) == LOW;
  bool stopPressed  = digitalRead(BUTTON_STOP_PIN) == LOW;

  if (startPressed && lastStartState == HIGH && startButtonTimer > BUTTON_DEBOUNCE_MS) {
    loggerManager.startLogging();
    Serial.println("Start button pressed");
    startButtonTimer = 0;
  }
  lastStartState = startPressed;

  if (stopPressed && lastStopState == HIGH && stopButtonTimer > BUTTON_DEBOUNCE_MS) {
    loggerManager.stopLogging();
    Serial.println("Stop button pressed");
    stopButtonTimer = 0;
  }
  lastStopState = stopPressed;
}
