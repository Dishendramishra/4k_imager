#include <AccelStepper.h>

#define mot1_dirPin 2
#define mot1_stepPin 3

#define mot2_dirPin 4
#define mot2_stepPin 5

#define motorInterfaceType 1

AccelStepper stepper1 = AccelStepper(motorInterfaceType, mot1_stepPin, mot1_dirPin);
AccelStepper stepper2 = AccelStepper(motorInterfaceType, mot2_stepPin, mot2_dirPin);

bool flag = true;

void go_home() {
  while (analogRead(A0) < 1000) {
    stepper1.move(50);
    stepper1.runToPosition();
    stepper1.setCurrentPosition(0);
    Serial.println(analogRead(A0));
  }
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial);

  stepper1.setAcceleration(6400);
  stepper1.setMaxSpeed(6400);
}

void loop() { 

  if (Serial.available() > 0) {

    String str = Serial.readString();

    if (str.startsWith("is")) {
      Serial.print("sensor val: ");
      Serial.println(analogRead(A0));
    }
    else if (str.startsWith("ser")) {
      int angle = str.substring(3).toInt();
      Serial.println("done");
    }
    else if (str.startsWith("home")) {
      go_home();
      Serial.println("done");
    }
    else if (str.startsWith("maf1_")) {
      flag = true;
      long int pos = str.substring(5).toInt();
      stepper1.moveTo(pos);
      stepper1.runToPosition();
      Serial.println("done");
    }
    else if (str.startsWith("mrf1_")) {
      flag = true;
      long int pos = str.substring(5).toInt();
      stepper1.move(pos);
      stepper1.runToPosition();
      Serial.println("done");
    }
    else if (str.startsWith("maf2_")) {
      flag = true;
      long int pos = str.substring(5).toInt();
      stepper2.moveTo(pos);
      stepper2.runToPosition();
      Serial.println("done");
    }
    else if (str.startsWith("mrf2_")) {
      flag = true;
      long int pos = str.substring(5).toInt();
      stepper2.move(pos);
      stepper2.runToPosition();
      Serial.println("done");
    }
    else if(str.startsWith("stop")){
      flag = false;
      stepper1.stop();
      stepper2.stop();
    }
    // else if(str.startsWith("steps")){
    //   Serial.print("steps_left: ");
    //   Serial.println(stepper1.distanceToGo());
    // }
    else{
      Serial.println("invalid cmd!");
    }
  }
//  if( flag &&  stepper1.distanceToGo() != 0){
//      stepper1.run();
//      Serial.print("s");
//      Serial.println(stepper1.distanceToGo());
//    }
}
