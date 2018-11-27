#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PWMServoDriver.h>
#include <Bounce2.h>

// Map robot piece to PWM ports
#define BASE 0
#define ARM_FIRST 7
#define ARM_SECOND 15
#define ARM_THREE 12
#define HAND 8
#define FINGER 3

const uint8_t servos[6] = {BASE, ARM_FIRST, ARM_SECOND, ARM_THREE, HAND, FINGER}; // Numbers pwm port to servos

// Map safety parameters (avoid extreme displacement)
#define MAX_BASE 474
#define MIN_BASE 166
#define MAX_ARM_FIRST 425
#define MIN_ARM_FIRST 260
#define MAX_ARM_SECOND 560
#define MIN_ARM_SECOND 215
#define MAX_ARM_THREE 550
#define MIN_ARM_THREE 215
#define MAX_HAND 458
#define MIN_HAND 140
#define MAX_FINGER 350
#define MIN_FINGER 173
#define SAFE_DIST 20


// DISPLAY: OLED display TWI address
#define OLED_ADDR   0x3C

// BUTTONS: Max & Min
#define NUM_BUTTONS 4

// da sistemare
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {9, 10, 11, 12};
Bounce * buttons = new Bounce[NUM_BUTTONS];

// Init library
Adafruit_SSD1306 display(-1);
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
// Analog Force read
int fsrAnalogPin = A7; 
int fsrReading;    

// Status posizione servo
int selectedServo = 0;
int v_servo = 310;
//int v_servo_position[16] = {310, 310, 310, 340, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310};
//int v_servo_position_reset[16] = {310, 310, 310, 340, 310, 310, 310, 310, 330, 310, 310, 310, 310, 310, 310, 310};

const uint8_t servoPwmPort[6] = {BASE, ARM_FIRST, ARM_SECOND, ARM_THREE, HAND, FINGER}; // Numbers pwm port to servos
int v_servo_position[6] = {310, 340, 330, 310, 310, 310};
int v_servo_position_reset[6] = {277, 340, 340, 310, 310, 310};

void setup() {
  // Init Serial port output
  Serial.begin(9600);

  // Init Bottoni
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach( BUTTON_PINS[i] , INPUT_PULLUP  );       //setup the bounce instance for the current button
    buttons[i].interval(25);              // interval in ms
  }

  // initialize and clear display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();

  // display a line of text
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Init PWM Module
  pwm.begin();
  Serial.println(F("PWM: Set freq to 60 Hz"));
  pwm.setPWMFreq(60);

  //displayString(F("Booting Ale!"));
  resetArm();
}

void displayString(String msg) {
  Serial.println(msg);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(msg);
  display.display();
}

void resetArm() {
  // Init Serial port output
  displayString(F("Resetting arm"));

  // Loop for every servos
  for (int i = 0; i < 6; i++) {
    pwm.setPWM(servoPwmPort[i], 0, v_servo_position_reset[i]);
    v_servo_position[i] = v_servo_position_reset[i];
    delay(100);
  }

  displayString(F("Resetted Arm"));

}

void ArmAtRest() {
  // Init Serial port output
  displayString(F("Rest arm"));
  rotateBase(190);
  moveArmFirstSegment(280);
  moveArmSecondSegment(550);
  moveArmThirdSegment(500);
  rotateHand(150);
  rotateHand(440);
  displayString(F("Rested Arm"));

}
void moveArmFirstSegment(int move_to) {
  // Avoid too much move
  if (move_to > MAX_ARM_FIRST) {
    move_to = MAX_ARM_FIRST;
  }
  if (move_to < MIN_ARM_FIRST) {
    move_to = MIN_ARM_FIRST;
  }
  moveServo(ARM_FIRST, move_to);
}
void moveArmSecondSegment(int move_to) {
  moveServo(ARM_SECOND, move_to);
}
void moveArmThirdSegment(int move_to) {
  moveServo(ARM_THREE, move_to);
}
void rotateBase(int move_to) {
  moveServo(BASE, move_to);
}
void rotateHand(int move_to) {
  moveServo(HAND, move_to);
}
void closeFinger() {
  //
  for (int i = v_servo_position[5]; (i <= MAX_FINGER)or(fsrReading<1020) ; i = i + 1) {
      fsrReading=analogRead(fsrAnalogPin);
      Serial.println(fsrReading);
      moveServo(FINGER, i);
      delay(5);
    }
  
}
void openFinger() {
  moveServo(FINGER,MIN_FINGER+5);
}
void moveServo(int n_servo, int move_to) {
  // Log
  // displayString(("Moved to:"+ move_to + " for servo:" + n_servo));
  displayString(String(move_to));

  // Move to R
  if (v_servo_position[n_servo] <= move_to) {

    // Move fast to R
    if (move_to - v_servo_position[n_servo] > 20) {
      displayString("Move Fast R");
      pwm.setPWM(n_servo, 0, move_to - 20);
      v_servo_position[n_servo] = move_to - 20;
    }
    delay(5);
    // Move smooth to R
    for (int i = v_servo_position[n_servo]; i <= move_to ; i = i + 1) {
      displayString("Move Smooth R");
      pwm.setPWM(n_servo, 0, i);
      delay(5);
    }
  } else {
    // Move to L
    // Move fast to L
    if (v_servo_position[n_servo] - move_to > 20) {
      displayString("Move Fast L");
      pwm.setPWM(n_servo, 0, move_to + 20);
      v_servo_position[n_servo] = move_to + 20;
    }
    delay(5);
    for (int i = v_servo_position[n_servo]; i > move_to ; i = i - 1) {
      displayString("Move Smooth L");
      pwm.setPWM(n_servo, 0, i);
      delay(5);
    }
  }
  v_servo_position[n_servo] = move_to;

  delay(10);
  //displayString(("Moved to:"+ move_to + " for servo:" + n_servo));
}


void moveServo_old(int n_servo, int move_to) {
  // Init Serial port output
  Serial.println(F("Move Uno"));
  displayString(F("Move Uno"));

  // Init Bottoni
  if (v_servo_position[n_servo] <= move_to) {
    for (int i = v_servo_position[n_servo]; i <= move_to ; i = i + 1) {
      Serial.print(F("Moving..."));
      Serial.println(i);
      pwm.setPWM(n_servo, 0, i);
      delay(5);
    }
  } else {
    for (int i = v_servo_position[n_servo]; i > move_to ; i = i - 1) {
      Serial.print(F("Moving..."));
      Serial.println(i);
      pwm.setPWM(n_servo, 0, i);
      delay(5);
    }
  }
  v_servo_position[n_servo] = move_to;

  delay(10);

  Serial.print(F("Moved "));
  Serial.print(n_servo);
  Serial.print(F(" to "));
  Serial.println(v_servo_position[n_servo]);
  displayString(F("Moved Uno"));

}

void moveAll() {
  // Init Serial port output
  displayString(F("Move arms"));

  // Init Bottoni
  for (int i = 0; i < 16; i++) {
    Serial.print("Moving ");
    Serial.println(i);
    moveServo(i, 350);
    delay(10);
  }

  displayString(F("Moved arms"));

}
void loop() {
  
  // put your main code here, to run repeatedly:
  for (int i = 0; i < NUM_BUTTONS; i++)  {
    // Update the Bounce instance :
    buttons[i].update();
    // -------
    display.clearDisplay();
    if ( buttons[i].rose() ) {
      
      if (i == 0) {
        // Change part of the robot that will be moved
        display.setCursor(0, 0);
        selectedServo = (selectedServo + 1) % 6;
        display.println("Servo number: " + String(selectedServo));

      }
      if (i == 1) {
        // Move the selected arm + 1
        Serial.println("Servo number: " + String(selectedServo));
        Serial.println("Servo PWM Port: " + String(servoPwmPort[selectedServo]));
        Serial.println("Servo value: " + String(v_servo_position[selectedServo] + 1));
        display.setCursor(0, 0);
        display.println("Servo number: " + String(selectedServo));
        display.println("Servo value: " + String(v_servo_position[selectedServo] + 1));
        pwm.setPWM(servoPwmPort[selectedServo], 0, v_servo_position[selectedServo] + 1);
        v_servo_position[selectedServo] = v_servo_position[selectedServo] + 1;
      }
      if (i == 2) {
        // Move the selected arm - 1
        Serial.println("Servo number: " + String(selectedServo));
        Serial.println("Servo PWM Port: " + String(servoPwmPort[selectedServo]));
        Serial.println("Servo value: " + String(v_servo_position[selectedServo] - 1));
        display.setCursor(0, 0);
        display.println("Servo number: " + String(selectedServo));
        display.println("Servo value: " + String(v_servo_position[selectedServo] - 1));
        pwm.setPWM(servoPwmPort[selectedServo], 0, v_servo_position[selectedServo] - 1);
        v_servo_position[selectedServo] = v_servo_position[selectedServo] - 1;
      }
      if (i == 3) {
        // Run other program
        resetArm();
        delay(1000);
        ArmAtRest();

      }
      display.display();
    }
  
  }

}
