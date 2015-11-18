#include <TimerOne.h>

// 20 bits après le virgule, à dimenssionner suivant la vitesse
// Plus la vitesse est faible, plus STEP_SHIFT doit être petit
// Et inversement.
// Ex: pour une freq de 10kHz, 24 est une bonne valeur,
// Pour une freq de 1.7 kHz, 20 semble bon

#define motEnable 6
#define motStep 5
#define motDir 4
#define MS1 7
#define MS2 8
#define MS3 9

#define INITFREQ (90)
#define STEP_SHIFT (20)

const int freq = 10369;

void setup(void)
{
  Timer1.initialize(INITFREQ); // Call interupts each 90µs
  
  // 1 full tr in 86164 seconds witch is demultiplied by 6^7
  // it requires the motor to do 3.248874 tr/s
  // 3.24887423982 tr/s x 200 steps x 16 microsteps = 10369.3975674 impulses/s 
  // Impules @ 10.369 kHz, so wait 90µs
  
  // 1 full tr in 86164 seconds witch is demultiplied by 6^6
  // it requires the motor to do 0.54147903997 tr/s
  // Impules @ 1732.7329279 Hz, so wait 577.1236550497256 µs
  
  // Demultiplication with 6^4 plus 10 time demult
  // 1/((((6^4)*10)/86164.10)*200*16)*1000000
  // 2077.6451581790129239 µs
  
  pinMode(motEnable,OUTPUT); // Enable
  pinMode(motStep,OUTPUT); // Step
  pinMode(motDir,OUTPUT); // Dir
  
  digitalWrite(motEnable,LOW); // Set Enable low
  digitalWrite(motDir,LOW); // Set Dir high
  
  pinMode(MS1,OUTPUT); // MS1
  pinMode(MS2,OUTPUT); // MS2
  pinMode(MS3,OUTPUT); // MS3
  
  // Set microstep mode
  setMicroStep();
  
  Timer1.attachInterrupt(sideralSpeed);
  Serial.begin(115200);
  Serial.println("Motor stepper test");
  Serial.println("Starting now !");
}

void setMicroStep(void)
{
  // Set microstep mode
  digitalWrite(MS1,HIGH); 
  digitalWrite(MS2,HIGH);
  digitalWrite(MS3,HIGH);
}

void setFullStep(void)
{
  // Set fullstep mode
  digitalWrite(MS1,LOW); 
  digitalWrite(MS2,LOW);
  digitalWrite(MS3,LOW);
}

// Keep a track of the accumulated delay
//uint32_t StepCons=(577.1236550497257)*pow(2,STEP_SHIFT);
uint32_t StepCons=(2077.64515817901292394)*pow(2,STEP_SHIFT);
uint32_t lastStepErr = 0;
uint32_t StepCurr = StepCons;

void sideralSpeed(void)
{ 
  digitalWrite(motStep,HIGH); // Impulse start   
  digitalWrite(motStep,LOW); // Impulse stop. 
  
  StepCurr = StepCons + lastStepErr;
  Timer1.setPeriod(StepCurr>>STEP_SHIFT);
  //Serial.println(StepCurr>>STEP_SHIFT);
  lastStepErr = StepCurr & (((uint32_t)~0)>>(32 - STEP_SHIFT) ); // On prend juste le poid faible
}

// Begin @ 600tr/min
int StepFast = 500;
  //0 = Clockwise , 1 = Anti-clockwise
void fastSpeed(void)
{
  digitalWrite(motStep,HIGH); // Impulse start   
  digitalWrite(motStep,LOW); // Impulse stop.
  
  // Speed = number_of_step / time_between_steps so function like 1/x
  // to increase speed linearly: multiply by (constant x time_between_steps²)
  
  // Linear acceleration to 2340 tr/min
  if (StepFast > 129) {
    //10.000.000 give a nominal speed in 0.13s
    StepFast -= 1/10000000*pow(StepFast,2);
    Timer1.setPeriod(StepFast);
  }
}

void loop(void)
{
  // Each n time, reset the timer to correct periodic error
  //Timer1.setPeriod(microseconds);
  
  /* Début de mode acceleré
  if(){
    setFullStep();
    Timer1.attachInterrupt(fastSpeed);
  }
  
  if() {
    setMicroStep();
    Timer1.attachInterrupt(sideralSpeed);
  }
  */
  
  //noInterrupts();
  //interrupts();
  //Serial.print("Last step error : ");
  //Serial.println(lastStepErr);
  //Serial.print("Current Step : ");
  Serial.println(StepCurr>>STEP_SHIFT);
  delay(1);
}
