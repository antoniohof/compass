#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

void setup()
{

  Serial.begin(38400);
  delay(1000);

  Serial.println("[SETUP] - initializing sensor");


   if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("[SETUP] - Ooops, no HMC5883 detected ... Check your wiring!");
    delay(100);    
    while(1);
  }else {
      Serial.println("[SETUP] - successfully conected to mag!");
  }

    displaySensorDetails();
}


void loop()
{
  /* Get a new sensor event */ 
  sensors_event_t event; 
  mag.getEvent(&event);

  String data;
  data.concat(event.magnetic.x*100);
  data.concat("/");
  data.concat(event.magnetic.y*100);  
  data.concat("/");
  data.concat(event.magnetic.z*100);
  
  Serial.print(data);

  //separator
  Serial.print("a");

  delay(20);
}




void displaySensorDetails(void)
{
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}
