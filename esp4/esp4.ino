#include <SoftwareSerial.h>
#define RX 2
#define TX 3
String AP = "JioFi2_C959CD";       // AP NAME
String PASS = "abcdefgh1234"; // AP PASSWORD
String API = "25Y5GJAFDJER1FAV";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
double valSensor = 1;
double valSensor2 = 1;

int gas_sensor = A0; //Sensor pin 
float m = -0.3376; //Slope 
float b = 0.7165; //Y-Intercept 
float R0 = 19.42; //Sensor Resistance in fresh air from previous code

int CO_sensor = A1; //Sensor pin 
float m1 = -0.6527; //Slope 
float b1 = 1.30; //Y-Intercept 
float R01 = 1.35; //Sensor Resistance


SoftwareSerial esp8266(RX,TX); 
 
  
void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {
 valSensor = getSensorData();
 valSensor2 = getSensorData2();
 String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(valSensor)+"&" + field2 + "=" + String(valSensor2);
sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
}

double getSensorData(){
 // Replace with your own sensor code //MQ135
  float sensor_volt; //Define variable for sensor voltage 
  float RS_gas; //Define variable for sensor resistance  
  float ratio; //Define variable for ratio
  float sensorValue = analogRead(gas_sensor); //Read analog values of sensor  
  sensor_volt = sensorValue*(5.0/1023.0); //Convert analog values to voltage 
    RS_gas = ((5.0*10.0)/sensor_volt)-10.0; //Get value of RS in a gas
  ratio = RS_gas/R0;  // Get ratio RS_gas/RS_air
  double ppm_log = (log10(ratio)-b)/m; //Get ppm value in linear scale according to the the ratio value  
  double ppm = pow(10, ppm_log); //Convert ppm value to log scale 
  Serial.print("Our desired PPM = ");
  Serial.println(ppm);
  return ppm;
}

double getSensorData2(){
 // Replace with your own sensor code //MQ7
  float sensor_volt1; //Define variable for sensor voltage 
  float RS_gas1; //Define variable for sensor resistance  
  float ratio1; //Define variable for ratio
  float sensorValue1 = analogRead(CO_sensor); //Read analog values of sensor  
  sensor_volt1 = sensorValue1*(5.0/1023.0); //Convert analog values to voltage 
  RS_gas1 = ((5.0*10.0)/sensor_volt1)-10.0; //Get value of RS in a gas
  ratio1 = RS_gas1/R01;  // Get ratio RS_gas/RS_air
  double ppm_log1 = (log10(ratio1)-b1)/m1; //Get ppm value in linear scale according to the the ratio value  
  double ppm1 = pow(10, ppm_log1); //Convert ppm value to log scale 
  Serial.print("CO PPM = ");
  Serial.println(ppm1);
  return ppm1;
}

int getSensorData3(){
  return analogRead(A1); // Replace with your own sensor code
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
