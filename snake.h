#include <iostream>
#include <vector>
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#ifndef SNAKE_H
#define SNAKE_H
struct snakepart{
    int x,y;
    snakepart(int col, int row);
    snakepart();
};

class snakeclass{
    int points, minus_points, del;
    //indicates that the snake get food (it makes the snake longer)
    bool get_coin;
    bool get_posion;
    bool pass_gate;

    //indicates the current direction of the snake
    char direction;

    char partchar;
    char poison_items;
    char growth_items;
    char gate_items;

    snakepart coin;
    snakepart poison;
    snakepart gate_1;
    snakepart gate_2;
    std::vector<snakepart> snake; //represent the snake
    std::vector<snakepart> wallidx;
    void put_Coin();
    void put_Posion();
    void put_Gate();
    void passing_Gate();
    bool collision();
    void movesnake();

public:
    snakeclass();
    ~snakeclass();
    void start();
};

#endif
