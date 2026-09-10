#include <ESP32S3VGA.h>
#include <vector>
#include <algorithm>
#include "GameContext.h"
#include "ball.h"
#include "paddle.h"
#include "player.h"

/* 
 VGA pin configuration for bitluni's ESP32-S3-VGA library.
 The -1 entries are unused color lines (running
 with fewer color bits than the mode's max).
 Actual wiring used in this project:
   GPIO 4  -> VGA pin 1 (R)
   GPIO 5  -> VGA pin 2 (G)
   GPIO 6  -> VGA pin 3 (B)
   GPIO 15 -> VGA pin 13 (HSync)
   GPIO 16 -> VGA pin 14 (VSync)
*/
const PinConfig pins(
  -1, -1, -1, -1, 4,
  -1, -1, -1, -1, -1, 5,
  -1, -1, -1, -1, 6,
  15, 16);

// 3x5 bitmap font (3 columns x 5 rows) used for the scoreboard.
// Each 3-bit row marks which pixels are on (1) or off (0).
// Indices 0-9   -> digits '0' to '9'
// Indices 10-15 -> letters used to spell "SCORE:" (S,C,O,R,E,:)
const uint8_t ALPHANUM[16][5] = {
    {0b010, 0b101, 0b101, 0b101, 0b010}, // 0
    {0b010, 0b110, 0b010, 0b010, 0b111}, // 1
    {0b111, 0b001, 0b111, 0b100, 0b111}, // 2
    {0b111, 0b001, 0b111, 0b001, 0b111}, // 3
    {0b101, 0b101, 0b111, 0b001, 0b001}, // 4
    {0b111, 0b100, 0b111, 0b001, 0b111}, // 5
    {0b111, 0b100, 0b111, 0b101, 0b111}, // 6
    {0b111, 0b001, 0b010, 0b010, 0b010}, // 7
    {0b111, 0b101, 0b111, 0b101, 0b111}, // 8
    {0b111, 0b101, 0b111, 0b001, 0b111},  // 9
    {0b111, 0b100, 0b111, 0b001, 0b111},  // S
    {0b111, 0b100, 0b100, 0b100, 0b111},  // C
    {0b111, 0b101, 0b101, 0b101, 0b111}, // O
    {0b111, 0b101, 0b110, 0b101, 0b101},  // R
    {0b111, 0b100, 0b111, 0b100, 0b111},  // E
    {0b000, 0b010, 0b000, 0b010, 0b000}  // :

};

VGA vga;
Mode mode = Mode::MODE_320x240x60;
GameContext context{vga, mode, 20}; // playfield margin: 20 px

const int pinAudio = 18;
const int pinUp = 1;
const int pinDown = 2; 

unsigned long lastFrame = 0;
const int INTER_FRAME = 16; // frame rate control: ~60fps (1000/60 ~= 16ms), with margin

Ball ball1(context, mode.hRes/2, mode.vRes/2, 4, 2);
Player player1(context, context.marg+3, mode.vRes /2 - 34, 3, 40);

const int BEEP_CHANNEL = 0; // LEDC channel dedicated to audio
bool beepActive = false;
unsigned long beepStart = 0;
unsigned long beepDuration = 0;

void setup() {
  context.vga.bufferCount = 2; // two buffers to avoid flicker
  pinMode(pinUp, INPUT_PULLUP);
  pinMode(pinDown, INPUT_PULLUP);
  ledcAttachPin(pinAudio, BEEP_CHANNEL); // LEDC drives the pin as PWM output

  if (!vga.init(pins, mode, 8)) {
    while (1) delay(1); // VGA init failed: intentionally hang here
  }
  context.vga.start();
}

void loop() {
  updateBeep();

  const unsigned int now = millis();
  if (now - lastFrame >= INTER_FRAME){
    lastFrame = now;

    // 1) INPUT READING
    int input = 0;
    if (digitalRead(pinUp) == LOW){
      input = 1;
    }else if(digitalRead(pinDown) == LOW){
      input = -1;
    }
    
    // 2) STATE UPDATE
    context.vga.clear();
    ball1.move();
    player1.handleInput(input);
    check_collision(ball1, player1.getPaddle());

    // 3) DRAW
    draw_board();
    ball1.draw();
    player1.draw();    
    draw_score(player1.getScore(), context.marg, context.marg*0.5);
    context.vga.show();
  }

}

// Draws the numeric scoreboard. 
void draw_score(int score, int x, int y){
  std::vector<int> digits{};

  // Draw the "SCORE:" label first (ALPHANUM indices 10-15)
  for(int i = 10; i < 16; i++){
    draw_char(i, x, y);
    x+=4;
  }

  // Extract the score's digits (reverse order: units first)
  while (score / 10 != 0){
    digits.push_back(score % 10);
    score /= 10;
  }
  digits.push_back(score);

  std::reverse(digits.begin(), digits.end()); // normal order (most significant first)

  for(int digit : digits){
    draw_char(digit, x, y);
    x += 4;
  }

}

// Draws the playfield border, offset from the screen edges by marg.
void draw_board(){
  for(int i = context.marg; i <= mode.hRes - context.marg - 1; i++){
    vga.dot(i, context.marg, 255, 255, 255);
    vga.dot(i, mode.vRes - context.marg - 1, 255, 255, 255);
  }
  for(int j = context.marg; j <= mode.vRes - context.marg - 1; j++){
    vga.dot(context.marg, j, 255, 255, 255);
    vga.dot(mode.hRes - context.marg - 1, j, 255, 255, 255);
  }
}

// Draws a single ALPHANUM (3x5) character at (x, y).
void draw_char(int character, int x, int y) {
    if (character < 0 || character > 15) {
        return;
    }
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 3; col++) {
            if ((ALPHANUM[character][row] >> (2 - col)) & 1) {
                vga.dot(x + col, y + row, 255, 255, 255);
            }
        }
    }
}

/* 
  Checks whether the ball has reached the paddle's zone and decides
  whether it was a hit (bounce + score point) or a miss (reset ball
  position and score).
*/
void check_collision(Ball& ball, Paddle& paddle){
  static bool resolved = false;

  if (ball.getX() <= paddle.getX() + paddle.getWidth()) {
    if (resolved) return; // this approach was already handled
    resolved = true;

    if(ball.getY() >= paddle.getY() && ball.getY() <= paddle.getY() + paddle.getLength()){
      // Hit: bounce and score a point
      ball.setVx();
      player1.addPoint();
      playBeep(pinAudio, 1000, 30); 
    }
    else{
      // Miss: reset the ball to center and reset the score
      ball.setX(mode.hRes / 2);
      ball.setVx();
      player1.resetScore();
      playBeep(pinAudio, 200, 100); 
    }
  } else {
    resolved = false; // ball moved away, ready for the next crossing
  }
}

/* 
 Starts a tone on pin at frequencyHz for durationMs.
 Uses LEDC to generate the tone and only starts a logical timer (beepStart/beepDuration), 
 the actual turn-off is handled by updateBeep(), called on every loop()
 iteration.
*/
void playBeep(int pin, int frequencyHz, int durationMs) {
    ledcWriteTone(BEEP_CHANNEL, frequencyHz);
    beepActive = true;
    beepStart = millis();
    beepDuration = durationMs;
}

// Turns the tone off once beepDuration has elapsed since it started.
void updateBeep() {
    if (beepActive && millis() - beepStart >= beepDuration) {
        ledcWriteTone(BEEP_CHANNEL, 0);
        beepActive = false;
    }
}
