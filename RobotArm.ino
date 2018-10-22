#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PWMServoDriver.h>
#include <Bounce2.h>

#define BASE 0
#define ARM_FIRST 3
#define MAX_ARM_FIRST 425
#define MIN_ARM_FIRST 260

#define ARM_SECOND 8
#define ARM_THREE 15
#define HAND 4
#define FINGER 11
#define SAFE_DIST 20
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
int v_servo_position[16] = {310, 310, 310, 340, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310, 310};
int v_servo_position_reset[16] = {310, 310, 310, 340, 310, 310, 310, 310, 330, 310, 310, 310, 310, 310, 310, 310};

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
  display.setCursor(27, 30);
  display.print(msg);
  display.display();
}

void resetArm() {
  // Init Serial port output
  displayString(F("Resetting arm"));

  // Loop for every servos
  for (int i = 0; i < 16; i++) {
    pwm.setPWM(i, 0, v_servo_position_reset[i]);
    v_servo_position[i]=v_servo_position_reset[i];
    delay(100);
  }
 
  displayString(F("Resetted Arm"));

}

void moveArmFirstSegment(int move_to) {
  // Avoid too much move 
  if(move_to > MAX_ARM_FIRST) {
    move_to = MAX_ARM_FIRST;
  }
  if(move_to < MIN_ARM_FIRST) {
    move_to = MIN_ARM_FIRST;
  }
  moveServo(ARM_FIRST,move_to);
}
void moveArmSecondSegment(int move_to) {
  moveServo(ARM_SECOND,move_to);
}
void moveArmThirdSegment(int move_to) {
  moveServo(ARM_THREE,move_to);
}
void rotateBase(int move_to) {
  moveServo(BASE,move_to);
}
void rotateHand(int move_to) {
  moveServo(HAND,move_to);
}
void closeFinger() {
  //
  moveServo(FINGER,1);
}
void openFinger() {
  moveServo(FINGER,1);
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
      v_servo_position[n_servo]= move_to - 20;
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
      v_servo_position[n_servo]= move_to + 20;
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
    if ( buttons[i].fell() ) {
      display.clearDisplay();
      display.setCursor(27, 30);
      if (i == 1) {
        //Button one is pressed!
        displayString("Premuto 1+");
         displayString(String(v_servo_position[3]));
        pwm.setPWM(3, 0, v_servo_position[3]);
        v_servo_position[3] = v_servo_position[3] + 5;
      }
      if (i == 2) {
        //Button 2 is pressed!
        displayString("Premuto 2-");
        displayString(String(v_servo_position[3]));
        pwm.setPWM(3, 0, v_servo_position[3]);
        v_servo_position[3] = v_servo_position[3] - 5;
      }
      if (i == 3) {
        //Button 3 is pressed!
        displayString("Premuto 3");
        moveServo(0, 430);
        moveServo(3, 430);
        moveServo(8, 410);
        moveServo(15, 400);
        moveServo(4, 410);
        moveServo(11, 200);

      }
      if (i == 4) {
        //Button 4 is pressed!
        displayString("Premuto 4");
        resetArm();

      }
      if (i == 5) {
       displayString("Premuto 5");
        moveServo(0, 280);
        moveServo(3, 280);
        moveServo(8, 250);
        moveServo(15, 280);
        moveServo(4, 280);
        moveServo(11, 400);

      }
      if (i == 6) {
        //Button 6 is pressed! 
        displayString("Premuto 6");
        moveServo(8, 510); //ok

        moveServo(8, 250);
        moveServo(15, 500); //ok

        moveServo(15, 240);

      }
      if (i == 7) {
        //Button 7 is pressed! 
        displayString("Premuto 7");
        moveServo(4, 270);

        moveServo(4, 430);

      }
      if (i == 8) {
        //Button 8 is pressed! 
        displayString("Premuto 8");
        moveAll();
      }
      display.display();
    }
  }

}
