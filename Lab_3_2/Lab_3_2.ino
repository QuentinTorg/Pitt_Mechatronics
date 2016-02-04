/////////// hardware ///////////////
/*

  When running, disconnect arduino from computer.  Could cause power error on computer
  Set power supply somewhere between 7.5 and 10 volts.  Start at 7.5 and go up if stepper doesn't work
  Power supply should only be connected to arduino Vin, NOT VCC!!!!!!!!!

  *** if your motor turns backwards, flip stepperCoil1 side1 with stepperCoil1 side2

  pin9 --> Hbridge M1 Enable
  pin10 --> Hbridge M2 Enable
  pin2 --> Hbridge M1 Forward
  pin3 --> Hbridge M1 Reverse
  pin4 --> Hbridge M2 Forward
  pin5 --> Hbridge M2 Reverse

  power supply ground --> Hbridge ground
  power supply Positive --> Hbridge Motor Power IN
  power supply Positive --> Arduino Vin

  Arduino Vcc --> Hbridge +5V
  Arduino Ground --> Hbridge Ground

  Arduino Vcc --> Potentiometer outer pin1
  Arduino Ground --> Potentiometer outer pin2
  pinA0 --> Potentiometer middle pin

  ***
  stepperCoil1 side1 --> Hbridge M1 output pin
  stepperCoil1 side2 --> Hbridge M1 output pin
  ***
  
  stepperCoil2 side1 --> Hbridge M2 output pin
  stepperCoil2 side2 --> Hbridge M2 output pin
*/

// Software

byte coil1_enable_pin = 9;
byte coil1_pin_1 = 3;
byte coil1_pin_2 = 2;

byte coil2_enable_pin = 10;
byte coil2_pin_1 = 4;
byte coil2_pin_2 = 5;

byte sensor_pin = A0;

// stepper specific
int steps_per_rev = 200;
float max_vel = 60; // speed in steps/sec
float max_accel = 350; // speed in steps/sec/sec

// used for calculating physics
float setpoint = 0;
float current_step;  // stepper angle in degrees
float current_vel = 0;  // used to remember current velocity
float current_accel = 0;
float x;
float step_size;
float last_time = 0.0;
float current_time = 0.0;
float elapsed_time = 0.0;
int coil1_duty = 0;
int coil2_duty = 0;



void setup() {

  // Set pinmode of each output pin
  pinMode(coil1_enable_pin, OUTPUT);
  pinMode(coil1_pin_1, OUTPUT);
  pinMode(coil1_pin_2, OUTPUT);

  pinMode(coil2_enable_pin, OUTPUT);
  pinMode(coil2_pin_1, OUTPUT);
  pinMode(coil2_pin_2, OUTPUT);

  // calculate starting step so that potentiometer is centered
  current_step = float_map((float)analogRead(sensor_pin), 0.0, 1023.0, -steps_per_rev / 2.0, steps_per_rev / 2.0);

}

void loop() {
  // calculate setpoint in number of steps using float_map function
  setpoint = float_map((float)analogRead(sensor_pin), 0.0, 1023.0, -steps_per_rev / 2.0, steps_per_rev / 2.0);

  // computes how long it has been since the last time through the code
  current_time = micros() / 1000000.0;
  elapsed_time = current_time - last_time;


  if ((current_step <= setpoint) && (current_vel >= 0)) {
    // if the stepper needs to move in the + direction and is moving in the + direction
    // start by assuming it should start slowing down to stop at the setpoint
    current_accel = -max_accel;
    // x = how long it takes to slow down to setpoint
    x = (current_vel * current_vel) / (2 * abs(current_accel));
    if (x < abs(current_step - setpoint)) {
      // if it can stop in time, then continue to accelerate
      current_accel *= -1;
    }
  }
  else if ((current_step > setpoint) && (current_vel < 0)) {
    // if the stepper needs to move in the - direction and is moving in the - direction
    // start by assuming it should start slowing down to stop at the setpoint
    current_accel = max_accel;
    // x = how long it takes to slow down to setpoint
    x = (current_vel * current_vel) / (2 * abs(current_accel));
    if (x < abs(current_step - setpoint)) {
      // if it can stop in time then continue to accelerate
      current_accel *= -1;
    }
  }
  else if ((current_step >= setpoint) && (current_vel >= 0)) {
    // if the stepper needs to move in the - direction but is moving in the + direction
    // accelerate towards - direction
    current_accel = -max_accel;
  }
  else if ((current_step < setpoint) && (current_vel < 0)) {
    // if the stepper needs to move in the + direction but is moving in the - direction
    // accelerate towards + direction
    current_accel = max_accel;
  }


  // calculate the step size since the last cycle
  step_size = current_vel * elapsed_time + .5 * current_accel * (elapsed_time * elapsed_time);

  // if the step size is more than a step, set it back to 1 step so missed steps don't occur
  if (step_size > 1) {
    step_size = 1;
  }
  else if (step_size < -1) {
    step_size = -1;
  }

  // set the new current step and velocity based on calculations
  current_step += step_size;
  current_vel += current_accel * elapsed_time;

  // set the new motor phase state based on current step
  set_phase(current_step);

  // set the current time to the last time for the next iteration
  last_time = current_time;
}


void set_phase(float phase) {
  // find the phase of the stepper.  every 4 steps is a period of a sine function, or 2PI
  phase = (phase - (float)((int)(phase / 4)) * 4) * PI / 2;

  // set the duty of each cycle depending on which portion of the phase it is in
  // this enables microstepping, or incrementing the stepper in segments smaller than a step
  coil1_duty = map((int)(sin(phase) * 1000), -1000, 1000, -255, 255);
  coil2_duty = map((int)(cos(phase) * 1000), -1000, 1000, -255, 255);

  // write duty cycle for each coil
  analogWrite(coil1_enable_pin, abs(coil1_duty));
  analogWrite(coil2_enable_pin, abs(coil2_duty));

  // write direction pins for coil 1
  digitalWrite(coil1_pin_1, (coil1_duty >= 0 ? 0 : 1));
  digitalWrite(coil1_pin_2, !(coil1_duty >= 0 ? 0 : 1));

  // write direction pins for coil 2
  digitalWrite(coil2_pin_1, (coil2_duty >= 0 ? 0 : 1));
  digitalWrite(coil2_pin_2, !(coil2_duty >= 0 ? 0 : 1));
}

// the float_map function allows floats to be passed into the map function instead of just ints
float float_map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


