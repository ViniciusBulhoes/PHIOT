#include <FS.h>
#include <WiFi.h>
#include "time.h"
#include "SPIFFS.h"

#define butPin 32
#define ledPin 33

String credential = "NPITI-IoT";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -3 * 3600;
const int   daylightOffset_sec = 3600;

String s;
String lastS;
String lastState = "off";

bool prevBut = true;
bool currBut;
bool ledState;

unsigned long press_time = 0;          
unsigned long debounce = 200;

String formattedDateTime;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    formattedDateTime = "Failed to obtain time";
    formattedDateTime.replace("\n", " ");
    formattedDateTime.trim();
    return;
  }

  // Create a buffer for the formatted date/time string
  char timeStr[40];  // Adjust size as needed for your format

  // Format the time into the timeStr buffer (you can customize the format)
  strftime(timeStr, sizeof(timeStr), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  
  // Store the formatted date/time into the String variable
  formattedDateTime = String(timeStr);
  formattedDateTime.replace("\n", " ");
  formattedDateTime.trim();
  
}

void openFS() {
  if (!SPIFFS.begin()) {
    Serial.println("\nErro ao abrir o sistema de arquivos");
  }
  else {
    Serial.println("\nSistema de arquivos aberto com sucesso!");
  }
}

void readFile(String path) {
  Serial.println("Read file");
  File rFile = SPIFFS.open(path, "r");
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  else {
    Serial.print("----------Lendo arquivo ");
    Serial.print(path);
    Serial.println("  ---------");
    int count = 0;
    while (rFile.position() < rFile.size())
    {
      if(count != 0){
        lastS = s;
      }

      s = rFile.readStringUntil('\n');
      s.trim();
      Serial.println(s);

      int aux = s.lastIndexOf(" ");
      if(aux != -1){
        lastState = s.substring(aux+1);
      }else{
        lastState = s;
      }
      
      count += 1;
    }
    rFile.close();
  }
}

void writeFile(String state, String path){
  File rFile = SPIFFS.open(path, "a");
  if(!rFile){
    Serial.println("Unable to open file");
  }else{
    rFile.print(state);
  }

  rFile.close();
}

void setup(){
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting now");
  openFS(); // start SPIFFIS

  WiFi.begin(credential, credential); // connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  pinMode(butPin, INPUT);
  pinMode(ledPin, OUTPUT);
  readFile("/logLedState.txt");

  if(lastState.compareTo("on") == 0){
    ledState = true;
    digitalWrite(ledPin, HIGH);
  }else{
    ledState = false;
    digitalWrite(ledPin, LOW);
  }
}

void loop(){
  currBut = digitalRead(butPin);
  
  if(currBut == LOW && prevBut == HIGH && millis() - press_time > debounce){
    printLocalTime();
    Serial.print(formattedDateTime);
    writeFile(formattedDateTime, "/logLedState.txt");

    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    if(ledState){
      Serial.println(" on");
      String a (" on\n");
      writeFile(a, "/logLedState.txt");
    }else{
      Serial.println(" off");
      String a (" off\n");
      writeFile(a, "/logLedState.txt");
    }
        
    press_time = millis();
  }

  prevBut = currBut;
}