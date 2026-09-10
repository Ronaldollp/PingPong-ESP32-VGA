#ifndef _BALL_H_
#define _BALL_H_
#include <ESP32S3VGA.h>
#include "GameContext.h"

/*
 Represents the game ball. Its movement and bouncing depend on
 gc.mode (resolution) and gc.marg (playfield margin), which is why
 it holds a reference to the GameContext 
*/
class Ball{
    private:
        int x, y, vx, vy;
        GameContext& gc;
        int size = 2; // side length of the square that gets drawn (ball is 2x2 px)
    public: 
        Ball(GameContext&, int, int, int, int);
        void draw();
        void move();
        void setVx();
        int getVx();
        void setX(int);
        int getX(){ return x; };
        int getY(){ return y; };
        
};

#endif
