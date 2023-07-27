#include "f_texas.h"
#include "ui_f_texas.h"
#include "game_21.h"
#include <QString>
#include <QTimer>
#include <QValidator>

Banker b_tx;        //庄家
player* p_tx[4];    //玩家
int max_bet = 0;
int bk_num = 0;//当前庄家

f_texas::f_texas(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::f_texas)
{
    ui->setupUi(this);
    ui->f_fin->setVisible(false);
    ui->t_add_num->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));//number only
    ui->frame_c1->setVisible(false);
    ui->frame_c2->setVisible(false);
    ui->frame_c3->setVisible(false);
    ui->frame_c4->setVisible(false);
    ui->frame_c5->setVisible(false);
    ui->l_bk_num->setText(QString::number(bk_num+1, 10));
    //1.洗牌
    shuffing();
    b_tx.start();//牌顶弃一张
    for(int i = 0; i < 4; i++)
    {
        p_tx[i] = new player(1000);
        p_tx[i]->get_card();
        p_tx[i]->get_card();
    }
    player_turn();
}

void f_texas::drawcard1(Card c)
{
        ui->i1_12->setVisible(false);
        ui->i1_13->setVisible(false);
        ui->i1_14->setVisible(false);
        ui->i1_15->setVisible(false);
        switch (c.type)
        {
        case 0: ui->i1_12->setVisible(true); ui->num1_4->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");break;
        case 1: ui->i1_13->setVisible(true); ui->num1_4->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");break;
        case 2: ui->i1_14->setVisible(true); ui->num1_4->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}"); break;
        case 3: ui->i1_15->setVisible(true); ui->num1_4->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");break;
        }

        switch (c.num)
        {
        case 1:
            ui->num1_4->setText("A");
            break;
        case 11:
            ui->num1_4->setText("J");
            break;
        case 12:
            ui->num1_4->setText("Q");
            break;
        case 13:
            ui->num1_4->setText("K");
            break;
        default:
            ui->num1_4->setText(QString::number(c.num, 10));
            break;
        }
}

void f_texas::drawcard2(Card c)
{
    ui->i2_12->setVisible(false);
    ui->i2_13->setVisible(false);
    ui->i2_14->setVisible(false);
    ui->i2_15->setVisible(false);
    switch (c.type)
    {
    case 0: ui->i2_12->setVisible(true); ui->num2_4->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");break;
    case 1: ui->i2_13->setVisible(true); ui->num2_4->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");break;
    case 2: ui->i2_14->setVisible(true); ui->num2_4->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");break;
    case 3: ui->i2_15->setVisible(true); ui->num2_4->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");break;
    }

    switch (c.num)
    {
    case 1:
        ui->num2_4->setText("A");
        break;
    case 11:
        ui->num2_4->setText("J");
        break;
    case 12:
        ui->num2_4->setText("Q");
        break;
    case 13:
        ui->num2_4->setText("K");
        break;
    default:
        ui->num2_4->setText(QString::number(c.num, 10));
        break;
    }
}

void f_texas::player_turn()
{
    if(max_bet-p_tx[cur_player]->bet_money> p_tx[cur_player]->money)
    {
        p_tx[cur_player]->status=3;
//        cur_player++;
        nextturn(state);
        return;
    }
    while(p_tx[cur_player]->status || cur_player == bk_num)
    {
        cur_player++;
        if(cur_player == 4)
        {
            nextturn(state);
            return;
        }
//        nextturn(state);
//        return;
    }
    ui->l_next->setVisible(false);
    ui->c_bet->setEnabled(true);
    if(max_bet == 0) ui->c_call->setEnabled(false);
    else ui->c_call->setEnabled(true);
    ui->c_flop->setEnabled(true);
    ui->l_p_name->setText(QString::number(cur_player+1,10));
    ui->l_p_money->setText(QString::number(p_tx[cur_player]->money,10));
    ui->l_p_bet->setText(QString::number(p_tx[cur_player]->bet_money,10));
    drawcard1(p_tx[cur_player]->Com_Card[0]);
    drawcard2(p_tx[cur_player]->Com_Card[1]);
}

void f_texas::nextturn(int s)
{
    if(cur_player==4)
    {
        int check = 0;
        int cnt_flop = 0;
        for(int i = 0; i < 4; i++)
        {
            if(i==bk_num) continue;
            if(p_tx[i]->status!=0)
            {
                check++;
                cnt_flop++;
            }
            else if(p_tx[i]->bet_money==max_bet) check++;
        }
       if(cnt_flop==3)
       {
           fin(2);
           return;
       }
       if(check==3){
            if(s==0)//开三张牌
            {
                ui->l_state->setText("Flop");
                b_tx.get_card();
                b_tx.get_card();
                b_tx.get_card();
                DrawCard(1, b_tx.com_card[0]);
                DrawCard(2, b_tx.com_card[1]);
                DrawCard(3, b_tx.com_card[2]);
                cur_player = 0;
                this->state = 1;
            }
            else if(s==1)//开一张牌
            {
                ui->l_state->setText("Turn");
                b_tx.get_card();
                DrawCard(4, b_tx.com_card[3]);
                cur_player = 0;
                this->state = 2;
            }
            else if(s==2)//开一张牌
            {
                ui->l_state->setText("River");
                b_tx.get_card();
                DrawCard(5, b_tx.com_card[4]);
                fin(1);
                return;
            }
       }
       {
           cur_player = 0;
       }
    }
    player_turn();
}

void f_texas::fin(int type)
{
    if(type==1)
    {
        int win = 0;
        for(int i = 0; i < 4; i++)
        {
            if(i==bk_num) continue;
            if(p_tx[i]->status!=0)
            {
                win += p_tx[i]->bet_money;
                switch(i)
                {
                    case 0: ui->l_p1->setText("- " + QString::number(p_tx[i]->bet_money, 10)); break;
                    case 1: ui->l_p2->setText("- " + QString::number(p_tx[i]->bet_money, 10)); break;
                    case 2: ui->l_p3->setText("- " + QString::number(p_tx[i]->bet_money, 10)); break;
                    case 3: ui->l_p4->setText("- " + QString::number(p_tx[i]->bet_money, 10)); break;
                }
                continue;
            }

            switch(i)
            {
                case 0:
                {
                    if(p_tx[i]->Com_Card[0].num < b_tx.com_card[0].num)
                    {
                        ui->l_p1->setText("- " + QString::number(p_tx[i]->bet_money, 10));
                        win += p_tx[i]->bet_money;
                    }
                    else
                    {
                        ui->l_p1->setText("+ " + QString::number(p_tx[i]->bet_money, 10));
                        p_tx[i]->money += 2 * p_tx[i]->bet_money;
                        win -= p_tx[i]->bet_money;
                    }
                    break;
                }
                case 1:
                {
                    if(p_tx[i]->Com_Card[0].num < b_tx.com_card[0].num)
                    {
                        ui->l_p2->setText("- " + QString::number(p_tx[i]->bet_money, 10));
                        win += p_tx[i]->bet_money;
                    }
                    else
                    {
                        ui->l_p2->setText("+ " + QString::number(p_tx[i]->bet_money, 10));
                        p_tx[i]->money += 2 * p_tx[i]->bet_money;
                        win -= p_tx[i]->bet_money;
                    }
                    break;
                }
                case 2:
                {
                    if(p_tx[i]->Com_Card[0].num < b_tx.com_card[0].num)
                    {
                        ui->l_p3->setText("- " + QString::number(p_tx[i]->bet_money, 10));
                        win += p_tx[i]->bet_money;
                    }
                    else
                    {
                        ui->l_p3->setText("+ " + QString::number(p_tx[i]->bet_money, 10));
                        p_tx[i]->money += 2 * p_tx[i]->bet_money;
                        win -= p_tx[i]->bet_money;
                    }
                    break;
                }
                case 3:
                {
                    if(p_tx[i]->Com_Card[0].num < b_tx.com_card[0].num)
                    {
                        ui->l_p4->setText("- " + QString::number(p_tx[i]->bet_money, 10));
                        win += p_tx[i]->bet_money;
                    }
                    else
                    {
                        ui->l_p4->setText("+ " + QString::number(p_tx[i]->bet_money, 10));
                        p_tx[i]->money += 2 * p_tx[i]->bet_money;
                        win -= p_tx[i]->bet_money;
                    }
                    break;
                }
            }
        }
        switch(bk_num)
        {
        case 0:
            if(win > 0) ui->l_p1->setText("+ " + QString::number(win, 10));
            else        ui->l_p1->setText("- " + QString::number(-win, 10));
            break;
        case 1:
            if(win > 0) ui->l_p2->setText("+ " + QString::number(win, 10));
            else        ui->l_p2->setText("- " + QString::number(-win, 10));
            break;
        case 2:
            if(win > 0) ui->l_p3->setText("+ " + QString::number(win, 10));
            else        ui->l_p3->setText("- " + QString::number(-win, 10));
            break;
        case 3:
            if(win > 0) ui->l_p4->setText("+ " + QString::number(win, 10));
            else        ui->l_p4->setText("- " + QString::number(-win, 10));
            break;
        }
        p_tx[bk_num]->money += win;
        ui->f_fin->setVisible(true);
        QTimer* tim = new QTimer(this);
        if(tim->isActive()==false)  tim->start(2000);
        connect(tim, &QTimer::timeout, [=](){
            cur_player = 0;
            bk_num = (bk_num+1) % 4;
            regame();
            if(tim->isActive()==true)  tim->stop();
        });
    }
    else
    {
        int win = 0;
        for(int i = 0; i < 4; i++)
        {
            if(i==bk_num) continue;
            win += p_tx[i]->bet_money;
            switch(i)
            {
            case 0: ui->l_p1->setText("- " + QString::number(p_tx[i]->bet_money, 10)); break;
            case 1: ui->l_p2->setText("- " + QString::number(p_tx[i]->bet_money, 10)); break;
            case 2: ui->l_p3->setText("- " + QString::number(p_tx[i]->bet_money, 10)); break;
            case 3: ui->l_p4->setText("- " + QString::number(p_tx[i]->bet_money, 10)); break;
            }
        }
        switch(bk_num)
        {
        case 0: ui->l_p1->setText("+ " + QString::number(win, 10)); break;
        case 1: ui->l_p2->setText("+ " + QString::number(win, 10)); break;
        case 2: ui->l_p3->setText("+ " + QString::number(win, 10)); break;
        case 3: ui->l_p4->setText("+ " + QString::number(win, 10)); break;
        }
        p_tx[bk_num]->money += win;
        ui->f_fin->setVisible(true);
        QTimer* tim = new QTimer(this);
        if(tim->isActive()==false)  tim->start(2000);
        connect(tim, &QTimer::timeout, [=](){
            cur_player = 0;
            bk_num = (bk_num+1) % 4;
            regame();
            if(tim->isActive()==true)  tim->stop();
        });
    }
}

void f_texas::regame()
{
    ui->f_fin->setVisible(false);
    ui->frame_c1->setVisible(false);
    ui->frame_c2->setVisible(false);
    ui->frame_c3->setVisible(false);
    ui->frame_c4->setVisible(false);
    ui->frame_c5->setVisible(false);
    ui->l_bk_num->setText(QString::number(bk_num+1, 10));
    max_bet = 0;
    ui->l_max_bet->setText("0");

    //1.洗牌
    shuffing();
    b_tx.com_card.clear();
    b_tx.start();//牌顶弃一张
    for(int i = 0; i < 4; i++)
    {
        p_tx[i]->bet_money = 0;
        p_tx[i]->status = 0;
        p_tx[i]->Com_Card.clear();
        p_tx[i]->get_card();
        p_tx[i]->get_card();
    }
    player_turn();
}

f_texas::~f_texas()
{
    delete ui;
}


void f_texas::on_c_call_clicked()
{
    p_tx[cur_player]->bet(max_bet-p_tx[cur_player]->bet_money);
    ui->l_p_bet->setText(QString::number(p_tx[cur_player]->bet_money, 10));
    ui->l_p_money->setText(QString::number(p_tx[cur_player]->money, 10));
    ui->c_bet->setEnabled(false);
    ui->c_call->setEnabled(false);
    ui->c_flop->setEnabled(false);
    QTimer* tim = new QTimer(this);
    if(tim->isActive()==false)  tim->start(2000);
    ui->l_next->setVisible(true);
    connect(tim, &QTimer::timeout, [=](){
        cur_player++;
        nextturn(state);
        if(tim->isActive()==true)  tim->stop();
    });
}

void f_texas::on_c_bet_clicked()
{
    if(ui->t_add_num->text().isEmpty())
    {
//        ui->t_add_num->setText("COULDN'T BE EMPTY");
        return;
    }
    else
    {
        int add_num = ui->t_add_num->text().toInt();
        if(add_num > p_tx[cur_player]->money)
        {
            ui->t_add_num->setText("NOT THAT RICH");//正经翻译打不下
            return;
        }
        else
        {
            if(add_num+p_tx[cur_player]->bet_money > max_bet)
            {
                max_bet = add_num+p_tx[cur_player]->bet_money;
                ui->l_max_bet->setText(QString::number(max_bet, 10));
            }
            p_tx[cur_player]->bet(add_num);
            //刷新bet & money
            ui->t_add_num->setText("");
            ui->l_p_bet->setText(QString::number(p_tx[cur_player]->bet_money, 10));
            ui->l_p_money->setText(QString::number(p_tx[cur_player]->money, 10));
            ui->c_bet->setEnabled(false);
            ui->c_call->setEnabled(false);
            ui->c_flop->setEnabled(false);
            QTimer* tim = new QTimer(this);
            if(tim->isActive()==false)  tim->start(2000);
            ui->l_next->setVisible(true);
            connect(tim, &QTimer::timeout, [=](){
                cur_player++;
                nextturn(state);
                if(tim->isActive()==true)  tim->stop();
            });
        }
    }
}

void f_texas::on_c_flop_clicked()
{
    p_tx[cur_player]->surrender();
    cur_player++;
    QTimer* tim = new QTimer(this);
    if(tim->isActive()==false)  tim->start(2000);
    ui->l_next->setVisible(true);
    connect(tim, &QTimer::timeout, [=](){
        ui->l_next->setVisible(false);
        nextturn(state);
        if(tim->isActive()==true)  tim->stop();
    });
}

void f_texas::DrawCard(int i, Card c)
{
    if(i==1)
    {
        ui->i1_4->setVisible(false);
        ui->i1_5->setVisible(false);
        ui->i1_6->setVisible(false);
        ui->i1_7->setVisible(false);
        ui->frame_c1->setVisible(true);
        switch (c.num)
        {
        case 1:
            ui->num1_2->setText("A");
            break;
        case 11:
            ui->num1_2->setText("J");
            break;
        case 12:
            ui->num1_2->setText("Q");
            break;
        case 13:
            ui->num1_2->setText("K");
            break;
        default:
            ui->num1_2->setText(QString::number(c.num, 10));
            break;
        }
        switch (c.type)
        {
        case 0:
            ui->i1_4->setVisible(true);
            ui->num1_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            break;
        case 1:
            ui->num1_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i1_5->setVisible(true);
            break;
        case 2:
            ui->num1_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            ui->i1_6->setVisible(true);
            break;
        case 3:
            ui->num1_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i1_7->setVisible(true);
            break;
        default:
            break;
        }
     }
    else if(i==2)
    {
        ui->i2_4->setVisible(false);
        ui->i2_5->setVisible(false);
        ui->i2_6->setVisible(false);
        ui->i2_7->setVisible(false);
        ui->frame_c2->setVisible(true);
        switch (c.num)
        {
        case 1:
            ui->num2_2->setText("A");
            break;
        case 11:
            ui->num2_2->setText("J");
            break;
        case 12:
            ui->num2_2->setText("Q");
            break;
        case 13:
            ui->num2_2->setText("K");
            break;
        default:
            ui->num2_2->setText(QString::number(c.num, 10));
            break;
        }
        switch (c.type)
        {
        case 0:
            ui->i2_4->setVisible(true);
            ui->num2_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            break;
        case 1:
            ui->num2_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i2_5->setVisible(true);
            break;
        case 2:
            ui->num2_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            ui->i2_6->setVisible(true);
            break;
        case 3:
            ui->num2_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i2_7->setVisible(true);
            break;
        default:
            break;
        }
    }
    else if(i==3)
    {
        ui->i3_4->setVisible(false);
        ui->i3_5->setVisible(false);
        ui->i3_6->setVisible(false);
        ui->i3_7->setVisible(false);
        ui->frame_c3->setVisible(true);
        switch (c.num)
        {
        case 1:
            ui->num3_2->setText("A");
            break;
        case 11:
            ui->num3_2->setText("J");
            break;
        case 12:
            ui->num3_2->setText("Q");
            break;
        case 13:
            ui->num3_2->setText("K");
            break;
        default:
            ui->num3_2->setText(QString::number(c.num, 10));
            break;
        }
        switch (c.type)
        {
        case 0:
            ui->i3_4->setVisible(true);
            ui->num3_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            break;
        case 1:
            ui->num3_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i3_5->setVisible(true);
            break;
        case 2:
            ui->num3_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            ui->i3_6->setVisible(true);
            break;
        case 3:
            ui->num3_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i3_7->setVisible(true);
            break;
        default:
            break;
        }
    }
    else if(i==4)
    {
        ui->i4_4->setVisible(false);
        ui->i4_5->setVisible(false);
        ui->i4_6->setVisible(false);
        ui->i4_7->setVisible(false);
        ui->frame_c4->setVisible(true);
        switch (c.num)
        {
        case 1:
            ui->num4_2->setText("A");
            break;
        case 11:
            ui->num4_2->setText("J");
            break;
        case 12:
            ui->num4_2->setText("Q");
            break;
        case 13:
            ui->num4_2->setText("K");
            break;
        default:
            ui->num4_2->setText(QString::number(c.num, 10));
            break;
        }
        switch (c.type)
        {
        case 0:
            ui->i4_4->setVisible(true);
            ui->num4_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            break;
        case 1:
            ui->num4_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i4_5->setVisible(true);
            break;
        case 2:
            ui->num4_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            ui->i4_6->setVisible(true);
            break;
        case 3:
            ui->num4_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i4_7->setVisible(true);
            break;
        default:
            break;
        }
    }
    else if(i==5)
    {
        ui->i5_4->setVisible(false);
        ui->i5_5->setVisible(false);
        ui->i5_6->setVisible(false);
        ui->i5_7->setVisible(false);
        ui->frame_c5->setVisible(true);
        switch (c.num)
        {
        case 1:
            ui->num5_2->setText("A");
            break;
        case 11:
            ui->num5_2->setText("J");
            break;
        case 12:
            ui->num5_2->setText("Q");
            break;
        case 13:
            ui->num5_2->setText("K");
            break;
        default:
            ui->num5_2->setText(QString::number(c.num, 10));
            break;
        }
        switch (c.type)
        {
        case 0:
            ui->i5_4->setVisible(true);
            ui->num5_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            break;
        case 1:
            ui->num5_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i5_5->setVisible(true);
            break;
        case 2:
            ui->num5_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            ui->i5_6->setVisible(true);
            break;
        case 3:
            ui->num5_2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i5_7->setVisible(true);
            break;
        default:
            break;
        }
    }
}

void f_texas::on_b_set_clicked()
{
    this->hide();
    emit back();
}
