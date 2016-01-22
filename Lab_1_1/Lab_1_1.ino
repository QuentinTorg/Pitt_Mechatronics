

//////////   Hardware setup  /////////////////////////////////

// pin2 --> NO switch --> ground
// pin3 --> NO switch --> ground
// pin4 --> 330ohm --> LED long leg --> LED short leg --> ground
// pin5 --> 330ohm --> LED long leg --> LED short leg --> ground


//////////  Software  /////////////////////////////////////////

// set pin variables for simplicity
byte input_pin_1 = 2;
byte input_pin_2 = 3;
byte LED_pin_1 = 4;
byte LED_pin_2 = 5;

void setup() {
  // set pin modes for all input and output pins

  // inputs with pullup resistor to pull them high when disconnected from ground
  pinMode(input_pin_1, INPUT_PULLUP);
  pinMode(input_pin_2, INPUT_PULLUP);
  
  // outputs
  pinMode(LED_pin_1, OUTPUT);
  pinMode(LED_pin_2, OUTPUT);
}

void loop() {
  // set the state of each LED pin to the state of its respective input pin
  digitalWrite(LED_pin_1, !digitalRead(input_pin_1));
  digitalWrite(LED_pin_2, !digitalRead(input_pin_2));
}
