

byte coil1_enable_pin = 9;
byte coil1_pin_1 = 2;
byte coil1_pin_2 = 3;

byte coil2_enable_pin = 10;
byte coil2_pin_1 = 4;
byte coil2_pin_2 = 5;

byte sensor_pin = A0;

int steps_per_rev = 200;
int max_speed = 50; // speed in steps/sec
int max_accel = 50; // speed in steps/sec/sec

float setpoint = 0;
float step_count = 0; // current step count from origin
float current_step;  // stepper angle in degrees
float current_vel = 0;  // used to remember current velocity
float current_accel = 0;  // used to remember current accel
long int last_time = 0;
long int current_time = 0;
int elapsed_time = 0;



void setup() {
  // put your setup code here, to run once:
  pinMode(coil1_enable_pin, OUTPUT);
  pinMode(coil1_pin_1, OUTPUT);
  pinMode(coil1_pin_2, OUTPUT);
  
  pinMode(coil2_enable_pin, OUTPUT);
  pinMode(coil2_pin_1, OUTPUT);
  pinMode(coil2_pin_2, OUTPUT);

  current_step = map(analogRead(sensor_pin), 0, 1023, -steps_per_rev, steps_per_rev)
  
}

void loop() {
  // put your main code here, to run repeatedly:
  setpoint = map(analogRead(sensor_pin), 0, 1023, -steps_per_rev, steps_per_rev)

  current_time = micros();
  elapsed_time = current_time - last_time;

  increment =
  if setpoint
  current_steps = 


  
  
}




void set_phase(float steps) {
  // find the phase of the stepper
  phase = (steps % 4) * PI / 2;

  // set the duty of each cycle depending on which portion of the phase it is in
  // this enables microstepping, or incrementing the stepper in segments smaller than a step
  coil1_duty = map(sin(phase), -1, 1, -255, 255);
  coil2_duty = map(cos(phase), -1, 1, -255, 255);

  // write duty cycle for each coil
  analogWrite(coil1_enable_pin, abs(coil1_duty));
  analogWrite(coil2_enable_pin, abs(coil2_duty));

  // write direction pins for coil 1
  digitalWrite(coil1_pin_1, (coil1_duty >= 0 ? 0:1));
  digitalWrite(coil1_pin_2, !(coil1_duty >= 0 ? 0:1));

  // write direction pins for coil 2
  digitalWrite(coil2_pin_1, (coil2_duty >= 0 ? 0:1));
  digitalWrite(coil2_pin_2, !(coil2_duty >= 0 ? 0:1));
}


