#include <iostream>
#include <vector>
#include <ncurses.h>
#include <cstdlib>
#include <ctime>

#ifndef SNAKE_H
#define SNAKE_H

struct snakepart {
    int x, y;

    snakepart(int col, int row);

    snakepart();
};

class snakeclass {
    int points, minus_points, del;
    int coin_timer, poison_timer, gate_timer;
    int mission_points, mission_minus, mission_gate;
    int mission_Level;
    bool get_coin;
    bool pass_gate;
    bool game_complete;
    char pass_char;
    char direction;
    char partchar;
    char poison_items;
    char growth_items;
    char gate_items;
    snakepart coin;
    snakepart poison;
    snakepart gate_1;
    snakepart gate_2;
    std::vector <snakepart> snake;
    std::vector <snakepart> wallidx;

    void put_Coin();
    void put_Posion();
    void put_Gate();
    void passing_Gate();
    void pass_Mission();
    bool collision();
    void movesnake();
    void read_map();

public:
    snakeclass();

    ~snakeclass();

    void start();
};

#endif
