/*  ___   ___  ___  _   _  ___   ___   ____ ___  ____  
 * / _ \ /___)/ _ \| | | |/ _ \ / _ \ / ___) _ \|    \ 
 *| |_| |___ | |_| | |_| | |_| | |_| ( (__| |_| | | | |
 * \___/(___/ \___/ \__  |\___/ \___(_)____)___/|_|_|_|
 *                  (____/ 
 * Osoyoo Wifi Arduino Robot Car project
 * USe WI-FI UDP protocol to control robot car
 * tutorial url: https://osoyoo.com/?p=32758
 */
/*Declare L298N Dual H-Bridge Motor Controller directly since there is not a library to load.*/


//Define L298N Dual H-Bridge Motor Controller Pins
#define speedPinR 5    //  RIGHT PWM pin connect MODEL-X ENA
#define RightMotorDirPin1  7    //Right Motor direction pin 1 to MODEL-X IN1 
#define RightMotorDirPin2  8    //Right Motor direction pin 2 to MODEL-X IN2
#define speedPinL 6    // Left PWM pin connect MODEL-X ENB
#define LeftMotorDirPin1  9    //Left Motor direction pin 1 to MODEL-X IN3 
#define LeftMotorDirPin2  10   //Left Motor direction pin 1 to MODEL-X IN4 


/*From left to right, connect to D3,A1-A3 ,D10*/
#define LFSensor_0 A0  //OLD D3
#define LFSensor_1 A1
#define LFSensor_2 A2
#define LFSensor_3 A3
#define LFSensor_4 A4  //OLD D10

#define FAST_SPEED 70
#define MID_SPEED 65
#define SLOW_SPEED  55     //back speed


/*motor control*/
void go_Advance(void)  //Forward
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2,HIGH);
  digitalWrite(LeftMotorDirPin1,LOW);
  digitalWrite(LeftMotorDirPin2,HIGH);
  analogWrite(speedPinL,100);
  analogWrite(speedPinR,100);
}
void go_Left(int t=0)  //Turn left
{
  digitalWrite(RightMotorDirPin1,LOW );
  digitalWrite(RightMotorDirPin2,HIGH);
  digitalWrite(LeftMotorDirPin1,HIGH);
  digitalWrite(LeftMotorDirPin2,LOW);
  analogWrite(speedPinL,0);
  analogWrite(speedPinR,100);
  delay(t);
}
void go_Right(int t=0)  //Turn right
{
  digitalWrite(RightMotorDirPin1,HIGH);
  digitalWrite(RightMotorDirPin2,LOW);
  digitalWrite(LeftMotorDirPin1,LOW);
  digitalWrite(LeftMotorDirPin2,HIGH);
  analogWrite(speedPinL,100);
  analogWrite(speedPinR,0);
  delay(t);
}
void go_Back(int t=0)  //Reverse
{
  digitalWrite(RightMotorDirPin1,HIGH);
  digitalWrite(RightMotorDirPin2,LOW);
  digitalWrite(LeftMotorDirPin1,HIGH);
  digitalWrite(LeftMotorDirPin2,LOW);
  analogWrite(speedPinL,100);
  analogWrite(speedPinR,100);
  delay(t);
}
void stop_Stop()    //Stop
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2,LOW);
  digitalWrite(LeftMotorDirPin1,LOW);
  digitalWrite(LeftMotorDirPin2,LOW);
}
/*set motor speed */
void set_Motorspeed(int speed_L,int speed_R)
{
  analogWrite(speedPinL,speed_L); 
  analogWrite(speedPinR,speed_R);   
}

void setup()
{
   
  pinMode(RightMotorDirPin1, OUTPUT); 
  pinMode(RightMotorDirPin2, OUTPUT); 
  pinMode(speedPinL, OUTPUT);  
 
  pinMode(LeftMotorDirPin1, OUTPUT);
  pinMode(LeftMotorDirPin2, OUTPUT); 
  pinMode(speedPinR, OUTPUT); 
  stop_Stop();//stop move  

  Serial.begin(9600);   // initialize serial for debugging

}

boolean flag=false;
void loop()
{ 
 auto_tracking();
} //end of loop
 
char sensor[5];
 /*read sensor value string, 1 stands for black, 0 starnds for white, i.e 10000 means the first sensor(from left) detect black line, other 4 sensors detected white ground */
String read_sensor_values()
{   int sensorvalue=32;
  sensor[0]= digitalRead(LFSensor_0);
 
  sensor[1]=digitalRead(LFSensor_1);
 
  sensor[2]=digitalRead(LFSensor_2);
 
  sensor[3]=digitalRead(LFSensor_3);
 
  sensor[4]=digitalRead(LFSensor_4);
  sensorvalue +=sensor[0]*16+sensor[1]*8+sensor[2]*4+sensor[3]*2+sensor[4];
  sensorvalue =~sensorvalue;
  String senstr= String(sensorvalue,BIN);
  senstr=senstr.substring(11,16);


  return senstr;
}

void auto_tracking(){
 String sensorval= read_sensor_values();
  Serial.println(sensorval);
 if (  sensorval=="11000" || sensorval=="10000"  || sensorval=="01000")
 { 
  //The black line is in the left of the car, need  left turn 
      go_Left();  //Turn left
    set_Motorspeed(0,FAST_SPEED);
 //   delay(50);
 //   stop_Stop();
    }
else if (sensorval=="10100" || sensorval=="00100" || sensorval=="01100" || sensorval=="11100"  || sensorval=="10010" || sensorval=="11010")
{
      go_Advance();  //Turn slight left
    set_Motorspeed(SLOW_SPEED,MID_SPEED);
 //   delay(50);
//    stop_Stop();
  
}
 else if ( sensorval=="00011" || sensorval=="00001"  || sensorval=="00010" ){ //The black line is  on the right of the car, need  right turn 
  
     go_Right();  //Turn right
       set_Motorspeed(FAST_SPEED,0);
          // delay(50);
   // stop_Stop();
    }
 else if (sensorval=="00101" || sensorval=="00110" || sensorval=="00111" || sensorval=="01101" || sensorval=="01111"   || sensorval=="01011" || sensorval=="01110"  || sensorval=="01001")
 {
       go_Advance();  //Turn slight right
       set_Motorspeed( MID_SPEED,SLOW_SPEED);
        //   delay(50);
    //stop_Stop();
 }
  else if (sensorval=="01110"  || sensorval=="00100"   ){
      go_Advance();  //Turn slight right
       set_Motorspeed(MID_SPEED,MID_SPEED);
  //         delay(50);
  //  stop_Stop();
  }
 else if (sensorval=="11111"){
   stop_Stop();   //The car front touch stop line, need stop
    set_Motorspeed(0,0);
    }
 
  else   if (sensorval=="00000" ){
    go_Advance();  //Turn slight right
       set_Motorspeed(SLOW_SPEED,SLOW_SPEED);
     delay(50);
     stop_Stop();
    }
    
}
