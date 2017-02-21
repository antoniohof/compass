  bool echoed;
void setup()
{
  Serial.begin(38400);

  echoed = false;
}


void loop()
{
  while (Serial.available() > 0)
  {
      Serial.write(Serial.read());
      echoed = true;
  }
  
  if(echoed){
    Serial.write("a");
    echoed = false;
  }

  
  delay(10);
}

