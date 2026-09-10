#ifndef _PLAYER_H_
#define _PLAYER_H_
#include <ESP32S3VGA.h>
#include "GameContext.h"
#include "paddle.h"

// Player wraps a Paddle + its score.
class Player {
    private:
        Paddle paddle;
        int score = 0;

    public: 
        Player(GameContext&, int, int, int, int);

        // Forwards the input direction to the paddle (same meaning as
        // Paddle::move: 1 = up, -1 =        down, 0 = no input).
        void handleInput(int);

        void draw(){ paddle.draw();}
        void addPoint();
        int getScore();
        void resetScore();
        Paddle& getPaddle(){ return paddle;};
};

#endif
