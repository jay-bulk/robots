#define Left_Pin A0
#define Center_Pin A1
#define Right_Pin A2

#define ENA 5  	// D5  connect MODEL-X ENA (PWM of right wheels)
#define IN1 7   //Right Motor direction pin D7 to MODEL-X IN1 
#define IN2 8   //Right Motor direction pin D8 to MODEL-X IN2
#define ENB 6	// D6 connect MODEL-X ENB (PWM of left wheels)
#define IN3 9   //Left Motor direction pin 9 to MODEL-X IN3 
#define IN4 10  //Left Motor direction pin 10 to MODEL-X IN4 

#define TURN_TIME  50 
#define SPEED  150   
#define MOVE_TIME  50   

 
String read_sensor_values()
{   int sensorvalue=8;
   int sensor_left,sensor_center,sensor_right;
  sensor_left= digitalRead(Left_Pin);
 
  sensor_center=digitalRead(Center_Pin);
 
  sensor_right=digitalRead(Right_Pin);
 
  sensorvalue +=sensor_left*4+sensor_center*2+sensor_right;
 
  String senstr= String(sensorvalue,BIN);
 
  return senstr.substring(1,4);
}
/*motor control*/
void go_back()  //motor rotate clockwise -->robot go back
{
  digitalWrite(IN4,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN1,HIGH);
 
}
void go_ahead() //motor rotate counterclockwise -->robot go ahead
{
  digitalWrite(IN4,HIGH);
  digitalWrite(IN3,LOW); 
  digitalWrite(IN2,HIGH);
  digitalWrite(IN1,LOW);
   

}
void stop_Stop() //motor brake -->robot stop
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4,LOW); 
  set_Motorspeed(0,0);
}
void turn_right()  //left motor rotate clockwise and right motor rotate counterclockwise -->robot turn right
{
  digitalWrite(IN4,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN1,LOW);
  set_Motorspeed(SPEED,0);
  
}
void turn_left() //left motor rotate counterclockwise and right motor rotate clockwise -->robot turn left
{
  digitalWrite(IN4,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN1,HIGH);
    set_Motorspeed(0,SPEED);
 
}
/*set motor speed */
void set_Motorspeed(int lspeed,int rspeed) //change motor speed
{
  analogWrite(ENB,lspeed);//lspeed:0-255
  analogWrite(ENA,rspeed);//rspeed:0-255   
}
void setup() {
  /******L298N******/
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT); 
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT); 
  pinMode(ENA, OUTPUT);  
  pinMode(ENB, OUTPUT);
  pinMode(Left_Pin, INPUT);
  pinMode(Center_Pin, INPUT);
  pinMode(Right_Pin, INPUT);
  Serial.begin(9600);
 /*
  go_ahead();
  set_Motorspeed(180,180);
   delay(1000);
 
 
    go_back();
  set_Motorspeed(180,180);
  delay(1000);
 
  turn_left();

  delay(1000);

    turn_right();

  delay(1000);
  
  stop_Stop();
 */
}


void auto_tracking(){
 String sensorval= read_sensor_values();
 Serial.println(sensorval);
 if (sensorval=="100" or sensorval=="110")
 { 
    //The black line is in the left of the car, need  left turn 
  Serial.println("TURN right");
      turn_right();  //Turn right
           set_Motorspeed(SPEED*2/3,SPEED*6/8);
    delay(TURN_TIME);
    stop_Stop();
   delay(50);
  
    }

 if (sensorval=="011" or sensorval=="001"){ 
     //The black line is  on the right of the car, need  right turn 
         turn_left();  //Turn left
      Serial.println("TURN left");
       set_Motorspeed(SPEED*6/8,SPEED*2/3);
           delay(TURN_TIME);
      stop_Stop();
      delay(50);
     
    }
  if (sensorval=="010" or sensorval=="101"){ //The black line is  on the right of the car, need  right turn 
    Serial.println("go ahead");
    go_ahead();
    set_Motorspeed(SPEED*5/6,SPEED*5/6);
    delay(MOVE_TIME);
    stop_Stop();
    delay(50);
 
    }
 if (sensorval=="000" or sensorval=="111"){
    Serial.println("back");
          go_back();
       set_Motorspeed(SPEED*2/3,SPEED*2/3);
           delay(MOVE_TIME/3*2);
      stop_Stop();
      delay(50);
   
     
    }
 
}

void loop(){
 
auto_tracking();
}
