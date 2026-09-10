#ifndef _PADDLE_H_
#define _PADDLE_H_
#include <ESP32S3VGA.h>
#include "GameContext.h"

class Paddle {
    private:
        int x, y, length, width;
        GameContext& gc;

    public:
        Paddle(GameContext&, int, int, int, int);
        void draw();

        // d = 1  -> moves the paddle UP
        // d = -1 -> moves the paddle DOWN
        // d = 0  -> no input, doesn't move
        void move(int);

        int getX(){ return x; };
        int getY(){ return y; };
        int getLength(){ return length; };
        int getWidth(){ return width; }; 
};

#endif
