#ifndef GAME_21_H
#define GAME_21_H

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

struct CARD   //纸牌类
{
    int num;  //纸牌编号
    int type; //纸牌花色
};
typedef struct CARD Card;

#include "global.h"

void shuffing(); //洗牌
Card liscening();//发牌*1

class player  //普通玩家类
{
public:
    int status;
    /* 状态列表
     * 0 - 正常
     * 1 - 停牌
     * 2 - 投降
     * 3 - 爆牌
     */
    int money;             //总资金
    int bet_money;         //赌注
    vector<Card> Com_Card; //手牌

    player(int ini_money): money(ini_money), status(0), bet_money(0) {}

    void get_card();       //取单张牌
    int judge(void);       //计算点数，爆牌返回0
    void bet(int num);     //加注num
    void stop_card() {status = 1;}   //停牌
    void surrender() {status = 2;}   //投降
    int get_status() {return status;}//获取状态
    void start() {get_card();}
};

class Banker   //庄家类
{
public:
    vector<Card> com_card; //明牌堆
    Card under_card;       //暗牌
    void get_card() {com_card.push_back(liscening());}//摸一张
    void start() {under_card=liscening();
                  //get_card();
                 }
    int judge(void);       //计算点数，爆牌返回0
};

class AIplayer //AI类
{
public:
    int c;
    int status;
    int money;
    int bet_money;
    int once_bet;
    vector<Card> Com_Card;
    AIplayer(int n)
    {
        money = n;
        once_bet=money/10;
        status = 0;
        bet_money = 0;
    }
    int get_status()
    {
        return status;
    }
    void get_card()
    {
        if (status != 0)
        {
            return;
        }
        Com_Card.push_back(liscening());
    }
    void stop_card()
    {
        status = 1;
    }
    void bet(int num)
    {
        if (status != 0)
        {
            return;
        }
        bet_money += num;
    }
    void surrender()
    {
        status = 2;
    }
    void start() {get_card();}
    bool stop_or_get(void);
    int Bet_money(void);
    bool surrender_or_not(void);
    int judge(void);
};

#endif // GAME_21_H

