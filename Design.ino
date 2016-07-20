
int LED1=11;
int LED2=12;
int LED3=13;
int LED4=2;
int LED5=15;
int A_switch=4;
int B_switch=7;
int C_switch=9;
int T=50;
int button1;
int button2;
int button3;
const int LED = 14;                 // pin 14: green LED on the LaunchPad board
const int delay_value = 5;          // controls the speed with which the LED fades in and out
const int fade_switch = 6;          // normally-open momentary contact switch from pin 6 to GND
const int sleep_switch = 5;         // normally-open momentary contact switch from pin 5 to GND (S2 on LaunchPad)
const int debounce_delay = 250;     // 1/4 sec debouncing delay after sleep switch is closed or opened
const int fadeValue_min = 50;       // sets the lowest LED brightness value
const int fadeValue_max = 255;      // sets the highest LED brightness value
const int fadeValue_startup = 128;  // sets the initial LED brightness value
int fadeValue_step = 1;             // this value gets negated when endpoints are reached (not a constant)
int fadeValue = 0;                  // define global variable that is used 


int c[5][4][2]=
{
 { {LED1,LED2},{LED1,LED3},{LED1,LED4},{LED1,LED5} },
 { {LED2,LED3},{LED2,LED4},{LED2,LED5},{LED3,LED4} },
 { {LED3,LED5},{LED4,LED5},{LED5,LED4},{LED5,LED3} },
 { {LED4,LED3},{LED5,LED2},{LED4,LED2},{LED3,LED2} },
 { {LED5,LED1},{LED4,LED1},{LED3,LED1},{LED2,LED1} }
};
void setup(){
 pinMode(A_switch,INPUT_PULLUP);
 pinMode(B_switch,INPUT_PULLUP);
 pinMode(C_switch,INPUT_PULLUP);
 pinMode(LED1,INPUT);
 pinMode(LED2,INPUT);
 pinMode(LED3,INPUT);
 pinMode(LED4,INPUT);
 pinMode(LED5,INPUT);
 Initialize_Pins();   // configure I/O pins for normal operation
 fadeValue = fadeValue_startup;
}

void light(int pins[2]){
 pinMode( pins[0], OUTPUT );
 digitalWrite( pins[0], HIGH );

 pinMode( pins[1], OUTPUT );
 digitalWrite( pins[1], LOW );
}

void test (int pins[2],int T){
 setup();
 light(pins);
 delay(T);
 pinMode( pins[0], INPUT );
 pinMode( pins[1], INPUT );
}
 
void loop(){
  Output_to_LED(fadeValue,LED);  
  while (digitalRead(fade_switch) == LOW) {
    fadeValue = min(max(fadeValue+fadeValue_step,fadeValue_min),fadeValue_max);
    Output_to_LED(fadeValue,LED);
    delay(delay_value);
    if ((fadeValue == fadeValue_max) | (fadeValue == fadeValue_min)) {
      fadeValue_step = -fadeValue_step;
    }
  }
  
  if (digitalRead(sleep_switch) == LOW) {
      Fall_Asleep();
  }
  
 bailout2:
   for( int i=0; i<5; i++ ){
    for( int j=0; j<4; j++ ){
      test(c[i][j],T);
      if(i==4 && j==3){
        button2=digitalRead(B_switch);
        if(button2==LOW)
          T=50;
        else{
          light(c[4][3]);
          T=200;
          button3=digitalRead(C_switch);
          while(button3==HIGH){
          button3=digitalRead(C_switch);
          }
         }
      }
      if(i==4 && j==2){
        button2=digitalRead(B_switch);
        if(button2==LOW){
          T=100;
          goto bailout1;
        }
      }
      if(i==4 && j==1){
        button2=digitalRead(B_switch);
        if(button2==LOW){
          T=200;
          goto bailout1;
        }
      }
    
     }
   }
   bailout1:
  for( int i=4; i>=0; i-- ){
      for( int j=3; j>=0; j-- ){
      test(c[i][j],T);
      if(i==0 && j==0){
        button1=digitalRead(A_switch);
        if(button1==LOW){
          T=50;
          goto bailout2;
        }
        else{
          light(c[0][0]);
          T=200;
          button3=digitalRead(C_switch);
          while(button3==HIGH){
            button3=digitalRead(C_switch);
          }
         }
      }
    if(i==0 && j==1){
        button1=digitalRead(A_switch);
        if(button1==LOW){
          T=100;
          goto bailout2;
        }
    }
    if(i==0 && j==2){
        button1=digitalRead(A_switch);
        if(button1==LOW){
         T=200;
         goto bailout2;
        }
      }
     }
   }  

}
void Output_to_LED(int level, int LED_pin) {
// Function to send PWM signal to LED_pin (0 <= level <= 255)
    // nonlinearly process level to linearize the perceived LED brightness:
    float ftemp = float(level)/255;
    int   itemp = int(255*ftemp*ftemp*ftemp);
    // send PWM code to the specified microcontroller output pin:
    analogWrite(LED_pin,itemp);
}
void Initialize_Pins(void) {
  // Function to intialize direction of the I/O pins
  // (do not remove, as this function is called by Fall_Asleep)
  pinMode(LED,OUTPUT);
  pinMode(fade_switch,INPUT_PULLUP);
  pinMode(sleep_switch,INPUT_PULLUP);
}


void Fall_Asleep(void) {
  delay(debounce_delay);     // delay for approximately 250ms, longer than duration of switch bounce
  // Put all I/O pins into input mode to save power when uC goes to sleep:
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  pinMode(5,INPUT_PULLUP);   // pullup resistor for switch that will interrupt sleep mode
  pinMode(6,INPUT);
  pinMode(7,INPUT);
  pinMode(8,INPUT);
  pinMode(9,INPUT);
  pinMode(10,INPUT);
  pinMode(11,INPUT);
  pinMode(12,INPUT);
  pinMode(13,INPUT);
  pinMode(14,INPUT);
  pinMode(15,INPUT);
  // Enable interrupt upon HIGH to LOW transition at pin 5 (P1.3, S2 on LaunchPad board)
  P1IE |= BIT3;              // Enable interrupt by change in voltage at pin P1.3
  P1IES |= BIT3;             // Specifically, trigger interrupt at HIGH to LOW transition in P1.3
  P1IFG &= ~BIT3;            // Clear interrupt flage IFG corresponding to pin P1.3
  // fall asleep:
  _BIS_SR(LPM4_bits + GIE);  // Enter Low Power Mode 4 w/interrupt
  
  // upon waking up:
  delay(debounce_delay);           // delay for approximately 250ms, longer than duration of switch bounce
  Initialize_Pins();               // Initialize I/O pins after sleep mode
  Output_to_LED(fadeValue,LED);    // Light the LED at the previous brightness level
  // wait for user to release the wake-up switch:
  while (digitalRead(sleep_switch) == LOW) {
  }
}

// Interrupt service routine that executes when pin 5 transitions from HIGH to LOW:
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)  {
  __bic_SR_register_on_exit(LPM4_bits+GIE); // Resume execution in Active Mode
}
  

