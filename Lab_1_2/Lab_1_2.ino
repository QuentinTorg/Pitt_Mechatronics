//////////   Hardware setup  /////////////////////////////////
// setup works for Arduino nano, uno, pro mini, micro, and Mega

// pin2 --> NO switch --> ground
// pin3 --> NO switch --> ground
// pin4 --> 330ohm --> LED long leg --> LED short leg --> ground
// pin5 --> 330ohm --> LED long leg --> LED short leg --> ground
// pin6 --> 330ohm --> LED long leg --> LED short leg --> ground
// pin7 --> 330ohm --> LED long leg --> LED short leg --> ground

// pin 4 will be the lowest binary digit and pin 7 the highest

//Hi Quentin

//////////  Software  /////////////////////////////////////////

// set pin variables for simplicity
byte input_pin_1 = 2;
byte input_pin_2 = 3;
byte LED_pin_1 = 4;
byte LED_pin_2 = 5;
byte LED_pin_3 = 6;
byte LED_pin_4 = 7;

unsigned long last_count_time = 0;
unsigned long last_reverse_time = 0;

// count and direction variables
volatile byte button_count = 0;
volatile int count_direction = 1;

// interrupt debounce handling variables
int debounce_time = 50;

void setup() {
  // set pin modes for all input and output pins
  // inputs with pullup resistor to pull them high when disconnected from ground
  pinMode(input_pin_1, INPUT_PULLUP);
  pinMode(input_pin_2, INPUT_PULLUP);

  //attach an interrupt to both pins that runs when they are low
  attachInterrupt(digitalPinToInterrupt(input_pin_1), count, LOW);
  attachInterrupt(digitalPinToInterrupt(input_pin_2), rverse, LOW);

  // outputs
  pinMode(LED_pin_1, OUTPUT);
  pinMode(LED_pin_2, OUTPUT);
  pinMode(LED_pin_3, OUTPUT);
  pinMode(LED_pin_4, OUTPUT);
}

void loop() {
  // turn the LEDs on or off based on the value of their respective bit
  digitalWrite(LED_pin_1, bitRead(button_count, 0));
  digitalWrite(LED_pin_2, bitRead(button_count, 1));
  digitalWrite(LED_pin_3, bitRead(button_count, 2));
  digitalWrite(LED_pin_4, bitRead(button_count, 3));
}

// the count ISR is run when pin 2 is low
void count() {
  //debounce code here
  if (millis() - last_count_time > debounce_time) {
    button_count += count_direction;
    if (button_count == 16) {
      button_count = 0;
    }
    else if (button_count == 255) {
      button_count = 15;
    }
    last_count_time = millis(); // reset the debounce timer
  }
}


// the reverse ISR is run when pin 3 is low
void rverse() {
  // only run this code if it has been longer than the debounce time
  if (millis() - last_reverse_time > debounce_time) {
    // increment the button_count variable, and control the overflow with an if statement
    count_direction *= -1;
    last_reverse_time = millis(); // reset the debounce timer
  }
}

