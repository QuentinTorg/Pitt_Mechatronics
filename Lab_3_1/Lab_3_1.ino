/////////// hardware ///////////////
/*

  When running, disconnect arduino from computer.  Could cause power error on computer
  Set power supply somewhere between 7.5 and 10 volts.  Start at 7.5 and go up if stepper doesn't work
  Power supply should only be connected to arduino Vin, NOT VCC!!!!!!!!!

  *** to reverse the motor's direction, switch motor wire1 with motor wire2
  
  pin9 --> Hbridge M1 Enable
  pin2 --> Hbridge M1 Forward
  pin3 --> Hbridge M1 Reverse

  power supply ground --> Hbridge ground
  power supply Positive --> Hbridge Motor Power IN
  power supply Positive --> Arduino Vin

  Arduino Vcc --> Hbridge +5V
  Arduino Ground --> Hbridge Ground

  Arduino Vcc --> Potentiometer outer pin1
  Arduino Ground --> Potentiometer outer pin2
  pinA0 --> Potentiometer middle pin

  ***
  Motor wire1 --> Hbridge M1 output pin
  Motor wire2 --> Hbridge M1 output pin
  ***
  
*/

// Software

byte motor_enable_pin = 9;
byte motor_direction_pin_1 = 3;
byte motor_direction_pin_2 = 2;

byte sensor_pin = A0;

void setup() {

  // Set pinmode of each output pin
  pinMode(motor_enable_pin, OUTPUT);
  pinMode(motor_direction_pin_1, OUTPUT);
  pinMode(motor_direction_pin_2, OUTPUT);
}

void loop() {
  // calculate setpoint in number of steps using float_map function
  drive_motor(map((float)analogRead(sensor_pin), 0, 1023, -255, 255));
}


void drive_motor(int motor_output) {
  // write duty cycle for the motor
  analogWrite(motor_enable_pin, abs(motor_output));
  
  // write direction pins for motor
  digitalWrite(motor_direction_pin_1, (motor_output >= 0 ? 0 : 1));
  digitalWrite(motor_direction_pin_2, !(motor_output >= 0 ? 0 : 1));
}
