#include "paddle.h" 
#include "GameContext.h"

Paddle::Paddle(GameContext& gc, int x, int y, int width, int len): gc(gc), x(x), y(y), width(width),length(len){}

// Draws the paddle as a width x length rectangle at (x, y).
void Paddle::draw(){
    for(int i = 0; i < width; i++){
        for(int j = 0; j < length; j++){
            gc.vga.dot(x + i, y + j , 255, 255, 255);
        }
    }
}

void Paddle::move(int d){
    // Paddle speed: 7 px per input frame.
    if(d == 1) y -= 7;        // up
    else if (d == -1) y += 7; // down

    // Keeps the paddle inside the playfield (uses the field
    // margin, gc.marg, as the upper/lower bound).
    if (y < gc.marg) y = gc.marg;
    if(y > gc.mode.vRes - gc.marg - length ) y = gc.mode.vRes - gc.marg - length;
}
