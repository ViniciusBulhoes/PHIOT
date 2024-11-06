int blinkCount = 0;

int p1Points = 0;
int p2Points = 0;

unsigned long prevMillisG = 0;
unsigned long gameStartMillis;
unsigned long p1Millis;
unsigned long p2Millis;

bool gameStart = false;
bool gameRun = false;
bool gameWin1 = false;
bool gameWin2 = false;

bool prevScreen = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  unsigned long curMillis = millis();
  if(curMillis - prevMillis >= 250){
    digitalWrite(2, !digitalRead(2));
    prevMillis = curMillis;
  }*/

  if(touchRead(15) < 20 && !gameStart && !gameRun && !gameWin1 && !gameWin2){
    gameStart = true;
    prevScreen = false;
  }

  if(gameStart){
    unsigned long curMillisG = millis();

    if(curMillisG - prevMillisG >= 500){
      digitalWrite(5, !digitalRead(5));
      prevMillisG = curMillisG;
      blinkCount += 1;
    }

    if(blinkCount >= 6){
      blinkCount = 0;
      digitalWrite(5, LOW);
      gameStart = false;
      gameRun = true;
      gameStartMillis = millis();
    }
  }

  if(gameRun){
    if(touchRead(13) < 20){
      p1Points += 1;
      p1Millis = millis();
      gameWin1 = true;
      gameRun = false;
    }

    if(touchRead(14) < 20){
      Serial.println(touchRead(14));
      p2Points += 1;
      p2Millis = millis();
      gameWin2 = true;
      gameRun = false;
    }

  }

  if(gameWin1){
    digitalWrite(4, HIGH);
    
    if(!prevScreen){
      Serial.print("Jogador 1 pontos: ");
      Serial.println(p1Points);
      Serial.print("Tempo (s): ");
      Serial.println((p1Millis - gameStartMillis)/1000.0);
    }

    if(touchRead(15) < 20){
      digitalWrite(4, LOW);
      digitalWrite(2, LOW);
      gameWin1 = false;
      
    }

    prevScreen = true;
  }

  if(gameWin2){
    digitalWrite(2, HIGH);
  
    if(!prevScreen){
      Serial.print("Jogador 2 pontos: ");
      Serial.println(p2Points);
      Serial.print("Tempo (s): ");
      Serial.println((p2Millis - gameStartMillis)/1000.0);
    }

    if(touchRead(15) < 20){
      digitalWrite(4, LOW);
      digitalWrite(2, LOW);
      gameWin2 = false;
      
    }

    prevScreen = true;
  }

}
