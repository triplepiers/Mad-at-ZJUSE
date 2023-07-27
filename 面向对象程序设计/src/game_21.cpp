#include "game_21.h"
/* Version 2.0
 * 1. 增加对选择玩家数量的相应
 * 2. 增加对Help和Set按钮的支持
 * 3. 增加AI功能
 */
void shuffing()
{
    final = 0;
    unsigned seed=time(0);
    srand(seed);
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 1; j <= 13; ++j)
        {
            card[j + i * 13].num = j;
        }
    }
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 1; j <= 13; ++j)
        {
            card[j + i * 13].type = i%4;
        }
    }
    for (int ii = 0; ii < 500; ++ii)
    {
        int r1 = rand()%52 + 1;
        int r2 = rand()%52 + 1;
        int t = card[r1].num, w = card[r1].type;
        card[r1].num = card[r2].num;
        card[r1].type = card[r2].type;
        card[r2].num = t;
        card[r2].type = w;
    }
}

Card liscening()
{
    final++;
    return card[final];
}

void player::get_card()
{
    if (status != 0)
    {
        return;
    }
    Com_Card.push_back(liscening());
}

void player::bet(int num)
{
    int cur = num;
    if (status != 0)
    {
        return;
    }

    bet_money += cur;
    money -= cur;
}

int player::judge(void)
{
    int sum = 0;
    int down=0;
    vector<Card>::iterator it;
    for (it = Com_Card.begin(); it != Com_Card.end(); it++)
    {
        int a=(*it).num>10?10:(*it).num;
        if(a==1)
        {
            a=11;
        }
        sum +=a;
        if(a==11)
        {
            down-=10;
        }
    }
    if(sum>21)
    {
        while(sum>21 && down!=0)
        {
            sum-=10;
            down+=10;
        }
    }
    if(sum>21)
    {
        sum=0;
        this->status = 3;
    }
    return sum;
}

int Banker::judge(void)
{
    int sum = 0,down=0;
    int a=(under_card.num>10)?10:under_card.num;
    if(a==1)
    {
        a=11;
    }
    if(a==11)
    {
         down-=10;
    }
    sum+=a;
    vector<Card>::iterator it;
    for (it = com_card.begin(); it != com_card.end(); it++)
    {
        a=(*it).num>10?10:(*it).num;
        if(a==1)
        {
            a=11;
        }
        sum +=a;
        if(a==11)
        {
            down-=10;
        }
    }
    if(sum>21)
    {
        while(sum>21 && down!=0)
        {
            sum-=10;
            down+=10;
        }
    }
    if(sum>21)
    {
        sum=0;
    }
    return sum;
}

bool AIplayer::surrender_or_not(void)
{
    int num=judge();
    if(num==0)
    {
        return true;
    }
    else if(num < 19)
    {
        if(rand()%4==0)
        {
            return true;
        }
     }
     return false;
}
int AIplayer::Bet_money(void)
{
    int gap=count-c;
    c=count;
    int num=0;
    if(gap<0)
    {
        num++;
    }
    if(count<-2)
    {
        num++;
    }
    bet_money += num*once_bet;
    money -= num*once_bet;
    return num*once_bet;
}
bool AIplayer::stop_or_get(void)
 {
    int num=judge();
    if(num<=10)
    {
        return false;
    }
    if(num<17)
    {
        if(rand()%4)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return true;
}
int AIplayer::judge(void)
{
    int sum = 0;
    int down=0;
    vector<Card>::iterator it;
    for (it = Com_Card.begin(); it != Com_Card.end(); it++)
    {
        int a=(*it).num>10?10:(*it).num;
        if(a==1)
        {
            a=11;
        }
        sum +=a;
        if(a==11)
        {
            down-=10;
        }
    }
    if(sum>21)
    {
        while(sum>21 && down!=0)
        {
            sum-=10;
            down+=10;
        }
    }
    if(sum>21)
    {
        sum=0;
        this->status = 3;
    }
    return sum;
}
