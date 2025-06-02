const int x_js = A0;      // analog input, x-comp of xy-joystick (0~1023)
const int y_js = A1;      // analog input, y-comp of xy-joystick (0~1023)
const int z_js = A2;      // analog input, z-axis/z-rotate (only use VRY; 0~1023)
const int z_toggle = 48;  // z-axis/z-rotate toggle (H/L); use SW of z-joystick

const int x_pos = 22;        // +x mvmt digital output (H/L)
const int x_neg = 23;        // -x mvmt digital output (H/L)
const int y_pos = 24;        // +y mvmt digital output (H/L)
const int y_neg = 25;        // -y mvmt digital output (H/L)
const int z_trans_pos = 26;  // +z mvmt digital output (H/L)
const int z_trans_neg = 27;  // -z mvmt digital output (H/L)
const int z_ang_pos = 28;    // +z rotate digital output (H/L)
const int z_ang_neg = 29;    // -z rotate digital output (H/L)
const int x_pwm = 2;         // PWM digital x output
const int y_pwm = 3;         // PWM digital y output
const int z_trans_pwm = 4;   // PWM digital z translational output
const int z_ang_pwm = 5;     // PWM digital z angular output

const int left_solenoid_air = 50;
const int right_solenoid_air = 51;

// z_toggle setup
int newValue;
int oldValue;
int z_toggle_state = 0;  // Let 0 be z_translational and 1 be z_angular

// Solenoid setup
int left_solenoid_state = 0;                  // Let 0 be closed and 1 be open
int right_solenoid_state = 0;                 // Let 0 be closed and 1 be open
int left_deflate = 0;                         // Let 0 be inactive, 1 be inflating, and 2 be deflating
int right_deflate = 0;                        // Let 0 be inactive, 1 be inflating, and 2 be deflating
const unsigned long max_solenoid_time = 1000;  // Max inflating time - can change!
const unsigned long deflate_time = 500;       // Deflating time in between inflating - can change!
unsigned long left_deflate_start_time = 0;
unsigned long right_deflate_start_time = 0;
unsigned long left_start_time = 0;
unsigned long right_start_time = 0;

void setup() {
  Serial.begin(9600);
  pinMode(x_js, INPUT);
  pinMode(y_js, INPUT);
  pinMode(z_js, INPUT);
  pinMode(z_toggle, INPUT_PULLUP);  // Read 1 when button up and 0 when button down
  pinMode(x_pos, OUTPUT);
  pinMode(x_neg, OUTPUT);
  pinMode(y_pos, OUTPUT);
  pinMode(y_neg, OUTPUT);
  pinMode(z_trans_pos, OUTPUT);
  pinMode(z_trans_neg, OUTPUT);
  pinMode(z_ang_pos, OUTPUT);
  pinMode(z_ang_neg, OUTPUT);
  pinMode(x_pwm, OUTPUT);  // Might be unnecessary
  pinMode(y_pwm, OUTPUT);
  pinMode(z_trans_pwm, OUTPUT);
  pinMode(z_ang_pwm, OUTPUT);
  pinMode(left_solenoid_air, OUTPUT);
  pinMode(right_solenoid_air, OUTPUT);
}

bool in_deadzone(int value, int center = 512, int threshold = 50) { // Can change threshold!
  return ((value >= center - threshold) && (value <= center + threshold));
}

void loop() {
  int x_pos_value = analogRead(x_js);
  int y_pos_value = analogRead(y_js);
  int z_pos_value = analogRead(z_js);
  unsigned long time = millis();  // Used for solenoid timeout

  // Set z_toggle_state = 0 for z_translational and 1 for z_angular
  newValue = digitalRead(z_toggle);      // newValue = 1 when button up and 0 when button down
  if (newValue == 0 && oldValue == 1) {  // When button pressed
    if (z_toggle_state == 0) {           // Change from z_trans to z_ang
      digitalWrite(z_trans_pos, LOW);
      digitalWrite(z_trans_neg, LOW);
      z_toggle_state = 1;
      delay(100);
    } else if (z_toggle_state == 1) {  // Change from z_ang to z_trans
      digitalWrite(z_ang_pos, LOW);
      digitalWrite(z_ang_neg, LOW);
      z_toggle_state = 0;
      delay(100);
    }
  }
  oldValue = newValue;

  // Sets z_translational motor direction
  if (z_toggle_state == 0) {
    if (!in_deadzone(z_pos_value)) {
      if (z_pos_value < 462) {  // Move up
        digitalWrite(z_trans_pos, HIGH);
        digitalWrite(z_trans_neg, LOW);
      } else {  // Move down
        digitalWrite(z_trans_pos, LOW);
        digitalWrite(z_trans_neg, HIGH);
      }
    } else {  // Stay still
      digitalWrite(z_trans_pos, LOW);
      digitalWrite(z_trans_neg, LOW);
    }
  }

  // Sets z_angular motor direction
  if (z_toggle_state == 1) {
    if (!in_deadzone(z_pos_value)) {
      if (z_pos_value < 462) {  // Angle upwards
        digitalWrite(z_ang_pos, HIGH);
        digitalWrite(z_ang_neg, LOW);
      } else {
        digitalWrite(z_ang_pos, LOW);
        digitalWrite(z_ang_neg, HIGH);
      }
    } else {
      digitalWrite(z_ang_pos, LOW);
      digitalWrite(z_ang_neg, LOW);
    }
  }

  // Sets y motor direction
  if (!in_deadzone(y_pos_value)) {
    if (y_pos_value < 482) {  // Move forward
      digitalWrite(y_pos, HIGH);
      digitalWrite(y_neg, LOW);
    } else {  // Move backward
      digitalWrite(y_pos, LOW);
      digitalWrite(y_neg, HIGH);
    }
  } else {  // Stay still
    digitalWrite(y_pos, LOW);
    digitalWrite(y_neg, LOW);
  }

  // Assigns motor PWM values
  int tail_motor_pwr = map(abs(y_pos_value - 512), 0, 511, 0, 255);
  analogWrite(y_pwm, tail_motor_pwr);
  int z_trans_motor_pwr = map(abs(z_pos_value - 512), 0, 511, 0, 255);
  analogWrite(z_trans_pwm, z_trans_motor_pwr);
  int z_ang_motor_pwr = map(abs(z_pos_value - 512), 0, 511, 0, 255);
  analogWrite(z_ang_pwm, z_ang_motor_pwr);

  // Turn tail left/right
  if (!in_deadzone(x_pos_value)) {
    // Assume solenoids read LOW when closed and HIGH when open
    // UPDATE: solenoids read LOW when open and HIGH when open. Air flowing = air LOW, vent HIGH. Air venting = air HIGH, vent LOW. Tube closed = air LOW, vent LOW.
    // UPDATE: previous update is incorrect. Solenoids normally open. Now inflates for max_solenoid_time, deflates for deflate_time, & repeats
    // UPDATE: Solenoids normally closed. 
    // Inflating R moves tail L and fish turns L
    // UPDATE: Solenoids now normally closed
    if (x_pos_value < 462) {  // Move left
      // Turn off left solenoids first
      digitalWrite(left_solenoid_air, HIGH);  // May need to reverse depending on solenoid
      left_solenoid_state = 0;
      left_deflate = 0;
      if (right_deflate == 0) {
        digitalWrite(right_solenoid_air, LOW);  // May need to reverse depending on solenoid
        right_start_time = time;
        right_deflate = 1; 
        right_solenoid_state = 1;
      }
    } else {  // Move right
      // Turn off right solenoids first
      digitalWrite(right_solenoid_air, HIGH);  // May need to reverse depending on solenoid
      right_solenoid_state = 0;
      right_deflate = 0;
      if (left_deflate == 0) {
        digitalWrite(left_solenoid_air, LOW);  // May need to reverse depending on solenoid
        left_start_time = time;
        left_deflate = 1; 
        left_solenoid_state = 1;
      }
    }
  } else {                                   // Straight y-axis movement (tail neutral position)
    digitalWrite(left_solenoid_air, HIGH);   // May need to reverse depending on solenoid
    digitalWrite(right_solenoid_air, HIGH);  // May need to reverse depending on solenoid
    left_solenoid_state = 0;
    right_solenoid_state = 0;
    left_deflate = 0;
    right_deflate = 0;
  }

  // Enforce max solenoid time
  if (left_solenoid_state == 1 && (time - left_start_time >= max_solenoid_time)) {
    digitalWrite(left_solenoid_air, HIGH);  // May need to reverse depending on solenoid
    left_deflate_start_time = time;
    left_deflate = 2;
    left_solenoid_state = 0;
  }
  if (right_solenoid_state == 1 && (time - right_start_time >= max_solenoid_time)) {
    digitalWrite(right_solenoid_air, HIGH);  // May need to reverse depending on solenoid
    right_deflate_start_time = time;
    right_deflate = 2;
    right_solenoid_state = 0;
  }

  // Check deflate time
  if (left_deflate == 2 && (time - left_deflate_start_time >= deflate_time)) {
    left_deflate = 0;
    left_solenoid_state = 0;
  }
  if (right_deflate == 2 && (time - right_deflate_start_time >= deflate_time)) {
    right_deflate = 0;
    right_solenoid_state = 0;
  }
  Serial.print(left_solenoid_state); 
  Serial.print(left_deflate); 
  Serial.print(right_solenoid_state); 
  Serial.println(right_deflate); 
  delay(10);  // Slight delay for stability
}
