#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H
#include <ESP32S3VGA.h>

// Groups the references almost every game class needs (the VGA
// screen, the active video mode, and the playfield margin), so they
// don't have to be passed separately to Ball, Paddle, and Player.
struct GameContext {
    VGA& vga;    // video controller, used for drawing (dot(), clear(), show()...)
    Mode& mode;  // active resolution (hRes, vRes) for the chosen VGA mode in the .ino
    int marg;    // playfield border margin in pixels (used for the board
                 // and as the bounce boundary for the ball/paddle)
};

#endif
