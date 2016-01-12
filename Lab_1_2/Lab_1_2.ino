//////////   Hardware setup  /////////////////////////////////
// setup works for Arduino nano, uno, pro mini, micro, and Mega

// pin2 --> NO switch --> ground
// pin3 --> NO switch --> ground
// pin4 --> 330ohm --> LED --> ground
// pin5 --> 330ohm --> LED --> ground
// pin6 --> 330ohm --> LED --> ground
// pin7 --> 330ohm --> LED --> ground

// pin 4 will be the lowest binary digit and pin 7 the highest


//////////  Software  /////////////////////////////////////////

// set pin variables for simplicity
byte input_pin_1 = 2;
byte input_pin_2 = 3;
byte LED_pin_1 = 4;
byte LED_pin_2 = 5;
byte LED_pin_3 = 6;
byte LED_pin_4 = 7;

// count and direction variables
volatile byte button_count = 0;
volatile int count_direction = 1;

// interrupt debounce handling variables
int debounce_time = 20;
unsigned long count_interrupt_time = 0;
unsigned long reverse_interrupt_time = 0;

void setup() {
  // set pin modes for all input and output pins
  // inputs with pullup resistor to pull them high when disconnected from ground
  pinMode(INPUT_PULLUP, input_pin_1);
  pinMode(INPUT_PULLUP, input_pin_2);
  //attach an interrupt to both pins that runs when they are low
  attachInterrupt(digitalPinToInterrupt(input_pin_1), count, LOW);
  attachInterrupt(digitalPinToInterrupt(input_pin_2), reverse, LOW);
  
  // outputs
  pinMode(OUTPUT, LED_pin_1);
  pinMode(OUTPUT, LED_pin_2);
  pinMode(OUTPUT, LED_pin_3);
  pinMode(OUTPUT, LED_pin_4);
}

void loop() {
  // turn the LEDs on or off based on the value of their respective bit
  digitalWrite(LED_pin_1, bitRead(button_count, 1));
  digitalWrite(LED_pin_2, bitRead(button_count, 2));
  digitalWrite(LED_pin_3, bitRead(button_count, 3));
  digitalWrite(LED_pin_4, bitRead(button_count, 4));
}

// the count ISR is run when pin 2 is low
void count() {
  //debounce code here
  if (millis() - count_interrupt_time < debounce_time) {
    // increment the button_count variable, and control the overflow with an if statement
    button_count += count_direction;
    if (button_count == 16) {
      button_count = 0;
    }
    else if (button_count == 255){
      button_count = 15;
    }
  }
  count_interrupt_time = millis();
  while (!digitalRead(input_pin_1)){}
  
  }
}

// the reverse ISR is run when pin 3 is low
void reverse() {
  if (millis() - reverse_interrupt_time < debounce_time) {
    // increment the button_count variable, and control the overflow with an if statement
    count_direction *= -1;
  }
  reverse_interrupt_time = millis();
  while (!digitalRead(input_pin_2)){}
}
