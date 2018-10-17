#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PWMServoDriver.h>
#include <Bounce2.h>

// DISPLAY: OLED display TWI address
#define OLED_ADDR   0x3C

// BUTTONS: Max & Min
#define NUM_BUTTONS 8
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7, 8, 9};
Bounce * buttons = new Bounce[NUM_BUTTONS];

// Init library
Adafruit_SSD1306 display(-1);
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// Status posizione servo
int v_servo = 310;
int v_servo_position[16] = {310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310};

void setup() {
  // Init Serial port output
  Serial.begin(9600);

  // Init Bottoni
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach( BUTTON_PINS[i] , INPUT_PULLUP  );       //setup the bounce instance for the current button
    buttons[i].interval(25);              // interval in ms
  }

  pwm.begin();
  Serial.println(F("PWM: Set freq to 60 Hz"));
  pwm.setPWMFreq(60);

  // initialize and clear display

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();

  // display a line of text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  displayString(F("Booting Ale!"));
  resetArm();
}

void displayString(String msg) {
  display.clearDisplay();
  display.setCursor(27, 30);
  display.print(msg);
  display.display();
}

void resetArm() {
  // Init Serial port output
  Serial.println(F("Resetting arm"));
  displayString(F("Resetting arm"));

  // Init Bottoni
  for (int i = 0; i < 16; i++) {
    Serial.print("Resetting ");
    Serial.println(i);
    pwm.setPWM(i, 0, 310);
    v_servo_position[i]=310;
    delay(100);
  }
  Serial.println(F("Resetted Arm"));
  displayString(F("Resetted Arm"));

}

void moveServo(int n_servo, int move_to) {
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
  Serial.println("Move arms");
  displayString(F("Move arms"));

  // Init Bottoni
  for (int i = 0; i < 16; i++) {
    Serial.print("Moving ");
    Serial.println(i);
    moveServo(i, 350);
    delay(10);
  }
  Serial.println("Moved arms");
  displayString(F("Moved arms"));

}
void loop() {

  // put your main code here, to run repeatedly:
  for (int i = 0; i < NUM_BUTTONS; i++)  {
    // Update the Bounce instance :
    buttons[i].update();
    // If it fell, flag the need to toggle the LED
    if ( buttons[i].fell() ) {
      display.clearDisplay();
      display.setCursor(27, 30);
      if (i == 1) {
        //Button one is pressed!
        Serial.println("Premuto 1+ ");
        Serial.println(v_servo);
        display.print("Premuto 1+");
        pwm.setPWM(0, 0, v_servo);
        v_servo = v_servo + 5;
      }
      if (i == 2) {
        //Button one is pressed!
        Serial.println("Premuto 2-");
        Serial.println(v_servo);
        display.print("Premuto 2-");
        pwm.setPWM(0, 0, v_servo);
        v_servo = v_servo - 5;
      }
      if (i == 3) {
        //Button one is pressed!
        Serial.println("3:Move 0 to 430");
        display.print("3:Move 0 to 430");
        moveServo(0, 430);
        moveServo(3, 430);
        moveServo(8, 410);
        moveServo(15, 400);
        moveServo(4, 410);
        moveServo(11, 200);

      }
      if (i == 4) {
        //Button one is pressed!
        Serial.println("Premuto Reset");
        display.print("Premuto Reset");
        resetArm();

      }
      if (i == 5) {
        Serial.println("5:Move 0 to 280");
        display.print("5: Move 0 to 280");
        moveServo(0, 280);
        moveServo(3, 280);
        moveServo(8, 250);
        moveServo(15, 280);
        moveServo(4, 280);
        moveServo(11, 400);

      }
      if (i == 6) {
        //Button one is pressed! Servo uno
        Serial.println("Premuto 6");
        display.print("Premuto 6");
        moveServo(8, 510); //ok

        moveServo(8, 250);
        moveServo(15, 500); //ok

        moveServo(15, 240);

      }
      if (i == 7) {
        //Button one is pressed! Servo uno --> ok
        Serial.println("Premuto 7");
        display.print("Premuto 7");
        moveServo(4, 270);

        moveServo(4, 430);

      }
      if (i == 8) {
        //Button one is pressed! Servo uno
        Serial.println("Premuto 8");
        display.print("Premuto 8");
        moveAll();
      }
      display.display();
    }
  }

}