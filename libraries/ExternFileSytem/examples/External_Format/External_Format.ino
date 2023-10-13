#include <Adafruit_LittleFS.h>
#include <ExternalFileSystem.h>
#include <Adafruit_TinyUSB.h> // for Serial

using namespace Adafruit_LittleFS_Namespace;

// the setup function runs once when you press reset or power the board
void setup() 
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("ExternalFS Format Example");
  Serial.println();

  // Wait for user input to run.
  Serial.println("This sketch will destroy all of your data in External Flash!");
  Serial.print("Enter any keys to continue:");
  while ( !Serial.available() ) delay(1);
  Serial.println();
  Serial.println();

  // Initialize External File System
  ExternalFS.begin();

  Serial.print("Formating ... ");
  delay(1); // for message appear on monitor

  // Format 
  ExternalFS.format();

  Serial.println("Done");
}

// the loop function runs over and over again forever
void loop() 
{
  // nothing to do
}
