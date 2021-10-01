#include <AccelStepper.h>

#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
bool flag = true;

void go_home() {
  while (analogRead(A0) < 1000) {
    stepper.move(50);
    stepper.runToPosition();
    stepper.setCurrentPosition(0);
    Serial.println(analogRead(A0));
  }
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial);

  stepper.setAcceleration(6400);
  stepper.setMaxSpeed(6400);
}

void loop() {

  if (Serial.available() > 0) {

    String str = Serial.readString();

    if (str.startsWith("is")) {
      Serial.print("sensor val: ");
      Serial.println(analogRead(A0));
    }
    else if (str.startsWith("home")) {
      go_home();
      Serial.println("done");
    }
    else if (str.startsWith("ma")) {
      flag = true;
      long int pos = str.substring(2).toInt();
      stepper.moveTo(pos);
//      stepper.runToPosition();
      Serial.println("done");
    }
    else if (str.startsWith("mr")) {
      flag = true;
      long int pos = str.substring(2).toInt();
      stepper.move(pos);
//      stepper.runToPosition();
      Serial.println("done");
    }
    else if(str.startsWith("stop")){
      flag = false;
      stepper.stop();
    }
    else if(str.startsWith("steps")){
      Serial.print("steps_left: ");
      Serial.println(stepper.distanceToGo());
    }
  }
  if( flag &&  stepper.distanceToGo() != 0){
      stepper.run();
      Serial.print("s");
      Serial.println(stepper.distanceToGo());
    }
}
