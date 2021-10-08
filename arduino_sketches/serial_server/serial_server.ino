#include <AccelStepper.h>
#include <Servo.h>
#include <EEPROM.h>

#define mot1_dirPin 2
#define mot1_stepPin 3

#define mot2_dirPin 4
#define mot2_stepPin 5

#define motorInterfaceType 1
#define servo_pin 9

Servo servo;   // nd_filter servo: motor ms90s

AccelStepper stepper1 = AccelStepper(motorInterfaceType, mot1_stepPin, mot1_dirPin);
AccelStepper stepper2 = AccelStepper(motorInterfaceType, mot2_stepPin, mot2_dirPin);

bool flag = true;

long int step1_pos = 0;
int step1_loc = 0;
long int step1_total_steps = 3200;

//  { 0, 1, 2, 3, 4 } = { unknown, A, B, C, D}
int step1_filter;   
int step1_filter_loc = 10;

long int step2_pos = 0;
int step2_loc = 20;
long int step2_total_steps = 3200;

//  { 0, 1, 2, 3, 4 } = { unknown, A, B, C, D}
int step2_filter;
int step2_filter_loc = 30;

void home_stepper1() {
  while (analogRead(A1) < 1000) {
    stepper1.move(50);
    stepper1.runToPosition();
//    Serial.println(analogRead(A1));
  }
  stepper1.setCurrentPosition(0);
  save_motos_pos();
}

void home_stepper2() {
  while (analogRead(A2) < 1000) {
    stepper2.move(50);
    stepper2.runToPosition();
//    Serial.println(analogRead(A2));
  }
  stepper2.setCurrentPosition(0);
  save_motos_pos();
}

void save_filters_pos(){
    EEPROM.put(step1_filter_loc, step1_filter);
    EEPROM.put(step2_filter_loc, step2_filter);
}

void save_motos_pos(){
   EEPROM.put(step1_loc, stepper1.currentPosition());
   EEPROM.put(step2_loc, stepper2.currentPosition()); 
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial);

  servo.attach(servo_pin);
  // servo.write(0);
  
  stepper1.setAcceleration(6400);
  stepper1.setMaxSpeed(6400);

  stepper2.setAcceleration(6400);
  stepper2.setMaxSpeed(6400);
  // Check if EEPROM is used first time
  if(EEPROM.read(1023) != 123){
    // Serial.println("EEPROM not written!");
    EEPROM.write(1023, 123);
    EEPROM.put(step1_loc, 0);
    EEPROM.put(step2_loc, 0);
    EEPROM.put(step1_filter_loc, 0);
    EEPROM.put(step2_filter_loc, 0);

  }
  else{
    // Serial.println("EEPROM settings found :)");
    EEPROM.get(step1_loc, step1_pos);
    EEPROM.get(step2_loc, step2_pos);
    EEPROM.get(step1_filter_loc, step1_filter);
    EEPROM.get(step2_filter_loc, step2_filter);
    stepper1.setCurrentPosition(step1_pos);
    stepper2.setCurrentPosition(step2_pos);
    // Serial.print("step1_pos: "); Serial.println(step1_pos);
    // Serial.print("step2_pos: "); Serial.println(step2_pos);
    // Serial.print("step1_filter: "); Serial.println(step1_filter);
    // Serial.print("step2_filter: "); Serial.println(step2_filter);

  }
}

void loop() { 

  if (Serial.available() > 0) {

    String str = Serial.readString();

    if (str.startsWith("is")) {
      Serial.print("sensor1: ");
      Serial.println(analogRead(A0));
      Serial.print("sensor2: ");
      Serial.println(analogRead(A1));
    }
    else if (str.startsWith("filters")) {
      // Serial.print("Vars: ");
      Serial.print(step1_filter);
      Serial.println(step2_filter);
      // EEPROM.get(step1_filter, step1_filter_loc);
      // EEPROM.get(step2_filter, step2_filter_loc);
      // Serial.print("EEPROM: ");
      // Serial.print(step1_filter);
      // Serial.println(step2_filter);
    }
    else if (str.startsWith("s1")) {
      int filter = str.substring(2).toInt();
      if (filter == 1){
        stepper1.moveTo(step1_total_steps/4);
        stepper1.runToPosition();
        step1_filter = 1;
        Serial.println("done");
      }
      else if (filter == 2){
        stepper1.moveTo((step1_total_steps/4)*2);
        stepper1.runToPosition();
        step1_filter = 2;
        Serial.println("done");
      }
      else if (filter == 3){
        stepper1.moveTo((step1_total_steps/4)*3);
        stepper1.runToPosition();
        step1_filter = 3;
        Serial.println("done");
      }
      else if (filter == 4){
        stepper1.moveTo(0);
        stepper1.runToPosition();
        step1_filter = 4;
        Serial.println("done");
      }
      else{
        Serial.println("invalid cmd!");
      }
      save_filters_pos();
      save_motos_pos();
      
    }
    else if (str.startsWith("s2")) {
      int filter = str.substring(2).toInt();
      if (filter == 1){
        stepper2.moveTo(step2_total_steps/4);
        stepper2.runToPosition();
        step2_filter = 1;
        Serial.println("done");
      }
      else if (filter == 2){
        stepper2.moveTo((step2_total_steps/4)*2);
        stepper2.runToPosition();
        step2_filter = 2;
        Serial.println("done");
      }
      else if (filter == 3){
        stepper2.moveTo((step2_total_steps/4)*3);
        stepper2.runToPosition();
        step2_filter = 3;
        Serial.println("done");
      }
      else if (filter == 4){
        stepper2.moveTo(0);
        stepper2.runToPosition();
        step2_filter = 4;
        Serial.println("done");
      }
      else{
        Serial.println("invalid cmd!");
      }
      save_filters_pos();
      save_motos_pos();
      
    }
    else if (str.startsWith("ser")) {
      int angle = str.substring(3).toInt();
      servo.write(angle);
      Serial.println("done");
    }
    else if (str.startsWith("homestep1")) {
      home_stepper1();
      Serial.println("done");
    }
    else if (str.startsWith("homestep2")) {
      home_stepper2();
      Serial.println("done");
    }
    else if (str.startsWith("mas1_")) {
      flag = true;
      long int pos = str.substring(5).toInt();
      stepper1.moveTo(pos);
      stepper1.runToPosition();
      save_motos_pos();
      Serial.println("done");
    }
    else if (str.startsWith("mrs1_")) {
      flag = true;
      long int pos = str.substring(5).toInt();
      stepper1.move(pos);
      stepper1.runToPosition();
      save_motos_pos();
      Serial.println("done");
    }
    else if (str.startsWith("mas2_")) {
      flag = true;
      long int pos = str.substring(5).toInt();
      stepper2.moveTo(pos);
      stepper2.runToPosition();
      save_motos_pos();
      Serial.println("done");
    }
    else if (str.startsWith("mrs2_")) {
      flag = true;
      long int pos = str.substring(5).toInt();
      stepper2.move(pos);
      stepper2.runToPosition();
      save_motos_pos();
      Serial.println("done");
    }
    else if(str.startsWith("stop")){
      flag = false;
      stepper1.stop();
      stepper2.stop();
    }
    else if(str.startsWith("readmem")){
        EEPROM.get(step1_loc, step1_pos);
        EEPROM.get(step2_loc, step2_pos);
        Serial.print("stepper1: ");
        Serial.println(step1_pos);
        Serial.print("stepper2: ");
        Serial.println(step2_pos);
    }
    else if(str.startsWith("readmotors")){
        Serial.print("stepper1: ");
        Serial.println(stepper1.currentPosition());
        Serial.print("stepper2: ");
        Serial.println(stepper2.currentPosition());
    }
    else if(str.startsWith("loop")){
      while(analogRead(A1)<1000 && analogRead(A2)<1000 ){
        stepper1.move(500);
        stepper2.move(500);
        while(stepper1.distanceToGo()>0 && stepper2.distanceToGo()>0){
          stepper1.run(); stepper2.run();
        }
      }
    }
    else if(str.startsWith("clear")){
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
      }
      Serial.println("EEPROM cleared!");
    }
    else{
      Serial.print("invalid cmd! : ");
      Serial.println(str);
    }
  }
//  if( flag &&  stepper1.distanceToGo() != 0){
//      stepper1.run();
//      Serial.print("s");
//      Serial.println(stepper1.distanceToGo());
//    }
}
