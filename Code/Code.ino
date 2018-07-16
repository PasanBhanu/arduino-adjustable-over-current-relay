#include <LiquidCrystal.h>

// Display
LiquidCrystal LCD(12, 11, 5, 4, 3, 2);

// Pin Assaignment
const int currentPin = A0;
const int relayPin = 53;
const int buzzerPin = 52;
const int btnOk = 26;
const int btnUp = 25;
const int btnDown = 24;
const int btnLeft = 22;
const int btnRight = 23;

// System Variables
int systemStatus = 0;
float allowedMax = 0.0;
float acCurrent = 0.0;
int num1 = 0;
int num2 = 0;
int num3 = 4;
int num4 = 0;
int num1Status = 0;
int num2Status = 0;
int num3Status = 0;
int num4Status = 0;
int selectedNumber = 1;

// Event Variables
// Zero Error Sample
float zeroError = 0.0;

void setup() {
  LCD.begin(16, 2);
  
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(btnOk, INPUT);
  pinMode(btnUp, INPUT);
  pinMode(btnDown, INPUT);
  pinMode(btnLeft, INPUT);
  pinMode(btnRight, INPUT);
  
  // Turn Off Buzzer
  digitalWrite(buzzerPin, HIGH);
}

void loop() {
  
  // Starting Program
  if (systemStatus == 0){
    LCD.setCursor(0,0);
    LCD.print("Adjustable  Over");
    LCD.setCursor(0,1);
    LCD.print(" Current  Relay ");
    delay(1000);
    systemStatus = 1;
  }
  
  // Maximum Current Setup Start
  if (systemStatus == 1){
    LCD.setCursor(0,0);
    LCD.print("Set Max Current ");
    LCD.setCursor(0,1);
    LCD.print("  .            ");
    printDefault();
    systemStatus = 2;
  }
  
  // Maximum Current Setup
  if (systemStatus == 2){
    // Trigger Button Press Events
    if (digitalRead(btnRight) == LOW){
      if (selectedNumber < 4){
        selectedNumber = selectedNumber + 1;
        // Print default
        printDefault();
      }
    }
    if (digitalRead(btnLeft) == LOW){
      if (selectedNumber > 1){
        selectedNumber = selectedNumber - 1;
        // Print default
        printDefault();
      }
    }
    if (digitalRead(btnOk) == LOW){
      String maxLimit = String(num1) + String(num2) + "." + String(num3) + String(num4);
      allowedMax = maxLimit.toFloat();
      systemStatus = 3;
    }
    // Blink Number
    switch (selectedNumber){
      case 1:
        if (num1Status == 0){
          LCD.setCursor(0, 1);
          LCD.print("_");
          num1Status = 1;
        }else{
          LCD.setCursor(0, 1);
          LCD.print(num1);
          num1Status = 0;
        }
        break;
      case 2:
        if (num2Status == 0){
          LCD.setCursor(1, 1);
          LCD.print("_");
          num2Status = 1;
        }else{
          LCD.setCursor(1, 1);
          LCD.print(num2);
          num2Status = 0;
        }
        break;
      case 3:
        if (num3Status == 0){
          LCD.setCursor(3, 1);
          LCD.print("_");
          num3Status = 1;
        }else{
          LCD.setCursor(3, 1);
          LCD.print(num3);
          num3Status = 0;
        }
        break;
      case 4:
        if (num4Status == 0){
          LCD.setCursor(4, 1);
          LCD.print("_");
          num4Status = 1;
        }else{
          LCD.setCursor(4, 1);
          LCD.print(num4);
          num4Status = 0;
        }
        break;
    }
    // Up Down
    if (digitalRead(btnUp) == LOW){
      switch (selectedNumber){
        case 1:
          if (num1 < 1){
            num1 = num1 + 1;
          }
          break;
        case 2:
          if (num2 < 9){
            num2 = num2 + 1;
          }
          break;
        case 3:
          if (num3 < 9){
            num3 = num3 + 1;
          }
          break;
        case 4:
          if (num4 < 9){
            num4 = num4 + 1;
          }
          break;
      }
    }
    if (digitalRead(btnDown) == LOW){
      switch (selectedNumber){
        case 1:
          if (num1 > 0){
            num1 = num1 - 1;
          }
          break;
        case 2:
          if (num2 > 0){
            num2 = num2 - 1;
          }
          break;
        case 3:
          if (num3 > 0){
            num3 = num3 - 1;
          }
          break;
        case 4:
          if (num4 > 0){
            num4 = num4 - 1;
          }
          break;
      }
    }
    delay(250);
  }
  
  // Zero Clear
  if (systemStatus == 3){
    LCD.clear();
    LCD.setCursor(0,0);
    LCD.print(" Zero Error CRN ");
    LCD.setCursor(0,1);
    LCD.print("Analyzing :  0%");
    zeroError = 0;
    for (int i=1; i <= 5; i++){
      zeroError += calculateCurrent();
      LCD.setCursor(12,1);
      LCD.print(i * 20);
      delay(20);
    }
    systemStatus = 4;
    LCD.clear();
    zeroError = zeroError / 5;
  }
  
  // Confirm Zero Error
  if (systemStatus == 4){
    LCD.setCursor(0,0);
    LCD.print("Error : " + String(zeroError) + " A");
    LCD.setCursor(0,1);
    LCD.print("Confirm?        ");
    if (digitalRead(btnRight) == LOW){
      systemStatus = 3;
    }
    if (digitalRead(btnLeft) == LOW){
      zeroError = 0;
      systemStatus = 5;
    }
    // Start
    if (digitalRead(btnOk) == LOW){
      systemStatus = 5;
    }
  }
  
  // Turn On Relay
  if (systemStatus == 5){
    digitalWrite(relayPin, HIGH);
    LCD.clear();
    printLimit();
    systemStatus = 6;
  }
  
  // Processing
  if (systemStatus == 6){
    float current = calculateCurrent();
    acCurrent = current - zeroError;
    LCD.setCursor(8,0);
    if (acCurrent < 0){
      LCD.print(00.00);
    }else{
      LCD.print(acCurrent); 
    }
    if (digitalRead(btnLeft) == HIGH){
      if (acCurrent >= allowedMax){
        // Stop Current Flow
        digitalWrite(relayPin, LOW);
        digitalWrite(buzzerPin, LOW);
        systemStatus = 7;
        LCD.setCursor(0, 1);
        LCD.print(" Limit Reached! ");
      }
    }
  }
  
  // Reset System Status
  if (systemStatus == 7){
    if (digitalRead(btnOk) == LOW){
      allowedMax = 0;
      systemStatus = 0;
      digitalWrite(buzzerPin, HIGH);
    }
  }
}

void printDefault(){
  LCD.setCursor(0, 1);
  LCD.print(num1);
  LCD.setCursor(1, 1);
  LCD.print(num2);
  LCD.setCursor(3, 1);
  LCD.print(num3);
  LCD.setCursor(4, 1);
  LCD.print(num4);
  LCD.print(" A");
}

void printLimit(){
  LCD.setCursor(0, 0);
  LCD.print("Rec   :        A");
  LCD.setCursor(0, 1);
  LCD.print("Limit : ");
  LCD.print(num1);
  LCD.print(num2);
  LCD.print(".");
  LCD.print(num3);
  LCD.print(num4);
  LCD.print("  A");
}

// Smplaing Algorithm for Sensor Readings
float getVppSample(){
  float result;
  int readValue;
  int maxValue = 0;
  int minValue = 1024;
  uint32_t start_time = millis();
  while((millis()-start_time) < 500){
    readValue = analogRead(currentPin);
    if (readValue > maxValue) {
      maxValue = readValue;
    }
    if (readValue < minValue){
      minValue = readValue;
    }
  }
  result = ((maxValue - minValue) * 5.0)/1024.0;
  return result;
}

float calculateCurrent(){
  float voltage = getVppSample();
  float vRms = (voltage/2.0) * 0.707;
  float ampRms = (vRms * 1000) / 100; // Sensitivity Of The Sensor
  return ampRms;
}

