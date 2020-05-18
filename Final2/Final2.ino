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

#define stepsPerRevolution 100

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
int mixerDelay = 1;
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

void setup() {
  for(int i = 0; i <= 8; i++) pinMode(i, OUTPUT);
  //close all the valves
  closeValve(eggValvePin);
  closeValve(vanillaValvePin);
  closeValve(sugarValvePin);
  closeValve(flourValvePin);
  delay(500);
}

void loop() {
  currentMillis = millis();
  //Making Phase
  if(makingPhase){
    if(countRotations){
      motorStepCount++;
      if(motorStepCount == stepsPerRevolution){
        numRotations++;
        motorStepCount = 0;  
      }
    }
    
    if(isMixerOn){
      moveMotor(mixerAp, mixerBp, mixerCurrentStep, mixerDelay);
      mixerCurrentStep ++; 
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
          mixerDelay = 2;
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
      if(motorStepCount <= 100){
        moveMotor(productionAp, productionBp, productionCurrentStep, 10);
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
        if(motorStepCount <= 100){
          moveMotor(productionAp, productionBp, productionCurrentStep, 10);
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
    
  }
}

//Move the stepper motor
void moveMotor(int Ap,int Bp, int currentStep, int delayMillis){
  currentStep %= 4;
  delay(delayMillis);
  switch(currentStep){
    case 0:
    digitalWrite(Ap, 1);
    digitalWrite(Bp, 1);
    break;
  
    case 1:
    digitalWrite(Ap, 1);
    digitalWrite(Bp, 0);
    break;
  
    case 2:
    digitalWrite(Ap, 0);
    digitalWrite(Bp, 0);
    break;
  
    case 3:
    digitalWrite(Ap, 0);
    digitalWrite(Bp, 1);
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
