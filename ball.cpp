#include "ball.h"
#include "GameContext.h"

Ball::Ball(GameContext& gc, int x, int y, int vx, int vy): gc(gc), x(x), y(y), vx(vx), vy(vy){}

void Ball::move(){
    x += vx;
    y += vy;

    // Bounce off the RIGHT edge of the field.
    if (x >= gc.mode.hRes - gc.marg - size) {
        x = gc.mode.hRes - gc.marg - size;
        vx = -vx;
    }

    // Bounce off the TOP and BOTTOM edges of the field.
    if (y <= gc.marg) {
        y = gc.marg;
        vy = -vy;
    } else if (y >= gc.mode.vRes - gc.marg - size) {
        y = gc.mode.vRes - gc.marg - size;
        vy = -vy;
    }
}

// Draws the ball as a size x size square at (x, y).
void Ball::draw(){
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            gc.vga.dot(x+i, y+j, 255, 255, 255);
        }
    }
}

// Flips the horizontal direction.
void Ball::setVx(){
    vx = -vx;
}

int Ball::getVx(){
    return vx;
}

void Ball::setX(int x){
    this-> x = x;
}
