//I assumed in baking phase that the oven led turns off after one second

#define mixerAp 0
#define mixerBp 1
#define eggValvePin 2
#define vanillaValvePin 3
#define sugarValvePin 4
#define flourValvePin 5
#define ovenLedPin 6
#define productionAp 7
#define productionBp 8
#define cakeStandAp 9
#define cakeStandBp 10
#define rasberryValvePin 11
#define pineappleValvePin 12
#define standArm 14
#define keypadPin 15

#define stepsPerRevolution 64

//The five tracks are ordered from the outer circle to the inner one
#define track1Angle 0
#define track2Angle 20
#define track3Angle 40
#define track4Angle 60
#define track5Angle 80

//Time it takes to go from 0 to 180 as its speed is 100ms / 60 degrees
#define servoOpeningTime 300

//time Management
unsigned long currentMillis;
unsigned long openingTime;
unsigned long waitTime;
//phase control
boolean makingPhase = true;
boolean bakingPhase = false;
boolean decorationPhase = false;

int motorStepCount = 0;
//mixer motor variables
int mixerCurrentStep = 0;
int numRotations = 0;
int mixerDelay = 1562;
int currentRotations;
boolean isMixerOn = false;
boolean countRotations = false;
//Production line variables
int productionCurrentStep = 0;

//valves control
boolean isEggsAdded = false;
boolean isEggValveOpened = false;
boolean isEggValveClosed = true; 

boolean isVanillaAdded = false;
boolean isVanillaValveOpened = false;
boolean isVanillaValveClosed = true;

boolean isSugarAdded = false;
boolean isSugarValveOpened = false;
boolean isSugarValveClosed = true;

int addingFlourTimes = 0;
boolean isFlourAdded = false;
boolean addFlourTime = false;
boolean isFlourValveOpened = false;
boolean isFlourValveClosed = true;
boolean isFlourAddingStarted = false;

//Baking Phase variables
boolean isHeatingStarted = false;
boolean isBeforeBaking = true;

//decorationPhaseVaribles
boolean isKeyRead = false;

void setup() {
  for(int i = 0; i <= 14; i++) pinMode(i, OUTPUT);
  //close all the valves
  closeValve(eggValvePin);
  closeValve(vanillaValvePin);
  closeValve(sugarValvePin);
  closeValve(flourValvePin);
  moveArmToAngle(0);
  delay(500);
}

void loop() {
  currentMillis = millis();
  //Making Phase
  if(makingPhase){
    if(isMixerOn){
      moveMotor(mixerAp, mixerBp, mixerCurrentStep, mixerDelay);
      mixerCurrentStep ++; 
    }
    
    if(countRotations){
      motorStepCount++;
      if(motorStepCount == stepsPerRevolution){
        numRotations++;
        motorStepCount = 0;  
      }
    }
    
    
    if(!isEggsAdded){
      waitTime = servoOpeningTime + 500; 
      addEggs();
    }
  
    if(numRotations == 5 && !isVanillaAdded){
      countRotations = false;
      waitTime = servoOpeningTime + 100;
      addVanilla();
    }
  
    if(isVanillaAdded && !isSugarAdded){
      if(numRotations - currentRotations == 4){
        countRotations = false;
        isMixerOn = false;
        waitTime = servoOpeningTime + 200;
        addSugar();
      }
    }
  
    if(isSugarAdded && !isFlourAdded){
      if(numRotations - currentRotations == 10){
          countRotations = false;
          isMixerOn = false;
          //reduce the motor Speed
          mixerDelay *= 2;
          currentRotations = numRotations;
          addFlourTime = true;
          addingFlourTimes++;
          isFlourAddingStarted = true;
      }
  
      if(addFlourTime){
        waitTime = servoOpeningTime + 100;
        addFlour();
      }
      if(numRotations - currentRotations == 4 && isFlourAddingStarted){
        isMixerOn = false;
        countRotations = false;
        currentRotations = numRotations;
        if(addingFlourTimes != 3){
          addingFlourTimes++;
          addFlourTime = true;
          isFlourValveOpened = false;
          isFlourValveClosed = true;
        }
        else{
          isFlourAdded = true;
          makingPhase = false;
          bakingPhase = true;
          motorStepCount = 0;
        }
      }
    }
  }
  if(bakingPhase){
    if(isBeforeBaking){    
      if(motorStepCount <= stepsPerRevolution){
        moveMotor(productionAp, productionBp, productionCurrentStep, 15620);
        productionCurrentStep ++;
        motorStepCount++;
      }
      else{
        //Motor moved one full rotation
        if(!isHeatingStarted){
          openingTime = currentMillis;
          digitalWrite(ovenLedPin, HIGH);
          isHeatingStarted = true;
        }
        if(currentMillis - openingTime >= 1000){
          digitalWrite(ovenLedPin, LOW);
          motorStepCount = 0;
          isBeforeBaking = false;
        }
      }
    }
    else{
        if(motorStepCount <= stepsPerRevolution){
          moveMotor(productionAp, productionBp, productionCurrentStep, 15620);
          productionCurrentStep ++;
          motorStepCount++;
        }
        else{
          bakingPhase = false;
          decorationPhase = true; 
          delay(1000);     
        }
    }
  }
  if(decorationPhase){
     if(analogRead(keypadPin)){
        if(!isKeyRead)
          chooseDecoration();
     }
     else{
      isKeyRead = false;
     }
  }
}

//Move the stepper motor
void moveMotor(int Ap,int Bp, int currentStep, int delayMicros){
  currentStep %= 4;
  delayMicroseconds(delayMicros);
  switch(currentStep){
    case 0:
    digitalWrite(Ap, 0);
    digitalWrite(Bp, 0);
    break;
  
    case 1:
    digitalWrite(Ap, 0);
    digitalWrite(Bp, 1);
    break;


    case 2:
    digitalWrite(Ap, 1);
    digitalWrite(Bp, 1);
    break;
  
    case 3:
    digitalWrite(Ap, 1);
    digitalWrite(Bp, 0);  
   }
}

//Move the Stepper Motor for a certain number of steps
void moveMotorForSteps(int Ap,int Bp, int numSteps, int delayMicros){
  int currentStep = -1;
  while(numSteps != 0){
    currentStep = (currentStep + 1) % 4;
    delayMicroseconds(delayMicros);
    switch(currentStep){
      case 0:
      digitalWrite(Ap, 0);
      digitalWrite(Bp, 0);
      break;
    
      case 1:
      digitalWrite(Ap, 0);
      digitalWrite(Bp, 1);
      break;

      
      case 2:
      digitalWrite(Ap, 1);
      digitalWrite(Bp, 1);
      break;
    
      case 3:
      digitalWrite(Ap, 1);
      digitalWrite(Bp, 0);    
     }
     numSteps --;
  }  
}

void openValve(int valvePin){
  digitalWrite(valvePin, HIGH);
  delayMicroseconds(2000);
  digitalWrite(valvePin, LOW);
}

void closeValve(int valvePin){
  digitalWrite(valvePin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(valvePin, LOW);
}

void moveArmToAngle(int angle){
  int pulseDuration = 1000 + (angle/180.0) * 1000;
  digitalWrite(standArm, HIGH);
  delayMicroseconds(pulseDuration);
  digitalWrite(standArm, LOW);
  delayMicroseconds(20000 - pulseDuration);
}

void addEggs(){
  if(!isEggValveOpened){
    openValve(eggValvePin);
    isEggValveOpened = true;
    isEggValveClosed = false;
    openingTime = currentMillis;
  }
  else if(!isEggValveClosed){
    if(currentMillis - openingTime >= waitTime){
      closeValve(eggValvePin);
      isEggValveClosed = true;
      isEggsAdded = true;
      isMixerOn = true;
      countRotations = true;
      motorStepCount = 0;
    }
  }
}

void addVanilla(){
  if(!isVanillaValveOpened){
    openValve(vanillaValvePin);
    isVanillaValveOpened = true;
    isVanillaValveClosed = false;
    openingTime = currentMillis;
  }
  else if(!isVanillaValveClosed){
    if(currentMillis - openingTime >= waitTime){
      if(currentMillis - openingTime >= servoOpeningTime){
        countRotations = true;
        motorStepCount = 0;
        currentRotations = numRotations;
      }
      closeValve(vanillaValvePin);
      isVanillaValveClosed = true;
      isVanillaAdded = true;
    }
  }
}

void addSugar(){
  if(!isSugarValveOpened){
    openValve(sugarValvePin);
    isSugarValveOpened = true;
    isSugarValveClosed = false;
    openingTime = currentMillis;
  }
  else if(!isSugarValveClosed){
    if(currentMillis - openingTime >= waitTime){
      closeValve(sugarValvePin);
      isSugarValveClosed = true;
      isSugarAdded = true;
      isMixerOn = true;
      countRotations = true;
      motorStepCount = 0;
      currentRotations = numRotations;
    }
  }
}

void addFlour(){
  if(!isFlourValveOpened){
    openValve(flourValvePin);
    isFlourValveOpened = true;
    isFlourValveClosed = false;
    openingTime = currentMillis;
  }
  else if(!isFlourValveClosed){
    if(currentMillis - openingTime >= waitTime){
      closeValve(flourValvePin);
      isFlourValveClosed = true;
      isMixerOn = true;
      countRotations = true;
      motorStepCount = 0;
      addFlourTime = false;
    }
  }
}

void chooseDecoration(){
  int reading = analogRead(keypadPin);
  switch(reading){
    case 852:
      isKeyRead = true;
      decorateOne();
      break;
    case 786:
      isKeyRead = true;
      decorateTwo();
      break;
    case 730:
      isKeyRead = true;
      decorateThree();
      break;
    case 681:
      isKeyRead = true;
      decorateFour();
      break;    
  }
}

void decorateOne(){
  //move arm to the outer track (track 1)
  moveArmToAngle(track1Angle);
  //Assume settlement Time for arm
  delay(1000);
  digitalWrite(rasberryValvePin, HIGH);
  moveMotorForSteps(cakeStandAp, cakeStandBp, stepsPerRevolution, 15620);
  digitalWrite(rasberryValvePin, LOW);

  //move arm to track 3
  moveArmToAngle(track3Angle);
  delay(1000);
  digitalWrite(pineappleValvePin, HIGH);
  moveMotorForSteps(cakeStandAp, cakeStandBp, stepsPerRevolution, 15620);
  digitalWrite(pineappleValvePin, LOW);
}

void decorateTwo(){
  for(int i = 1; i <= 4; i++){
    moveArmToAngle(track1Angle);
    delay(1000);
    digitalWrite(rasberryValvePin, HIGH);
    moveMotorForSteps(cakeStandAp, cakeStandBp, stepsPerRevolution / 8, 15620);
    digitalWrite(rasberryValvePin, LOW);
    delay(1000);

    moveArmToAngle(track3Angle);
    delay(1000);
    digitalWrite(pineappleValvePin, HIGH);
    moveMotorForSteps(cakeStandAp, cakeStandBp, stepsPerRevolution / 8, 15620);
    digitalWrite(pineappleValvePin, LOW);
    delay(1000);
  }
}

void decorateThree(){
  //pineApple Circle
  moveArmToAngle(track5Angle);
  delay(1000);
  digitalWrite(pineappleValvePin, HIGH);
  delay(1000);
  digitalWrite(pineappleValvePin, LOW);

  //add 16 circles of rasberry
  moveArmToAngle(track1Angle);
  delay(1000);
  for(int i = 1; i <= 16; i++){
    digitalWrite(rasberryValvePin, HIGH);
    delay(1000);
    digitalWrite(rasberryValvePin, LOW);
    moveMotorForSteps(cakeStandAp, cakeStandBp, stepsPerRevolution / 16, 15620);
    delay(1000);
  }
}

void decorateFour(){
  //large middle rasberry Circle
  moveArmToAngle(track5Angle);
  delay(1000);
  digitalWrite(rasberryValvePin, HIGH);
  delay(1000);
  digitalWrite(rasberryValvePin, LOW);

  moveArmToAngle(track4Angle);
  delay(1000);
  digitalWrite(rasberryValvePin, HIGH);
  moveMotorForSteps(cakeStandAp, cakeStandBp, stepsPerRevolution, 15620);
  digitalWrite(rasberryValvePin, LOW);

  //alternating rasberry and pineapple circles assume they are 16
  //assume the even ones are rasberry and the odd ones are pineapple
  moveArmToAngle(track1Angle);
  delay(1000);
  for(int i = 1; i <= 16; i++){
    if(i % 2 == 0){
      digitalWrite(rasberryValvePin, HIGH);
      delay(1000);
      digitalWrite(rasberryValvePin, LOW);
    }
    else{
      digitalWrite(pineappleValvePin, HIGH);
      delay(1000);
      digitalWrite(pineappleValvePin, LOW);
      
    }
    moveMotorForSteps(cakeStandAp, cakeStandBp, stepsPerRevolution / 16, 15620);
    delay(1000);
  }  
}
