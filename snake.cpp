#include "snake.h"
#include "map.h"
#include <unistd.h>
using namespace std;

//const int maxwidth = 100;
//const int maxheight = 40;
const int game_width = 60;
const int game_height = 30;

//특정 Window를 나타내는 pointer data type
WINDOW *game_Board; //게임 칸
WINDOW *mission_Board; //미션 칸
WINDOW *score_Board; //점수 칸
int coin_timer = 0, poison_timer = 0, gate_timer = 0;


snakepart::snakepart(int col, int row) {
    x = col;
    y = row;
}

snakepart::snakepart() {
    x = 0;
    y = 0;
}


snakeclass::snakeclass() {
    initscr();
    nodelay(stdscr, true);
    keypad(stdscr, true);
    noecho(); //사용자로부터 입력 받은 문자 출력 X
    curs_set(0);//커서 가리기
    //terminal 크기 조정 Window까지 동시 조정가능
    //resize_term(row, col);
    //resize_term(maxwidth, maxheight);

    //색 사용 설정 전 무조건 선언해야 함
    start_color();

    //COLOR 설정 (숫자, 폰트 색, 폰트 배경색)
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    refresh(); //호출되기 전까지 수행했던 모든 작업들을 스크린에 업데이트한다.

    //새 윈도우 생성 (행 크기, 열 크기, 윈도우 시작 y좌표, 윈도우 시작 x좌표)
    game_Board = newwin(game_height, game_width, 0, 0); //게임보드 윈도우 사이즈와 포지션 설정
    //w가 붙는 이유는 특정 window를 위한 함수이기 때문이다.
    wattron(game_Board, COLOR_PAIR(1));
    wborder(game_Board, '@', '@', '@', '@', 'X', 'X', 'X', 'X');
    //윈도우 경계선 설정
    //인자는 ls, rs, ts, bs, tl, growthtr, bl, br 순
    //left side, right side, top side, bottom side, top left corner,
    // top right corner, bottom left corner, bottom right corner
    wattroff(game_Board, COLOR_PAIR(1));
    partchar = 'O';
    poison_items = 'X';
    growth_items = '$';
    gate_items = '#';
    coin.x = 0;
    coin.y = 0;
    poison.x = 0;
    poison.y = 0;
    gate_1.x = 0;
    gate_1.y = 0;
    gate_2.x = 0;
    gate_2.y = 0;
    for (int i = 0; i < 3; i++)
        snake.push_back(snakepart(40 + i, 10));
    points = 0, minus_points = 0;
    for(int i = 0; i < 30; i++){
        for(int j = 0; j < 60; j++){
            if(map[i][j] == 1)
                wallidx.push_back(snakepart(j,i));
        }
    }
    wattron(game_Board, COLOR_PAIR(1));
    for(int i = 0; i < wallidx.size(); i++){
        wmove(game_Board, wallidx[i].y, wallidx[i].x);
        waddch(game_Board, '@');
    }
    wattroff(game_Board, COLOR_PAIR(1));
    wrefresh(game_Board);
    //del = 110000;
    del = 100000; //0.5초
    get_coin = 0;
    direction = 'l';
    pass_gate = 0;
    srand(time(NULL));


//    int random_item = rand() % 2 + 1; // 1 or 2
//    if(random_item == 1) put_Coin();
//    else put_Posion();
    put_Coin();
    put_Posion();
    put_Gate();

    //draw the snake
    for (int i = 0; i < snake.size(); i++) {
        wmove(game_Board, snake[i].y, snake[i].x);
        waddch(game_Board, partchar);
    }

    wrefresh(game_Board);

    score_Board = newwin(10, 40, 0, 62); //스코어 보드 윈도우 사이즈와 포지션 설정
    //w가 붙는 이유는 특정 window를 위한 함수이기 때문이다.
    wbkgd(score_Board, COLOR_PAIR(2)); //스코어 보드 윈도우 백그라운드 적용
    wattron(score_Board, COLOR_PAIR(1));
    mvwprintw(score_Board, 1, 2, "score board");
    mvwprintw(score_Board, 2, 2, "B : %d", snake.size());
    mvwprintw(score_Board, 3, 2, "+ : 0");
    mvwprintw(score_Board, 4, 2, "- : 0");
    wborder(score_Board, '-', '-', '-', '-', '+', '+', '+', '+');
    wrefresh(score_Board);

    mission_Board = newwin(20, 40, 10, 62); //스코어 보드 윈도우 사이즈와 포지션 설정
    //w가 붙는 이유는 특정 window를 위한 함수이기 때문이다.
    wbkgd(mission_Board, COLOR_PAIR(2)); //스코어 보드 윈도우 백그라운드 적용
    wattron(mission_Board, COLOR_PAIR(1));
    mvwprintw(mission_Board, 1, 1, "This is mission board");
    wborder(mission_Board, '-', '-', '-', '-', '+', '+', '+', '+');
    wrefresh(mission_Board);
}

snakeclass::~snakeclass() {
    nodelay(stdscr, false);
    getch();
    delwin(game_Board);
    delwin(score_Board);
    delwin(mission_Board);
    endwin();
}

void snakeclass::put_Coin() {
    while (1) {
        int tmpx = rand() % game_width + 1; //x와 y 좌표를 game_board 사이즈 안에서 랜덤으로 출력하게 함
        int tmpy = rand() % game_height + 1;
        for (int i = 0; i < snake.size(); i++) //스네이크 사이즈만큼 돌려서
            if (snake[i].x == tmpx && snake[i].y == tmpy) //코인이 스네이크와 겹치지 않게
                continue;
        if (poison.x == tmpx && poison.y == tmpy) //코인이 독 아이템과 겹치지 않게
            continue;
        if(map[tmpy][tmpx] == 1 || map[tmpy][tmpx] == 7) //코인이 벽과 게이트랑 겹치지 않게
            continue;
        if (tmpx >= game_width - 2 || tmpy >= game_height - 3)
            continue;
        coin.x = tmpx; //코인에 성공적인 좌표를 할당
        coin.y = tmpy;
        map[tmpy][tmpx] = 5;
        break;
    }
    wattron(game_Board, COLOR_PAIR(2)); //코인에다가 2번 색깔
    wmove(game_Board, coin.y, coin.x); //성공적인 좌표에 좌표 이동
    waddch(game_Board, growth_items); //좌표에 캐릭터형 변수 넣어준다.
    wattroff(game_Board, COLOR_PAIR(2));

    wrefresh(game_Board);
}

void snakeclass::put_Posion() {
    while (1) {
        int tmpx = rand() % game_width + 1;
        int tmpy = rand() % game_height + 1;
        for (int i = 0; i < snake.size(); i++)
            if (snake[i].x == tmpx && snake[i].y == tmpy) //독 아이템이 스네이크와 겹치지 않게
                continue;
        if (coin.x == tmpx && coin.y == tmpy) //독 아이템이 코인과 겹치지 않게
            continue;
        if(map[tmpy][tmpx] == 1 || map[tmpy][tmpx] == 7) //코인이 벽과 게이트랑 겹치지 않게
            continue;
        if (tmpx >= game_width - 2 || tmpy >= game_height - 3)
            continue;
        poison.x = tmpx;
        poison.y = tmpy;
        map[tmpy][tmpx] = 6;
        break;
    }
    wattron(game_Board, COLOR_PAIR(3));
    wmove(game_Board, poison.y, poison.x);
    waddch(game_Board, poison_items);
    wattroff(game_Board, COLOR_PAIR(3));

    wrefresh(game_Board);
}

void snakeclass::put_Gate(){

    while(1){
        int gate1_idx = rand() % (wallidx.size());
        int gate2_idx = rand() % (wallidx.size());
        if(gate1_idx == gate2_idx){//2개의 게이트 x,y좌표가 똑같지 않게
          continue;
        }
        gate_1.x = wallidx[gate1_idx].x;
        gate_1.y = wallidx[gate1_idx].y;
        gate_2.x = wallidx[gate2_idx].x;
        gate_2.y = wallidx[gate2_idx].y;
        map[wallidx[gate1_idx].y][wallidx[gate1_idx].x] = 7;
        map[wallidx[gate2_idx].y][wallidx[gate2_idx].x] = 7;

        break;
        //map 배열에 1값이 저장되어있는 인덱스를 골라서 그중에서 랜덤으로 선택
    }
    wattron(game_Board, COLOR_PAIR(4));
    wmove(game_Board, gate_1.y, gate_1.x);
    waddch(game_Board,'1');  //gate items
    wattroff(game_Board, COLOR_PAIR(4));//gate1

    wattron(game_Board, COLOR_PAIR(4));
    wmove(game_Board, gate_2.y, gate_2.x);
    waddch(game_Board, '2');
    wattroff(game_Board, COLOR_PAIR(4));//gate2
    wrefresh(game_Board);

}


bool snakeclass::collision() {
    //벽에 부딪히면
    if(map[snake[0].y][snake[0].x] == 1){
        return true;
    }
    if(map[snake[0].y][snake[0].x] == 7){ //게이트로가면
        if(snake[0].x == gate_1.x && snake[0].y == gate_1.y){ // gate1에 들어가면
            //기능 수행, 나오는곳은 gate2, gate2 의 위치에따라 4분할 (상단,하단,좌측,우측)
            if(gate_2.y == 0) {//벽이 상단에 있을때
                snake[0].x = gate_2.x;
                snake[0].y = 0; // 아랫방향
                direction = 'd';
            }
            else if(gate_2.x == 0){ //벽이 좌측에 있을때
                snake[0].x = 0;
                snake[0].y = gate_2.y;
                direction = 'r';
            }
            else if(gate_2.y == 29){ //벽이 하단에 있을때
                snake[0].x = gate_2.x;
                snake[0].y = 29;
                direction = 'u';
            }
            else if(gate_2.x == 59){ //벽이 우측에 있을때
                snake[0].x = 59;
                snake[0].y = gate_2.y;
                direction = 'l';
            }
            else{ //벽이 맵 중앙에 있을때
                if(direction == 'l'){ //진출방향이 왼쪽일때 , 왼쪽 -> 시계(위) -> 반시계(아래) -> 역방향(오)
                    // int arrow[4][2] = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}};
                    // for(int i =0; i < 4; i++){
                    //     if(!map[gate_2.y + arrow[i][0]][gate_2.x+arrow[i][1]]){
                    //         snake[0].x = gate_2.x+arrow[i][1];
                    //         snake[0].y = gate_2.y+arrow[i][0];
                    //         break;
                    //     }
                    // }

                    if(map[gate_2.y][gate_2.x-1] != 1){ //왼
                        //snake[0].x = gate_2.x-1;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;
                    }
                    else if(map[gate_2.y-1][gate_2.x] != 1){ //위
                        snake[0].x = gate_2.x;
                        //snake[0].y = gate_2.y-1;
                        snake[0].y = gate_2.y;
                        direction = 'u';
                    }
                    else if(map[gate_2.y+1][gate_2.x] != 1){ //아래
                        snake[0].x = gate_2.x;
                        //snake[0].y = gate_2.y+1;
                        snake[0].y = gate_2.y;
                        direction = 'd';
                    }
                    else if(map[gate_2.y][gate_2.x+1] != 1){ //오른쪽
                        //snake[0].x = gate_2.x+1;
                        //snake[0].y = gate_2.y;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;
                        direction = 'r';
                    }
                }
                else if(direction == 'u'){//위 -> 오른쪽 -> 왼쪽 -> 아
                    if(map[gate_2.y-1][gate_2.x] != 1){ //위
                        //snake[0].x = gate_2.x;
                        //snake[0].y = gate_2.y-1;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;

                    }
                    else if(map[gate_2.y][gate_2.x+1] != 1){ //오른쪽
                        //snake[0].x = gate_2.x+1;
                        //snake[0].y = gate_2.y;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;
                        direction = 'r';
                    }
                    else if(map[gate_2.y][gate_2.x-1] != 1){ //왼
                        //snake[0].x = gate_2.x-1;
                        //snake[0].y = gate_2.y;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;
                        direction = 'l';
                    }
                    else if(map[gate_2.y+1][gate_2.x] != 1){//아래
                        //snake[0].x = gate_2.x;
                        //snake[0].y = gate_2.y+1;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;
                        direction = 'd';
                    }
                }
                else if(direction == 'd'){//아레 -> 왼쪽 -> 오른쪽 -> 위
                    if(map[gate_2.y+1][gate_2.x] != 1){//아래
                        //snake[0].x = gate_2.x;
                        //snake[0].y = gate_2.y+1;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;
                    }
                    else if(map[gate_2.y][gate_2.x-1] != 1){ //왼
                        //snake[0].x = gate_2.x-1;
                        //snake[0].y = gate_2.y;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;
                        direction = 'l';
                    }
                    else if(map[gate_2.y][gate_2.x+1] != 1){ //오른쪽
                        //snake[0].x = gate_2.x+1;
                        //snake[0].y = gate_2.y;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;
                        direction = 'r';
                    }
                    else if(map[gate_2.y-1][gate_2.x] != 1){ //위
                        //snake[0].x = gate_2.x;
                        //snake[0].y = gate_2.y-1;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;
                        direction = 'u';
                    }
                }
                else if(direction == 'r'){//오른쪽 -> 아래 -> 위 -> 왼쪽
                    if(map[gate_2.y][gate_2.x+1] != 1){ //오른쪽
                        //snake[0].x = gate_2.x+1;
                        //snake[0].y = gate_2.y;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;

                    }
                    else if(map[gate_2.y+1][gate_2.x] != 1){//아래
                        //snake[0].x = gate_2.x;
                        //snake[0].y = gate_2.y+1;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;
                        direction = 'd';
                    }
                    else if(map[gate_2.y-1][gate_2.x] != 1){ //위
                        //snake[0].x = gate_2.x;
                        //snake[0].y = gate_2.y-1;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;
                        direction = 'u';
                    }
                    else if(map[gate_2.y][gate_2.x-1] != 1){ //왼
                        //snake[0].x = gate_2.x-1;
                        //snake[0].y = gate_2.y;
                        snake[0].x = gate_2.x;
                        snake[0].y = gate_2.y;
                        direction = 'l';
                    }
                }
            }
            pass_gate = true;
        }
        else if(snake[0].x == gate_2.x && snake[0].y == gate_2.y){
            if(gate_1.y == 0) {//벽이 상단에 있을때
                snake[0].x = gate_1.x;
                snake[0].y = 0; // 아랫방향
                direction = 'd';
            }
            else if(gate_1.x == 0){ //벽이 좌측에 있을때
                snake[0].x = 0;
                snake[0].y = gate_1.y;
                direction = 'r';
            }
            else if(gate_1.y == 29){ //벽이 하단에 있을때
                snake[0].x = gate_1.x;
                snake[0].y = 29;
                direction = 'u';
            }
            else if(gate_1.x == 59){ //벽이 우측에 있을때
                snake[0].x = 59;
                snake[0].y = gate_1.y;
                direction = 'l';
            }
            else{ //벽이 맵 중앙에 있을때
                if(direction == 'l'){ //진출방향이 왼쪽일때 , 왼쪽 -> 시계(위) -> 반시계(아래) -> 역방향(오)
                    if(map[gate_1.y][gate_1.x-1] != 1){ //왼
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                    }
                    else if(map[gate_1.y-1][gate_1.x] != 1){ //위
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                        direction = 'u';
                    }
                    else if(map[gate_1.y+1][gate_1.x] != 1){ //아래
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                        direction = 'd';
                    }
                    else if(map[gate_1.y][gate_1.x+1] != 1){ //오른쪽
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                        direction = 'r';
                    }
                }
                else if(direction == 'u'){//위 -> 오른쪽 -> 왼쪽 -> 아
                    if(map[gate_1.y-1][gate_1.x] != 1){ //위
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                    }
                    else if(map[gate_1.y][gate_1.x+1] != 1){ //오른쪽
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                        direction = 'r';
                    }
                    else if(map[gate_1.y][gate_1.x-1] != 1){ //왼
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                        direction = 'l';
                    }
                    else if(map[gate_1.y+1][gate_1.x] != 1){//아래
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                        direction = 'd';
                    }
                }
                else if(direction == 'd'){//아레 -> 왼쪽 -> 오른쪽 -> 위
                    if(map[gate_1.y+1][gate_1.x] != 1){//아래
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                    }
                    else if(map[gate_1.y][gate_1.x-1] != 1){ //왼
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                        direction = 'l';
                    }
                    else if(map[gate_1.y][gate_1.x+1] != 1){ //오른쪽
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                        direction = 'r';
                    }
                    else if(map[gate_1.y-1][gate_1.x] != 1){ //위
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                        direction = 'u';
                    }
                }
                else if(direction == 'r'){//오른쪽 -> 아래 -> 위 -> 왼쪽
                    if(map[gate_1.y][gate_1.x+1] != 1){ //오른쪽
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                    }
                    else if(map[gate_1.y+1][gate_1.x] != 1){//아래
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                        direction = 'd';
                    }
                    else if(map[gate_1.y-1][gate_1.x] != 1){ //위
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                        direction = 'u';
                    }
                    else if(map[gate_1.y][gate_1.x-1] != 1){ //왼
                        snake[0].x = gate_1.x;
                        snake[0].y = gate_1.y;
                        direction = 'l';
                    }
                }
            }
            pass_gate = true;
        }
    }
    if(pass_gate){
        wattron(game_Board, COLOR_PAIR(4));
        wmove(game_Board, gate_1.y, gate_1.x);
        waddch(game_Board,'1');  //gate items
        wattroff(game_Board, COLOR_PAIR(4));//gate1

        wattron(game_Board, COLOR_PAIR(4));
        wmove(game_Board, gate_2.y, gate_2.x);
        waddch(game_Board, '2');
        wattroff(game_Board, COLOR_PAIR(4));//gate2
        wrefresh(game_Board);
    }

    for (int i = 1; i < snake.size(); i++) { //자기 몸에 닿을 때
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
            return true;
    }
    //if(snake[snakesize()].x == )
    //collision with the coin
    if (snake[0].x == coin.x && snake[0].y == coin.y) {
        get_coin = true;
        map[coin.y][coin.x] = 0;
        put_Coin();
        coin_timer = 0;
        points++;
        mvwprintw(score_Board, 2, 6, "%d", snake.size() + 1);
        mvwprintw(score_Board, 3, 6, "%d", points);
        wrefresh(score_Board);
    } else
        get_coin = false;

    //collision with the poison
    if (snake[0].x == poison.x && snake[0].y == poison.y) {
        map[poison.y][poison.x] = 0;
        put_Posion();
        poison_timer = 0;
        minus_points++;
        wmove(game_Board, snake[snake.size() - 1].y, snake[snake.size() - 1].x);
        wprintw(game_Board, " ");
        snake.pop_back();
        wrefresh(game_Board);

        mvwprintw(score_Board, 2, 6, "%d", snake.size());
        mvwprintw(score_Board, 4, 6, "%d", minus_points);
        wrefresh(score_Board);
    }
    return false;
}

void snakeclass::passing_Gate(){
    if(snake[snake.size()-1].x == gate_1.x && snake[snake.size()-1].y == gate_1.y){
        pass_gate = false;
        wattron(game_Board, COLOR_PAIR(1));
        wmove(game_Board, gate_1.y, gate_1.x);
        waddch(game_Board,'@');  //gate items
        wattroff(game_Board, COLOR_PAIR(1));//gate1

        wattron(game_Board, COLOR_PAIR(1));
        wmove(game_Board, gate_2.y, gate_2.x);
        waddch(game_Board, '@');
        wattroff(game_Board, COLOR_PAIR(1));//gate2
        points++;
        mvwprintw(score_Board, 3, 6, "%d", points);
        wrefresh(game_Board);
        map[gate_1.y][gate_1.x] = 1;
        map[gate_2.y][gate_2.x] = 1;
        put_Gate();
        gate_timer = 0;
        wrefresh(score_Board);
    }
    if(snake[snake.size()-1].x == gate_2.x && snake[snake.size()-1].y == gate_2.y){
        pass_gate = false;
        wattron(game_Board, COLOR_PAIR(1));
        wmove(game_Board, gate_1.y, gate_1.x);
        waddch(game_Board,'@');  //gate items
        wattroff(game_Board, COLOR_PAIR(1));//gate1

        wattron(game_Board, COLOR_PAIR(1));
        wmove(game_Board, gate_2.y, gate_2.x);
        waddch(game_Board, '@');
        wattroff(game_Board, COLOR_PAIR(1));//gate2
        points++;
        mvwprintw(score_Board, 3, 6, "%d", points);
        wrefresh(game_Board);
        map[gate_1.y][gate_1.x] = 1;
        map[gate_2.y][gate_2.x] = 1;
        put_Gate();
        gate_timer = 0;
        wrefresh(score_Board);
    }
}

void snakeclass::movesnake() {
    //detect key
    int tmp = getch();
    switch (tmp) {
        case KEY_LEFT:
            if (direction != 'r')
                direction = 'l';
            break;
        case KEY_UP:
            if (direction != 'd')
                direction = 'u';
            break;
        case KEY_DOWN:
            if (direction != 'u')
                direction = 'd';
            break;
        case KEY_RIGHT:
            if (direction != 'l')
                direction = 'r';
            break;
        case KEY_BACKSPACE:
            direction = 'q';
            break;
    }

    if (!get_coin) {
        if(map[snake[snake.size()-1].y][snake[snake.size()-1].x] == 1){
            attron(COLOR_PAIR(1));
            move(snake[snake.size() - 1].y, snake[snake.size() - 1].x);
            printw("@");
            attroff(COLOR_PAIR(1));
            refresh();
            snake.pop_back();
        }
        else{
            move(snake[snake.size() - 1].y, snake[snake.size() - 1].x);
            printw(" ");
            refresh();
            snake.pop_back();
        }
    }

    if (direction == 'l') {
        snake.insert(snake.begin(), snakepart(snake[0].x - 1, snake[0].y));
    } else if (direction == 'r') {
        snake.insert(snake.begin(), snakepart(snake[0].x + 1, snake[0].y));
    } else if (direction == 'u') {
        snake.insert(snake.begin(), snakepart(snake[0].x, snake[0].y - 1));
    } else if (direction == 'd') {
        snake.insert(snake.begin(), snakepart(snake[0].x, snake[0].y + 1));
    }
    wmove(game_Board, snake[0].y, snake[0].x);
    waddch(game_Board, partchar);
    wrefresh(game_Board);
}

void snakeclass::start() {
    while (1) {
        if (collision() || snake.size() < 3) { //벽에 부딪히거나 스네이크 사이즈가 3보다 낮으면 사망
            wmove(game_Board, 12, 24); // y좌표 12랑 x좌표 24로 move
            wprintw(game_Board, "game_over"); //y : 12 x : 24에 gameover 출력
            wrefresh(game_Board); //wrefresh로 화면 새로고침 (게임보드)
            break; //게임 종료
        }

        movesnake(); //movesnake 실행
        if (direction == 'q')
            break;
        passing_Gate();


        #ifdef WIN32
        #else
        usleep(del); //0.5초에 한 칸씩
        coin_timer++; //코인 타이머 시작
        poison_timer++; //독 타이머 시작
        gate_timer++;

        if(coin_timer % 100 == 0){ //5초가 되면 실행
            wmove(game_Board, coin.y, coin.x); //원래 코인이 있던 곳으로 좌표 옮김
            wprintw(game_Board, " "); // " " 공백으로 지워버려
            map[coin.y][coin.x] = 0;
            put_Coin();
            coin_timer = 0;
        }
        if(poison_timer % 100 == 0){
            wmove(game_Board, poison.y, poison.x);
            wprintw(game_Board, " ");
            map[poison.y][poison.x] = 0;
            put_Posion();
            poison_timer = 0;
        }
        if(gate_timer % 200 == 0){ //초기화
          wattron(game_Board, COLOR_PAIR(1));
          wmove(game_Board, gate_1.y, gate_1.x); //gate1 초기
          wprintw(game_Board, "@");
          wattroff(game_Board, COLOR_PAIR(1));
          wattron(game_Board, COLOR_PAIR(1));
          wmove(game_Board, gate_2.y, gate_2.x);//gate2 초기화
          wprintw(game_Board, "@");
          wattroff(game_Board, COLOR_PAIR(1));
          map[gate_1.y][gate_1.x] = 1;
          map[gate_2.y][gate_2.x] = 1;
          put_Gate();
          gate_timer = 0;
        }
        #endif
    }
}