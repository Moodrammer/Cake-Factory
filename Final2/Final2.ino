#define mixerAp 0
#define mixerBp 1
#define eggValvePin 2
#define vanillaValvePin 3
#define sugarValvePin 4
#define flourValvePin 5

//Time it takes to go from 0 to 180 as its speed is 100ms / 60 degrees
#define servoOpeningTime 300

//time Management
unsigned long currentMillis;
unsigned long openingTime;
unsigned long waitTime;
//mixer motor variables
int mixerCurrentStep = 0;
boolean isMixerOn = false;
//valves control
boolean isEggsAdded = false;
boolean isEggValveOpened = false;
boolean isEggValveClosed = true; 

void setup() {
  for(int i = 0; i <= 5; i++) pinMode(i, OUTPUT);
  //close all the valves
  closeValve(eggValvePin);
  closeValve(vanillaValvePin);
  closeValve(sugarValvePin);
  closeValve(flourValvePin);
}

void loop() {
  currentMillis = millis();
  if(!isEggsAdded){
    waitTime = servoOpeningTime + 500; 
    addEggs();
  }
  if(isMixerOn){
    moveMotor(mixerAp, mixerBp, mixerCurrentStep, 1);
    mixerCurrentStep ++; 
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
    }
  }
}
