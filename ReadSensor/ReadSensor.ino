#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* This driver reads raw data from the BNO055

   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3V DC
   Connect GROUND to common ground

   History
   =======
   2015/MAR/03  - First release (KTOWN)
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (50)

Adafruit_BNO055 bno = Adafruit_BNO055();

void setup()
{

  Serial.begin(38400);
  delay(1000);

  Serial.println("[SETUP] - initializing sensor");

  //connect to sensor
  if(!bno.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("[SETUP] - Ooops, no BNO055 detected ... Check your wiring!");
    delay(100);    
    while(1);
  }else {
      Serial.println("[SETUP] - successfully conected to sensor!");
  }

 /* Display the current temperature */
  int8_t temp = bno.getTemp();
  Serial.print("Current Temperature: ");
  Serial.print(temp);
  Serial.println(" C");

  bno.setExtCrystalUse(true);

  
  }


void loop()
{
  /* Get a new sensor event */ 
  imu::Quaternion quat = bno.getQuat();

  String data;
  data.concat(quat.x()*100);
  data.concat(",");
  data.concat(quat.y()*100);  
  data.concat(",");
  data.concat(quat.z()*100);
  data.concat(",");
  data.concat(quat.w()*100);

  Serial.println(data);
  //writeString(data);
  
  //separator
  //String lineEnding = "\n";
  //writeString(lineEnding);

  delay(BNO055_SAMPLERATE_DELAY_MS);
}

void writeString(String stringData) { // Used to serially push out a String with Serial.write()

  for (int i = 0; i < stringData.length(); i++)
  {
    Serial.write(stringData[i]);   // Push each char 1 by 1 on each loop pass
  }

}


