#include <SoftwareSerial.h>

#define DEBUG true
const int obstruction_sensor = 9;
const int water_sensor = 5;
SoftwareSerial esp8266(2, 3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
// This means that you need to connect the TX line from the esp to the Arduino's pin 2
// and the RX line from the esp to the Arduino's pin 3
const int CCW = 11;
const int CW = 10;
const int water = 5;
const int dist = 9;
const int LED = 3;
const int LED2 = 7;
int state = 0;

#define IR_PROXIMITY_SENSOR A1
#define ADC_REF 5
float voltage;


void setup()
{
  Serial.begin(115200);
  esp8266.begin(115200); // your esp's baud rate might be different

  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  pinMode(water_sensor, INPUT);
  pinMode(obstruction_sensor, INPUT);

  pinMode(CCW, OUTPUT);
  pinMode(CW, OUTPUT);
  pinMode(water, INPUT);
  pinMode(dist, INPUT);
  pinMode(3, OUTPUT);
  pinMode(LED2, OUTPUT);

  sendCommand("AT+RST\r\n", 2000, DEBUG); // reset module
  sendCommand("AT+CWMODE=1\r\n", 1000, DEBUG); // configure as access point
  sendCommand("AT+CWJAP=\"BROADWATER\",\"quadricep01\"\r\n", 3000, DEBUG);
  delay(10000);
  sendCommand("AT+CIFSR\r\n", 1000, DEBUG); // get ip address
  sendCommand("AT+CIPMUX=1\r\n", 1000, DEBUG); // configure for multiple connections
  sendCommand("AT+CIPSERVER=1,53\r\n", 1000, DEBUG); // turn on server on port 80

  Serial.println("Server Ready");
}

void loop()
{
  if (esp8266.available()) // check if the esp is sending a message
  {


    if (esp8266.find("+IPD,"))
    {
      delay(1000); // wait for the serial buffer to fill up (read all the serial data)
      // get the connection id so that we can then disconnect
      int connectionId = esp8266.read() - 48; // subtract 48 because the read() function returns

      // Build string to send back to device that is requesting pin toggle
      String content;
      content = "Status: ";

      if (digitalRead(water_sensor) == HIGH) // NO WATER DETECTED
      {
        content += "No Rain, Window ";

        if (digitalRead(obstruction_sensor) == HIGH) // IF NO OBSTRUCTION
        {
          digitalWrite(LED2, !digitalRead(LED2)); // toggle pin

          if (digitalRead(LED2)) // LED ON/TRUE = WINDOW CLOSED
          {
            content += "Closed.";
            while (getVoltage() <= 2.25) // Moves window until distance reached
            {
              digitalWrite(CW, HIGH); // To CLOSE window
              digitalWrite(CCW, LOW);
            }
            digitalWrite(CW, LOW);
            digitalWrite(CCW, LOW); // Turn OFF Motor
          }
          else
          {
            content += "Open.";
            while (getVoltage() >= 0.75)
            {
              digitalWrite(CW, LOW);
              digitalWrite(CCW, HIGH); // To OPEN window
            }
            digitalWrite(CW, LOW);
            digitalWrite(CCW, LOW); // Turn OFF Motor
          }
        }
        else
        {
          content += "Obstructed.";
          digitalWrite(CW, LOW);
          digitalWrite(CCW, LOW);
        }
      }
      else   // WATER DETECTED
      {
        content += "Raining, Window ";

        if (digitalRead(obstruction_sensor) == HIGH)  // NO OBSTRUCTION
        {
          content += "Closed.";
        }
        else
        {
          content += "Obstructed.";
          digitalWrite(CW, LOW);
          digitalWrite(CCW, LOW);
        }
      }


      sendHTTPResponse(connectionId, content);

      // make close command
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId; // append connection id
      closeCommand += "\r\n";

      sendCommand(closeCommand, 1000, DEBUG); // close connection
    }
  }
}

/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug)
{
  String response = "";

  int dataSize = command.length();
  char data[dataSize];
  command.toCharArray(data, dataSize);

  esp8266.write(data, dataSize); // send the read character to the esp8266
  if (debug)
  {
    Serial.println("\r\n====== HTTP Response From Arduino ======");
    Serial.write(data, dataSize);
    Serial.println("\r\n========================================");
  }

  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {

      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }

  if (debug)
  {
    Serial.print(response);
  }

  return response;
}

/*
* Name: sendHTTPResponse
* Description: Function that sends HTTP 200, HTML UTF-8 response
*/
void sendHTTPResponse(int connectionId, String content)
{

  // build HTTP response
  String httpResponse;
  String httpHeader;
  // HTTP Header
  httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n";
  httpHeader += "Content-Length: ";
  httpHeader += content.length();
  httpHeader += "\r\n";
  httpHeader += "Connection: close\r\n\r\n";
  httpResponse = httpHeader + content + " "; // There is a bug in this code: the last character of "content" is not sent, I cheated by adding this extra space
  sendCIPData(connectionId, httpResponse);
}

/*
* Name: sendCIPDATA
* Description: sends a CIPSEND=<connectionId>,<data> command
*
*/
void sendCIPData(int connectionId, String data)
{
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += data.length();
  cipSend += "\r\n";
  sendCommand(cipSend, 1000, DEBUG);
  sendData(data, 1000, DEBUG);
}

/*
* Name: sendCommand
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendCommand(String command, const int timeout, boolean debug)
{
  String response = "";

  esp8266.print(command); // send the read character to the esp8266

  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {

      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }

  if (debug)
  {
    Serial.print(response);
  }

  return response;
}

/****************************************************************************/
/*Function: Get voltage from the sensor pin that is connected with analog pin*/
/*Parameter:-void                           */
/*Return: -float,the voltage of the analog pin            */
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
