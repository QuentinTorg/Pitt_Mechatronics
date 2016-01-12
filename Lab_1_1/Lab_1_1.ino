//////////   Hardware setup  /////////////////////////////////

// pin2 --> NO switch --> ground
// pin3 --> NO switch --> ground
// pin4 --> 330ohm --> LED --> ground
// pin5 --> 330ohm --> LED --> ground




//////////  Software  /////////////////////////////////////////

// set pin variables for simplicity
byte input_pin_1 = 2;
byte input_pin_2 = 3;
byte LED_pin_1 = 4;
byte LED_pin_2 = 5;

void setup() {
  // set pin modes for all input and output pins

  // inputs with pullup resistor to pull them high when disconnected from ground
  pinMode(INPUT_PULLUP, input_pin_1);
  pinMode(INPUT_PULLUP, input_pin_2);
  
  // outputs
  pinMode(OUTPUT, LED_pin_1);
  pinMode(OUTPUT, LED_pin_2);
}

void loop() {
  // set the state of each LED pin to the state of its respective input pin
  digitalWrite(LED_pin_1, digitalRead(input_pin_1));
  digitalWrite(LED_pin_2, digitalRead(input_pin_2));
}