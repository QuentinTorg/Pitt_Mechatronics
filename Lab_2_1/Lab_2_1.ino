//////////   Hardware setup  /////////////////////////////////

// pin2 --> 330ohm --> LED long leg --> LED short leg --> ground
// pin3 --> 330ohm --> LED long leg --> LED short leg --> ground
// pin4 --> 330ohm --> LED long leg --> LED short leg --> ground
// pin5 --> 330ohm --> LED long leg --> LED short leg --> ground
// pin6 --> 330ohm --> LED long leg --> LED short leg --> ground

// pinA0 --> middle potentiometer pin
// 5V --> outer potentiometer pin 1
// outer potentiometer pin 2 --> ground


//////////  Software  /////////////////////////////////////////

// set pin variables for simplicity
byte LED_pin_1 = 2;
byte LED_pin_2 = 3;
byte LED_pin_3 = 4;
byte LED_pin_4 = 5;
byte LED_pin_5 = 6;
byte sense_pin = A0;

// set sensor and timing variables here
int min_delay = 1;
int max_delay = 500;

// test what the possible min and max sensor readings could be and insert
int min_sensor_value = 0;
int max_sensor_value = 1023;

// variables used for chaser
int chase_direction = 1;
int chase_LED_state = -1;
int delay_time;


void setup() {

  // uncomment the Serial.print lines to print sensor readings.  
  // Leaving them running will slow the code when running the chaser though
  //Serial.begin(9600);
  
  // outputs
  pinMode(LED_pin_1, OUTPUT);
  pinMode(LED_pin_2, OUTPUT);
  pinMode(LED_pin_3, OUTPUT);
  pinMode(LED_pin_4, OUTPUT);
  pinMode(LED_pin_5, OUTPUT);

  // set all pins low to start
  digitalWrite(LED_pin_1, LOW);
  digitalWrite(LED_pin_2, LOW);
  digitalWrite(LED_pin_3, LOW);
  digitalWrite(LED_pin_4, LOW);
  digitalWrite(LED_pin_5, LOW);
}

void loop() {
  // scale the sensor reading to the min and max delay times using the map() function
  delay_time = map(analogRead(sense_pin), min_sensor_value, max_sensor_value, min_delay, max_delay);

  // uncomment the Serial.print lines to print sensor readings.  
  // Leaving them running will slow the code when running the chaser though
  //Serial.print("sensor reading = ");
  //Serial.println(analogRead(sense_pin));

  digitalWrite(LED_pin_1 + chase_LED_state, LOW);
  
  chase_LED_state += chase_direction;
  if (chase_LED_state == -1 || chase_LED_state == 5) {
    chase_LED_state -= chase_direction * 2;
    chase_direction *= -1;
  }

  digitalWrite(LED_pin_1 + chase_LED_state, HIGH);
  
  delay(delay_time);
}
