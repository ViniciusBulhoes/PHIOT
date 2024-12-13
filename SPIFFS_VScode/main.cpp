#include <Arduino.h>
#include <FS.h>
#include <WiFi.h>
#include "SPIFFS.h"

#define ledPin1 18
#define ledPin2 19

String wifi;
String password;
String modo; // recebe "normal" ou "morse"
int vel1;
int vel2;
String estado_inicial; // recebe "on" ou "off"

int blink_count1 = 0;
int blink_count2 = 0;

bool ledS1;
bool ledS2;

unsigned long time1;
unsigned long time2;

String s;

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
    
    while (rFile.position() < rFile.size())
    {
      s = rFile.readStringUntil(';');
      s.trim();
      Serial.println(s);

      int aux = s.lastIndexOf(" ");
	    String word_aux = s.substring(0, aux);
      word_aux.trim();
      
      if(aux != -1){
        if(word_aux.equals("wifi =")){
          wifi = s.substring(aux+1);
        }
        
        if(word_aux.equals("password =")){
          password = s.substring(aux+1);
        }
        
        if(word_aux.equals("modo =")){
          modo = s.substring(aux+1);
        }
        
        if(word_aux.equals("vel1 =")){
          vel1 = s.substring(aux+1).toInt();
        }
        
        if(word_aux.equals("vel2 =")){
          vel2 = s.substring(aux+1).toInt();
        }
        
        if(word_aux.equals("estado_inicial =")){
          estado_inicial = s.substring(aux+1);
        }
        
      }
    
    }

    rFile.close();
  }
}

void setup(){
  Serial.begin(115200);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  openFS();
  readFile("/teste.txt");
    
  WiFi.begin(wifi, password); // connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to wifi");
  
  if(estado_inicial.equals("on")){
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
  }else{
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
  }
  time1 = millis();
  time2 = millis();
}

void loop(){
  ledS1 = digitalRead(ledPin1);
  ledS2 = digitalRead(ledPin2);
  if(modo.equals("normal")){
    if(millis()-time1 >= vel1){
      digitalWrite(ledPin1, !ledS1);
      time1 = millis();
    }

    if(millis()-time2 >= vel2){
      digitalWrite(ledPin2, !ledS2);
      time2 = millis();
    }

  }

  if(modo.equals("morse")){
    if(blink_count1 < 5){
      if(millis()-time1 >= vel1){
        digitalWrite(ledPin1, !ledS1);
        time1 = millis();
        blink_count1 += 1;
      }
    }else{
      if(millis()-time1 >= 3*vel1){
        digitalWrite(ledPin1, !ledS1);
        time1 = millis();
        blink_count1 = 0;
      }
    }

    if(blink_count2 < 5){
      if(millis()-time2 >= vel2){
        digitalWrite(ledPin2, !ledS2);
        time2 = millis();
        blink_count2 += 1;
      }
    }else{
      if(millis()-time2 >= 3*vel2){
        digitalWrite(ledPin2, !ledS2);
        time2 = millis();
        blink_count2 = 0;
      }
    }
  }
}