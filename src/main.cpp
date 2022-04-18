#include <Arduino.h>
#include "set8mhz.h"
#include "MatrixController.h"
#include "game_config.h"

// for convenience when setting individual leds
#define ON true
#define OFF false

// pins for MatrixController
#define DATA_PIN 0
#define CLK_PIN 2
#define CS_PIN 1

// for readability
#define NUM_LEDS 64
#define SIDE_LEN 8

// analog pins for joystick
#define X_PIN A3
#define Y_PIN A2

// direction enum
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

// non-blocking interval timer - executes every "interval" ms
// https://eloquentarduino.github.io/2019/12/non-blocking-arduino-code/
#define EVERY_MS(interval) \
  static uint32_t __EVERY_MS__##interval = millis(); \
  if (millis() - __EVERY_MS__##interval >= interval \
      && (1 | (__EVERY_MS__##interval = millis()))) // edge case for millis() = 0

// convert (x, y) position to byte
#define POS_TO_BYTE(x,y) ((((x) << 4) | (y)))
// convert byte to x and y positions
#define BYTE_TO_X(b) ((uint8_t)(((uint8_t)b) >> 4))
#define BYTE_TO_Y(b) ((uint8_t)((b) & 0xF))
#define SET_LED_FROM_BYTE(b,s) (setLed(BYTE_TO_X(b), BYTE_TO_Y(b), (s)))

// Function prototypes

// Initialize game data and displays
void resetGame();
// Read joystick input and set next_dir
void updateDirection();
// Update the snake's position and check for food and collisions
void updateSnake();
// Get the next head position based on next_dir
uint8_t getNewHeadPos();
// Place food in a random location not occupied by the snake
// (resets if it fails to find a valid location)
void generateFood();
// Set single LED at the byte-encoded position
void setLedFromByte(uint8_t b, bool state);

// Resets the ATTiny85 board
void(* resetFunc) (void) = 0;

// Controller object for the 8x8 matrix
MatrixController mc(DATA_PIN, CLK_PIN, CS_PIN);

// snake data
uint8_t len; // length of body (including head)
uint8_t curr_dir; // current direction
uint8_t next_dir; // direction in next frame

// (x, y) position of each segment of snake (x is bits 7..4, y is 3..0)
// index 0 is head, len-1 is tail
uint8_t seg_pos[NUM_LEDS];

// food data
uint8_t food_pos; // current position of food
uint8_t food_on; // food led blinker

// fun start-up aniimation
void pulseXAnimation() {
  mc.clear();
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      if (x == y || 7-x == y) mc.setLed(y, x, ON);
    }
  }
  mc.show();
  for (int i = 0; i <= 15; i++) {
    mc.setIntensity(i);
    delay(12);
  }
  for (int i = 15; i >= 0; i--) {
    mc.setIntensity(i);
    delay(20);
  }
}

void setup() {
  SET_8MHZ; // set clock speed to 8MHZ
  mc.begin(); // init MatrixController
  // set pinMode for joystick pins
  pinMode(X_PIN, INPUT);
  pinMode(Y_PIN, INPUT);

  pulseXAnimation();
  
  mc.setIntensity(INTENSITY);

  resetGame();
}

void loop() {
  // read joystick input
  updateDirection();

  // update game frame
  EVERY_MS(FRAME_REFRESH_TIME) {
    updateSnake();
  }

  // quickly blink food pixel to differentiate it from the snake
  EVERY_MS(FOOD_BLINK_TIME) {
    // don't blink if head is on it
    if (seg_pos[0] != food_pos) {
      setLedFromByte(food_pos, 1 & food_on++);
      mc.show();
    }
  }
}

void resetGame() {
  len = INIT_LEN;
  curr_dir = INIT_DIR;
  next_dir = INIT_DIR;
  for (uint8_t i = 0; i < INIT_LEN; i++) {
    // currently only works for INIT_DIR = RIGHT
    seg_pos[i] = POS_TO_BYTE(INIT_HEAD_X - i, INIT_HEAD_Y);
  }
  // initialize display
  mc.clear();
  for (uint8_t i = 0; i < len; i++) {
    setLedFromByte(seg_pos[i], ON);
  }
  food_pos = POS_TO_BYTE(INIT_FOOD_X, INIT_FOOD_Y);
  food_on = 0;
  setLedFromByte(food_pos, ON);
  mc.show();
}

void updateDirection() {
  uint16_t vx = analogRead(X_PIN);
  uint16_t vy = analogRead(Y_PIN);
  // only change direction if new direction is perpendicular
  switch (curr_dir) {
    case UP:
    case DOWN:
      if (vx < LOWER) next_dir = LEFT;
      else if (vx > UPPER) next_dir = RIGHT;
      break;
    case LEFT:
    case RIGHT:
      if (vy < LOWER) next_dir = DOWN;
      else if (vy > UPPER) next_dir = UP;
      break;
  }
}

void updateSnake() {
  uint8_t head_pos = getNewHeadPos();
  curr_dir = next_dir;

  // check if snake ate some food
  if (head_pos == food_pos) {
    // leave tail on if food eaten (show length increase)
    len++;
    generateFood();
  } else {
    // check if snake hit itself
    if (mc.getLed(BYTE_TO_Y(head_pos), BYTE_TO_X(head_pos))) {
      resetFunc();
    }
    // turn off tail led if no food
    setLedFromByte(seg_pos[len-1], OFF);
  }

  // update seg_pos with updated values
  for (uint8_t i = len-1; i >= 1; i--) { // update all but head
    seg_pos[i] = seg_pos[i-1];
  }
  seg_pos[0] = head_pos; // update head

  setLedFromByte(head_pos, ON);
  mc.show();
}

uint8_t getNewHeadPos() {
  uint8_t head_x = BYTE_TO_X(seg_pos[0]);
  uint8_t head_y = BYTE_TO_Y(seg_pos[0]);

  // update head based on the next direction
  switch (next_dir) {
    case UP:
      if (head_y == SIDE_LEN-1) resetFunc();
      else head_y++;
      break;
    case DOWN:
      if (head_y == 0) resetFunc();
      else head_y--;
      break;
    case LEFT:
      if (head_x == 0) resetFunc();
      else head_x--;
      break;
    case RIGHT:
      if (head_x == SIDE_LEN-1) resetFunc();
      else head_x++;
      break;
  }
  return POS_TO_BYTE(head_x, head_y);
}

void generateFood() {
  if (len >= NUM_LEDS) return; // snake fills entire screen
  const uint16_t MAX_ITR = 1 << 9; // max number of tries before giving up
  uint16_t itr = 0; // iteration count
  uint8_t rand_pos;
  // break once a valid position is found
  while (true) {
    if (++itr > MAX_ITR) {
      // too many tries, give up
      resetFunc();
    }
    rand_pos = (random(SIDE_LEN) << 4) | random(SIDE_LEN); // random x and y
    // check if snake occupies the point
    bool pos_is_valid = true;
    for (uint8_t i = 0; i < len; i++) {
      if (seg_pos[i] == rand_pos) {
        pos_is_valid = false;
        break; // stop looking if we hit a snake segment
      }
    }
    if (!pos_is_valid) continue; // try again :/
    break; // success!
  }
  food_pos = rand_pos;
}

void setLedFromByte(uint8_t b, bool state) {
  mc.setLed(BYTE_TO_Y(b), BYTE_TO_X(b), state);
}
