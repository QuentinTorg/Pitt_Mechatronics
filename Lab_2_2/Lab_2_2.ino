//////////   Hardware setup  /////////////////////////////////

// pin11 --> 

// pinA0 --> middle potentiometer pin
// 5V --> outer potentiometer pin 1
// outer potentiometer pin 2 --> ground


//////////  Software  /////////////////////////////////////////

// set pin variables for simplicity
byte motor_pin = 10;
byte sense_pin = A0;

// test what the possible min and max sensor readings could be and insert
int min_sensor_value = 0;
int max_sensor_value = 1023;
int min_motor_speed = 0;
int max_motor_speed = 255;

byte motor_speed = 0;

void setup() {

  // uncomment the Serial.print lines to print sensor readings.  
  // Leaving them running will slow the code when running the chaser though
  Serial.begin(9600);
  
  // outputs
  pinMode(motor_pin, OUTPUT);

  // set all pins low to start
  digitalWrite(motor_pin, LOW);
}

void loop() {
  // scale the sensor reading to the min and max delay times using the map() function
  motor_speed = map(analogRead(sense_pin), min_sensor_value, max_sensor_value, min_motor_speed, max_motor_speed);

  // uncomment the Serial.print lines to print sensor readings.  
  // Leaving them running will slow the code when running the chaser though
  Serial.print("mtor_speed = ");
  Serial.println(motor_speed);

  analogWrite(motor_pin, motor_speed);
}
