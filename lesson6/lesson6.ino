/*  ___   ___  ___  _   _  ___   ___   ____ ___  ____  
 * / _ \ /___)/ _ \| | | |/ _ \ / _ \ / ___) _ \|    \ 
 *| |_| |___ | |_| | |_| | |_| | |_| ( (__| |_| | | | |
 * \___/(___/ \___/ \__  |\___/ \___(_)____)___/|_|_|_|
 *                  (____/ 
 * Arduino Smart Car Tutorial Lesson 7
 * Tutorial URL http://osoyoo.com/2019/02/26/osoyoo-v2-robot-car-lesson-7-imitation-driving-with-bluetooth-app/
 * CopyRight www.osoyoo.com

 * After running the code, smart car will go forward 5 seconds, then go backward 5
 * seconds, then left turn for 5 seconds then right turn for 5 seconds then stop. 
 * 
 */
 
int buttonState;

#define speedPinRight 5    			//  D5  connect MODEL-X ENA (PWM of right wheels)
#define dir1PinRight  7    //Right Motor direction pin D7 to MODEL-X IN1 
#define dir2PinRight  8    //Right Motor direction pin D8 to MODEL-X IN2
#define speedPinLeft 6    			// D6 connect MODEL-X ENB (PWM of left wheels)
#define dir1PinLeft  9    	//Left Motor direction pin 9 to MODEL-X IN3 
#define dir2PinLeft  10   	//Left Motor direction pin 10 to MODEL-X IN4 

   //back speed
#define MAX_PACKETSIZE 32    //Serial receive buffer

struct car_status{
  int speed;
  int angle;
  int direct;
};
char buffUART[MAX_PACKETSIZE];
unsigned int buffUARTIndex = 0;
unsigned long preUARTTick = 0;
int SPEED =150;  //avoidance motor speed 
int TURNSPEED=50  ;
/*motor control*/
void go_Back(void)  //Forward
{
  digitalWrite(dir1PinRight, HIGH);
  digitalWrite(dir2PinRight,LOW);
  digitalWrite(dir1PinLeft,HIGH);
  digitalWrite(dir2PinLeft,LOW);

}
void go_Right()  //Turn left
{
  digitalWrite(dir1PinRight, HIGH);
  digitalWrite(dir2PinRight,LOW);
  digitalWrite(dir1PinLeft,LOW);
  digitalWrite(dir2PinLeft,HIGH);
}
void go_Left()  //Turn right
{
  digitalWrite(dir1PinRight, LOW);
  digitalWrite(dir2PinRight,HIGH);
  digitalWrite(dir1PinLeft,HIGH);
  digitalWrite(dir2PinLeft,LOW);
}
void go_Advance()  //Reverse
{
  digitalWrite(dir1PinRight, LOW);
  digitalWrite(dir2PinRight,HIGH);
  digitalWrite(dir1PinLeft,LOW);
  digitalWrite(dir2PinLeft,HIGH);
}
void back_Right()  //Reverse
{
  digitalWrite(dir1PinRight, LOW);
  digitalWrite(dir2PinRight,HIGH);
  digitalWrite(dir1PinLeft,HIGH);
  digitalWrite(dir2PinLeft,LOW);
}
void back_Left()  //Reverse
{
  digitalWrite(dir1PinRight, HIGH);
  digitalWrite(dir2PinRight,LOW);
  digitalWrite(dir1PinLeft,LOW);
  digitalWrite(dir2PinLeft,HIGH);
}
void stop_Stop(){
    digitalWrite(dir1PinRight,LOW);
  digitalWrite(dir2PinRight,LOW);
  digitalWrite(dir1PinLeft,LOW);
  digitalWrite(dir2PinLeft,LOW);
   set_Motorspeed(0,0) ;
}

/*set motor speed */
void set_Motorspeed(int speed_L,int speed_R)
{
  analogWrite(speedPinRight,speed_R); 
  analogWrite(speedPinLeft,speed_L);   
}


//WiFi / Bluetooth through the serial control
void do_Uart_Tick()
{

  char Uart_Date=0;
  if(Serial.available()) 
  {
    size_t len = Serial.available();
    uint8_t sbuf[len + 1];
    sbuf[len] = 0x00;
    Serial.readBytes(sbuf, len);
    //parseUartPackage((char*)sbuf);
    memcpy(buffUART + buffUARTIndex, sbuf, len);//ensure that the serial port can read the entire frame of data
    buffUARTIndex += len;
    preUARTTick = millis();
    if(buffUARTIndex >= MAX_PACKETSIZE - 1) 
    {
      buffUARTIndex = MAX_PACKETSIZE - 2;
      preUARTTick = preUARTTick - 200;
    }
  }
  car_status cs;
  if(buffUARTIndex > 0 && (millis() - preUARTTick >= 100))//APP send flag to modify the obstacle avoidance parameters
  { //data ready
    buffUART[buffUARTIndex] = 0x00;
    Uart_Date=buffUART[0];
    cs=get_status(buffUART);
    buffUARTIndex = 0;
  }
   SPEED=cs.speed+35;
   TURNSPEED=SPEED*3/2;
   if (SPEED>250) SPEED=250;
   if (TURNSPEED>250) TURNSPEED=250;
  switch (Uart_Date)    //serial control instructions
  {
    case 'M':  
    go_Advance();
    set_Motorspeed(SPEED,SPEED) ;
    break;
    case 'L':  
    go_Left(); 
    if(cs.angle=1){
       set_Motorspeed(0,60) ;
    }
    if(cs.angle=2){
       set_Motorspeed(20,70) ;
    }
    break;
    case 'R': 
    go_Right(); 
    if(cs.angle=-1){
       set_Motorspeed(60,0) ;
    }
    if(cs.angle=-2){
      set_Motorspeed(70,20) ;
    }
    break;
    case 'B':  
    go_Back(); 
       set_Motorspeed(SPEED,SPEED) ;
    break;
    case 'X': 
    back_Left();
    if(cs.angle=-1){
       set_Motorspeed(0,60) ;
    }
    if(cs.angle=-2){
       set_Motorspeed(20,70) ;
    }
    break;
    case 'Y':  
    back_Right(); 
    if(cs.angle=1){
       set_Motorspeed(60,0) ;
    }
    if(cs.angle=2){
      set_Motorspeed(70,20)
      ;
    }
    break;
 
    case 'E': stop_Stop() ;break;
    case 'J': stop_Stop() ;break;
    default:break;
  }
}

//car motor control
void setup()
{
 
  pinMode(dir1PinRight, OUTPUT); 
  pinMode(dir2PinRight, OUTPUT); 
  pinMode(speedPinRight, OUTPUT);  
  pinMode(dir1PinLeft, OUTPUT);
  pinMode(dir2PinLeft, OUTPUT); 
  pinMode(speedPinLeft, OUTPUT); 


  Serial.begin(9600);//In order to fit the Bluetooth module's default baud rate, only 9600
 
}
int lastButtonState=0;
void loop()
{  
do_Uart_Tick();
 
}

car_status get_status( char buffUART[])
{
  car_status cstatus;
  int index=2;
  if (buffUART[index]=='-'){
    cstatus.angle=-buffUART[index+1]+'0';
    index=index+3;
    
  } else {
   
    cstatus.angle=buffUART[index]-'0';
     index=index+2;
  }
  int currentvalue;
  int spd=0;
  while (buffUART[index]!=',')
  {
    currentvalue=buffUART[index]-'0';
    spd=spd*10+currentvalue;
    index++;
  }
  cstatus.speed=spd;
  index++;
  cstatus.direct=buffUART[index]-'0';
  return cstatus;
}
