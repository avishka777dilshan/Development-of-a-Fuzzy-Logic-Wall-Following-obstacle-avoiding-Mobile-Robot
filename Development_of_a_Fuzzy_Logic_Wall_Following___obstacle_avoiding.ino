#include <AFMotor.h> 
AF_DCMotor mR (3);
AF_DCMotor mL (4);

// sensor pins configuration
#define trigL 11
#define ecoL 10
#define trigF 3 
#define ecoF 2
#define trigR 13
#define ecoR 9

//FUZZY configuration
#define negativeBigSpeed -200
#define negativeSmallSpeed -100
#define positiveBigSpeed 200
#define positiveSmallSpeed 100
#define normalSpeed 0
float negative,zero,positive;
float delta_negative,delta_zero,delta_positive;
float negativeBig,negativeSmall,positiveBig,positiveSmall,normal;
float PWM_difference,PWM,PWM_VAL;
float error,previous_error,delta_error;
const float SetDistance=15;

static float cmL,cmF,cmR,durationL,durationF,durationR,distanceL,distanceF,distanceR;
static float PWM_Forward=80;
const float turn_delay=2000;

/*=============================================================================MAIN=====================================================================================*/
void setup() {
  Serial.begin(9600);
  sensorPinConfig();  
  sensorTrigure();
  error=SetDistance-cmL;
  previous_error=error;
}

/*=============================================================================LOOP=====================================================================================*/
void loop() {
  
  sensorTrigure();
  PWM_VAL=errorCalc();
 // delay(500);
  Serial.println("");
  Serial.print("PWM_VAL = ");
  Serial.println(PWM_VAL);
  Serial.print("cmL     = ");
  Serial.println(cmL);
  
  if(cmL<=25){
      mR.setSpeed(PWM_Forward-(2*PWM_VAL));
      mL.setSpeed(PWM_Forward+(2*PWM_VAL));
      mR.run(FORWARD);
      mL.run(FORWARD);
    
  }
  static int i;
  if(i>0){
  if((cmL<30)&&(cmF<40)){
    mR.setSpeed(40);
    mL.setSpeed(200);
    mR.run(FORWARD);
    mL.run(FORWARD);
    delay(1000);
    }
  }
  if(cmL>30){
    
    mR.setSpeed(200);
    mL.setSpeed(0);
    mR.run(FORWARD);
    mL.run(FORWARD);
    delay(1500); 
    mR.setSpeed(100);
    mL.setSpeed(100);
    delay(2000);
    }
    
   if((cmL<15)&&(cmF<15)&&(cmR<15)){
    mR.setSpeed(0);
    mL.setSpeed(0);
    mR.run(FORWARD);
    mL.run(FORWARD);
    }
  
 i++;
}
/*===================================================================SENSOR PIN CONFIGURATION===========================================================================*/
void sensorPinConfig(){
  pinMode(trigL, OUTPUT); // initialising as output
  pinMode(ecoL, INPUT); // initialising input
  pinMode(trigF, OUTPUT); // initialising output
  pinMode(ecoF, INPUT); // initialising input
  pinMode(trigR, OUTPUT); // initialising output
  pinMode(ecoR, INPUT); // initialising input
  
  }
/*=======================================================================SENSOR TRIGURE=================================================================================*/
void sensorTrigure(){
   
  digitalWrite(trigL, LOW);
  delayMicroseconds(2);
  digitalWrite(trigL, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigL, LOW);
  calculationL();
  
  digitalWrite(trigF, LOW);
  delayMicroseconds(2);
  digitalWrite(trigF, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigF, LOW);
  calculationF();
  
  digitalWrite(trigR, LOW);
  delayMicroseconds(2);
  digitalWrite(trigR, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigR, LOW);
  calculationR();
  
  }
/*====================================================================SENSOR DISTANCE CALCULATION========================================================================*/
void calculationL(){
  durationL = pulseIn(ecoL, HIGH); // using pulsin function to determine total time
  distanceL = durationL*(0.034)/2;
  cmL = distanceL; // calling method
  return(cmL);
  }
  
void calculationF(){
  durationF = pulseIn(ecoF, HIGH); // using pulsin function to determine total time
  distanceF = durationF*(0.034)/2;
  cmF = distanceF; // calling method
  return(cmF);
  }

void calculationR(){
  durationR = pulseIn(ecoR, HIGH); // using pulsin function to determine total time
  distanceR = durationR*(0.034)/2;
  cmR = distanceR; // calling method
  return(cmR);
  }
/*=======================================================================================================================================================================*/
/*=====================================================================FUZZY IMPLIMANTATION==============================================================================*/
/*=======================================================================================================================================================================*/

/*
#define negativeBigSpeed -20
#define negativeSmallSpeed -10
#define positiveBigSpeed 20
#define positiveSmallSpeed 10
#define normalSpeed 0
int negative,zero,positive;
int delta_negative,delta_zero,delta_positive;
int negativeBig,negativeSmall,positiveBig,positiveSmall,normal;
int PWM_Left=100;
int PWM_Right=100;
*/

/*====================================================================MEMBER SHIP FUNCTIONS==============================================================================*/

/*----------------------------------------------------------------------------ERROR--------------------------------------------------------------------------------------*/

float membershipFunction_error(float error){
if(error<=(-5)){
  negative=1;
  zero=0;
  positive=0;
  }
if((error>(-5))&&(error<=0)){
  zero=(error-(-5))/5;
  negative=1-zero;
  positive=0;
  }
if((error>0)&&(error<=5)){
  positive=(error)/5;
  zero=1-positive;
  negative=0;
  }
if(error>5){
  negative=0;
  zero=0;
  positive=1;  
  }

return(negative,zero,positive);
}

/*------------------------------------------------------------------------DELTA ERROR------------------------------------------------------------------------------------*/
float membershipFunction_delta_error(float delta_error){
if(delta_error<=(-5)){
  delta_negative=1;
  delta_zero=0;
  delta_positive=0;
  }
if((error>(-5))&&(error<=0)){
  delta_zero=(delta_error-(-5))/5;
  delta_negative=1-delta_zero;
  delta_positive=0;
  }
else if((error>0)&&(error<=5)){
  delta_positive=(delta_error)/5;
  delta_zero=1-delta_positive;
  delta_negative=0;
  }
if(error>5){
  delta_negative=0;
  delta_zero=0;
  delta_positive=1;
  }
  return(delta_negative,delta_zero,delta_positive);
}

/*========================================================================INFERENCE ENGINE===============================================================================*/
float inferenceEngine(float negative,float zero,float positive,float delta_negative,float delta_zero,float delta_positive){
if(negative && delta_negative ){
  if(negative<=delta_negative){
    negativeBig=negative;
  }else{
    negativeBig=delta_negative;
    }
  }

if(negative && delta_zero ){
  if(negative<delta_zero){
    negativeSmall=negative;
  }else{
    negativeSmall=delta_zero;
    }
  }

if(negative && delta_positive){
  normal=normalSpeed;
  
  }
  
if(zero && delta_negative ){
  if(zero<delta_negative){
    negativeSmall=zero;
  }else{
    negativeSmall=delta_negative;
    }
  }
  
if(zero && delta_zero){
  normal=normalSpeed;
  
  }
  
  
if(zero && delta_positive ){
  if(zero<delta_positive){
    positiveSmall=zero;
  }else{
    positiveSmall=delta_positive;
    }
  }
  
if(positive && delta_negative){
  normal=normalSpeed;
  
  }
  

if(positive && delta_zero ){
  if(positive<delta_zero){
    positiveSmall=positive;
  }else{
    positiveSmall=delta_zero;
    }
  }


if(positive && delta_positive ){
  if(positive<delta_positive){
    positiveBig=positive;
  }else{
    positiveBig=delta_positive;
    }
  }

else{
  normal=normalSpeed;
  }

  return(negativeBig,negativeSmall,positiveSmall,positiveBig,normal);
}

/*========================================================================WEIGHTED FUNCTION===============================================================================*/

int weighted(float negativeBig,float negativeSmall,float positiveSmall,float positiveBig,float normal){
 
 PWM_difference =  ((negativeBigSpeed*negativeBig)+(negativeSmallSpeed*negativeSmall)+(positiveBigSpeed*positiveBig)+(positiveSmallSpeed*positiveSmall)+(normal*zero))/(negativeBig+negativeSmall+positiveBig+positiveSmall+zero);

 return(PWM_difference);
}

/*========================================================================ERROR CALCULATION===============================================================================*/
int errorCalc(){
  error=SetDistance-cmL;
  delta_error=error-previous_error;
  previous_error=error;

  Serial.print("error =");
  Serial.println(error);
  Serial.print("delta_error = ");
  Serial.println(delta_error);

  membershipFunction_error(error);
  membershipFunction_delta_error(delta_error);
  inferenceEngine(negative,zero,positive,delta_negative,delta_zero,delta_positive);
  PWM=weighted(negativeBig,negativeSmall,positiveSmall,positiveBig,normal);
  return(PWM);
  }
