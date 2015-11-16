#include <TimerOne.h>

// 20 bits après le virgule, à dimenssionner suivant la vitesse
// Plus la vitesse est faible, plus STEP_SHIFT doit être petit
// Et inversement.
// Ex: pour une freq de 10kHz, 24 est une bonne valeur,
// Pour une freq de 1.7 kHz, 20 semble bon
#define INITFREQ (90)
#define STEP_SHIFT (20)

const int freq = 10369;

void setup(void)
{
  Timer1.initialize(INITFREQ); // Call interupts each 104µs
  
  // 1 full tr in 86164 seconds witch is demultiplied by 6^7
  // it requires the motor to do 3.248874 tr/s
  // 3.24887423982 tr/s x 200 steps x 16 microsteps = 10369.3975674 impulses/s 
  // Impules @ 10.369 kHz, so wait 90µs
  
  // 1 full tr in 86164 seconds witch is demultiplied by 6^6
  // it requires the motor to do 0.54147903997 tr/s
  // Impules @ 1732.7329279 Hz, so wait 577.1236550497256 µs
  
  pinMode(6,OUTPUT); // Enable
  pinMode(5,OUTPUT); // Step
  pinMode(4,OUTPUT); // Dir
  
  digitalWrite(6,LOW); // Set Enable low
  digitalWrite(4,HIGH); // Set Dir high
  
  pinMode(7,OUTPUT); // MS1
  pinMode(8,OUTPUT); // MS2
  pinMode(9,OUTPUT); // MS3
  
  // Set microstep mode
  digitalWrite(7,HIGH); 
  digitalWrite(8,HIGH);
  digitalWrite(9,HIGH);
  
  Timer1.attachInterrupt(motorStep); // Impulse a 1/3200th tr
  Serial.begin(9600);
  Serial.println("Motor stepper test");
  Serial.println("Starting now !");
}


// Keep a track of the accumulated delay
uint32_t StepCons=(577.1236550497257)*pow(2,STEP_SHIFT);
uint32_t lastStepErr = 0;
uint32_t StepCurr = StepCons;

void motorStep(void)
{ 
  digitalWrite(5,HIGH); // Impulse start   
  digitalWrite(5,LOW); // Impulse stop. 
  
  StepCurr = StepCons + lastStepErr;
  Timer1.setPeriod(StepCurr>>STEP_SHIFT);
  //Serial.println(StepCurr>>STEP_SHIFT);
  lastStepErr = StepCurr & (((uint32_t)~0)>>(32 - STEP_SHIFT) ); // On prend juste le poid faible
}

void loop(void)
{
  // Each n time, reset the timer to correct periodic error
  //Timer1.setPeriod(microseconds);
  
  //noInterrupts();
  //interrupts();
  //Serial.print("Last step error : ");
  //Serial.println(lastStepErr);
  //Serial.print("Current Step : ");
  //Serial.println(StepCurr>>STEP_SHIFT);
  //delay(50);
}
