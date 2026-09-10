#include <ESP32S3VGA.h>
#include "player.h"

Player::Player(GameContext& gc, int x, int y, int width,int len): paddle(gc, x, y, width, len), score(0){}

void Player::handleInput(int dir){
    paddle.move(dir);
}

void Player::addPoint(){
    score+=1;
}

int Player::getScore(){
    return score;
}

void Player::resetScore(){
    score = 0;
}
