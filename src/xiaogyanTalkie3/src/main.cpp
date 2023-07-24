#include <Arduino.h>
#include "Application.h"

// our application
Application *application;

void setup()
{
//  Serial.begin(115200);
  Serial.begin(460800);
  // start up the application
  application = new Application();
  application->begin();
  Serial.println("XIAOGYAN Walkie Talkie Application started");
}

void loop()
{
  // nothing to do - the application is doing all the work
  vTaskDelay(pdMS_TO_TICKS(1000));
}
