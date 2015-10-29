const int CCW = 11; // Counter-clockwise
const int CW = 10;  // Clockwise
const int water = 5;
const int dist = 9;
int state = 0;
#define IR_PROXIMITY_SENSOR A1
#define ADC_REF 5


void setup()
{
  pinMode(CCW, OUTPUT);
  pinMode(CW, OUTPUT);
  pinMode(water, INPUT);
  pinMode(dist, INPUT);
}


void loop()
{  
  if ((digitalRead(water) == 0) && (digitalRead(dist) == 1) && (state == 0)) // IF WET, NO OBJECT & OPEN
  {
    while(getVoltage() <= 2.25) // while window is > 10 cm away from IR sensor
    {
      if ((digitalRead(dist) == HIGH)) // OBJECT ABSENT
      {
        digitalWrite(CW, HIGH); // To CLOSE window
        digitalWrite(CCW, LOW);
      }
      while (digitalRead(dist) == LOW) // OBJECT DETECTED
      {
        digitalWrite(CW, LOW);
        digitalWrite(CCW, LOW); 
      }
    }
    state = 1; // Window is now closed
  }

  else if ((digitalRead(water) == 1) && (state == 1)) // IF DRY & CLOSED
  {
    while(getVoltage() >= 0.75) // while window is < 40 cm away from IR sensor
    {
      if ((digitalRead(dist) == HIGH)) // OBJECT ABSENT
      {
        digitalWrite(CW, LOW);
        digitalWrite(CCW, HIGH); // To OPEN window
      }
      while (digitalRead(dist) == LOW) // OBJECT DETECTED
      {
        digitalWrite(CW, LOW);
        digitalWrite(CCW, LOW);
      }
    }
    state = 0; // Window is now open
  }

  else
  {
    digitalWrite(CW, LOW);
    digitalWrite(CCW, LOW);
  }


}


// Function: Get voltage from the sensor pin that is connected with analog pin
// Parameter:-void                           
// Return: -float,the voltage of the analog pin            
float getVoltage()
{
  int sensor_value;
  int sum;
  // read the analog in value:
  for (int i = 0; i < 20; i ++) //Continuous sampling 20 times
  {
    sensor_value = analogRead(IR_PROXIMITY_SENSOR);
    sum += sensor_value;
  }
  sensor_value = sum / 20;
  float voltage;
  voltage = (float)sensor_value * ADC_REF / 1024;
  return voltage;
}







