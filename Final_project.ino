
#include <IRremote.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#define LCD_RS A5
#define LCD_E A4
#define LCD_D4 6
#define LCD_D5 7
#define LCD_D6 8
#define LCD_D7 9
LiquidCrystal lcd(LCD_RS,LCD_E,LCD_D4,LCD_D5,LCD_D6,LCD_D7);

#define IR_RECEIVE 5
#define IR_PLAY 5
#define IR_OFF 0
#define IR_EQ 13
#define IR_UP 10
#define IR_DOWN 8

#define WARNING_LED 11
unsigned long lastTimeWarning=millis();
unsigned long warningDelay=500;
byte warningLEDState= LOW;

#define ECHO 3
#define TRIGGER 2
unsigned long prevTime=millis();
unsigned long serialDelay=100;
volatile unsigned long pulseInBegin;
volatile unsigned long pulseInEnd;
volatile bool newDistanceAvailable= false;
double prevDistance=400;

#define LOCK_DISTANCE 50.0
#define ERROR_LED 12
bool isLocked=false;
unsigned long lastTimeError=millis();
unsigned long errorDelay=500;
byte errorLEDState= LOW;

#define PUSH_BUTTON 10
unsigned long lastTimeButtonChanged=millis();
unsigned long buttonDebounceDelay=50;
byte buttonState=LOW;

#define UNIT_CM 0
#define UNIT_INCH 1
int distanceUnit=UNIT_CM;
#define EEPROM_ADDRESS 50

#define MODE_DISTANCE 0
#define MODE_SETTINGS 1
int lcdMode=MODE_DISTANCE;

/*#define PHOTORESISTOR A0
#define LIGHT 13
*/
void triggerUS(){
  digitalWrite(TRIGGER,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER,LOW);
}

double getUD(){
  double durationMicros= pulseInEnd-pulseInBegin;
  double distance= durationMicros/58.0; //cm (148.0: inches)

if (distance>400.0){
  return prevDistance;
}
prevDistance=distance;
distance= prevDistance*0.7 + distance*0.3;
return distance;
}

void echoInterrupt(){
  if (digitalRead(ECHO)==HIGH){
    pulseInBegin=micros();
  }
  else{
    pulseInEnd=micros();
    newDistanceAvailable=true;
  }
}
void setWarningLEDBlinkRateFromDistance(double distance){
  warningDelay= distance*4; //0..400 cm => 0..1600 ms
}

void lock(){
  if (!isLocked){
    isLocked= true;
    //they should toggle at same time
    warningLEDState=LOW;
    errorLEDState=LOW;
  }
}

void unlock(){
  if (isLocked){
    isLocked=false;
    errorLEDState=LOW;
    digitalWrite(ERROR_LED,errorLEDState);
  }
}

void printLCDDistance(double distance){
  if (isLocked){
    lcd.setCursor(0,0);
    lcd.print("Appl. locked    ");
    lcd.setCursor(0,1);
    lcd.print("Press to unlock ");
  }
  else if (lcdMode== MODE_DISTANCE){
    lcd.setCursor(0,0);
    lcd.print("Dist");
    if (distanceUnit==UNIT_INCH){
      lcd.print(distance*0.393701);
      lcd.print("inch  ");
    }
    else{
      lcd.print(distance);
      lcd.print("cm");
    }
    
    lcd.setCursor(0,1);
    if (distance>100.0){
      lcd.print("no obstacle");
    }
    else{
      lcd.print("warning");
    }
  }
  else if (lcdMode=MODE_SETTINGS){
    lcd.setCursor(0,0);
    lcd.print("Press OFF       ");
    lcd.setCursor(0,1);
    lcd.print("to reset setting");
  }
}

void handleIR(long command){
  switch (command){
    case IR_PLAY:{
      unlock();
      lcd.clear();
      break;
    }
    case IR_OFF:{
      resetSetting();
      break;
    }
    case IR_EQ:{
      toggleDistanceUnit();
      break;
    }
    case IR_UP:{
      togglelcd();
      break;
    }
    case IR_DOWN:{
      togglelcd();
      break;
    }
    default:{
      //meow
    }
  }
}

void toggleDistanceUnit(){
  if (distanceUnit==UNIT_CM){
    distanceUnit=UNIT_INCH;
  }
  else{
    distanceUnit=UNIT_CM;
  }
  EEPROM.write(EEPROM_ADDRESS, distanceUnit);
}

void togglelcd(){
  switch(lcdMode){
    case MODE_DISTANCE: {
      lcdMode= MODE_SETTINGS;
      break;
    }
    case MODE_SETTINGS: {
      lcdMode=MODE_DISTANCE;
      break;
    }
    default: {
      lcdMode=MODE_DISTANCE;
    }
  }
}

void resetSetting(){
  if (lcdMode ==MODE_SETTINGS){
    distanceUnit=UNIT_CM;
    EEPROM.write(EEPROM_ADDRESS,distanceUnit);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Settings have   ");
    lcd.setCursor(0,1);
    lcd.print(("been reset      "));
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ECHO,INPUT);
  pinMode(TRIGGER,OUTPUT);
  pinMode(WARNING_LED, OUTPUT);
  pinMode(ERROR_LED,OUTPUT);
  pinMode(PUSH_BUTTON,INPUT);
  //pinMode(LED1, OUTPUT);
  //pinMode(LED2, OUTPUT);
  //pinMode(LED3, OUTPUT);

  buttonState=digitalRead(PUSH_BUTTON);
  attachInterrupt(digitalPinToInterrupt(ECHO),echoInterrupt,CHANGE);

  lcd.begin(16,2);
  lcd.print("Meow");
  IrReceiver.begin(IR_RECEIVE);

  distanceUnit=EEPROM.read(EEPROM_ADDRESS);
  if (distanceUnit==255){
    distanceUnit=UNIT_CM;
  }

}

void loop() {
  unsigned long timeNow=millis();
  
  if (timeNow-prevTime>serialDelay){
    prevTime=timeNow;
    triggerUS();
  }
  
  if (newDistanceAvailable){
    double distance= getUD();
    newDistanceAvailable=false;

    if(distance<LOCK_DISTANCE){
      lock();
    }
    setWarningLEDBlinkRateFromDistance(distance);
    printLCDDistance(distance);
    Serial.println(distance);
  }
  
  if (isLocked){
    if (timeNow-lastTimeError>errorDelay){
      lastTimeError=timeNow;
      //blinking
      errorLEDState=(errorLEDState == HIGH) ? LOW : HIGH;
      digitalWrite(ERROR_LED, errorLEDState);
      warningLEDState=(warningLEDState == HIGH) ? LOW : HIGH;
      digitalWrite(WARNING_LED, warningLEDState);
    }

    //when to unlock (also debounce)
    if (timeNow-lastTimeButtonChanged>buttonDebounceDelay){
      byte newButtonState=digitalRead(PUSH_BUTTON);
      if (newButtonState!=buttonState){
        lastTimeButtonChanged=timeNow;
        buttonState=newButtonState;
        if (buttonState==LOW){
          unlock();
        }
      }
    }
  }
  else if (timeNow-lastTimeWarning>warningDelay){
      lastTimeWarning=timeNow;
      //blinking
      warningLEDState=(warningLEDState == HIGH) ? LOW : HIGH;
      digitalWrite(WARNING_LED, warningLEDState);
  }
  if (IrReceiver.decode()){
    IrReceiver.resume();
    long command=IrReceiver.decodedIRData.command;
    handleIR(command);
  }  
}