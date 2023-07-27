#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "game_21.h"      //21点的头文件

Banker b;        //庄家
player* p[4];    //玩家
AIplayer* ai[3]; //AI

/*
 * MainWindow Size：
 * width * height = 1500 * 1000
 * 固定大小 不可调节
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    this->Game_21();
//    this->BankerLayer_21();
//    this->Game_21();
    this->TitleLayer();
//    TexasLayer = new f_texas;
//    connect(TexasLayer,SIGNAL(back()), this, show());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::TitleLayer()
{
    this->layer = 0;
    // -- 不可见部分 --
    ui->frame21->setVisible(false);
    ui->background->setVisible(false);
    ui->black_background->setVisible(false);
    ui->b_go->setVisible(false);
    ui->l_name->setVisible(false);
    ui->t_uname->setVisible(false);
    ui->l_title->setVisible(false);
    ui->b_21->setVisible(false);
    ui->b_tx->setVisible(false);
    ui->b_local->setVisible(false);
    ui->b_online->setVisible(false);
    ui->l_404->setVisible(false);
    ui->b_back->setVisible(false);
    ui->b_one->setVisible(false);
    ui->b_two->setVisible(false);
    ui->b_three->setVisible(false);
    ui->b_four->setVisible(false);
}

void MainWindow::on_b_start_clicked()
{
    this->Unamelayer();
}

void MainWindow::Unamelayer()
{
    this->layer = 1;
    // -- 可见部分 --
    ui->b_go->setVisible(true);
    ui->l_name->setVisible(true);
    ui->t_uname->setAlignment(Qt::AlignHCenter);//输入文字居中
    ui->t_uname->setVisible(true);
    // -- 不可见部分 --
    ui->b_start->setVisible(false);
}

void MainWindow::on_b_go_clicked()
{
    //存储用户名
    this->uname = ui->t_uname->text();
    this->GameTypeLayer();
}


void MainWindow::GameTypeLayer()
{
    this->layer = 2;
    // -- 可见部分 --
    //设置背景
    ui->background->setVisible(true);
    ui->l_title->setVisible(true);
    ui->l_title->setText("GAME LIST");
    ui->b_21->setVisible(true);
    ui->b_tx->setVisible(true);
    ui->b_back->setVisible(true);
    // -- 不可见部分 --
    ui->Title_Image->setVisible(false);
    ui->b_go->setVisible(false);
    ui->l_name->setVisible(false);
    ui->t_uname->setVisible(false);
    ui->b_one->setVisible(false);
    ui->b_two->setVisible(false);
    ui->b_three->setVisible(false);
    ui->b_four->setVisible(false);
}

void MainWindow::on_b_21_clicked()
{
    this->game_type_ = 1;
    this->GameModeLayer();
}

void MainWindow::on_b_tx_clicked()
{
    this->game_type_ = 2;
    this->GameModeLayer();
}

void MainWindow::GameModeLayer()
{
    this->layer = 3;
    // -- 可见部分 --
    ui->l_title->setText("MODE LIST");
    ui->b_local->setVisible(true);
    ui->b_online->setVisible(true);
    // -- 不可见部分 --
    ui->b_21->setVisible(false);
    ui->b_tx->setVisible(false);
}

void MainWindow::on_b_local_clicked()
{
    this->game_mode_ = 1;
    this->PlayerNumLayer();
}

void MainWindow::on_b_online_clicked()
{
    this->game_mode_ = 2;
    this->InterErrLayer();
}

void MainWindow::InterErrLayer()
{
    this->layer = 4;
    // -- 可见部分 --
    ui->black_background->setVisible(true);
    ui->l_title->setText("INTERNET ERROR!");
    ui->l_404->setVisible(true);
    // -- 不可见部分 --
    ui->background->setVisible(false);
    ui->b_local->setVisible(false);
    ui->b_online->setVisible(false);
}

void MainWindow::PlayerNumLayer()
{
    this->layer = 5;
    // -- 可见部分 --
    ui->l_title->setText("PLAYER NUMBER");
    ui->b_one->setVisible(true);
    ui->b_two->setVisible(true);
    ui->b_three->setVisible(true);
    ui->b_four->setVisible(true);
    // -- 不可见部分 --
    ui->b_local->setVisible(false);
    ui->b_online->setVisible(false);
}

void MainWindow::on_b_back_clicked()
{
    switch(this->layer)
    {
        case 2:
            ui->Title_Image->setVisible(true);
            ui->background->setVisible(false);
            ui->b_21->setVisible(false);
            ui->b_tx->setVisible(false);
            ui->b_back->setVisible(false);
            ui->l_title->setVisible(false);
            this->Unamelayer();   break;
        case 3:
            ui->b_local->setVisible(false);
            ui->b_online->setVisible(false);
            this->GameTypeLayer();break;
        case 4:
            ui->background->setVisible(true);
            ui->black_background->setVisible(false);
            ui->l_404->setVisible(false);
            this->GameModeLayer();break;
        case 5:
            ui->b_one->setVisible(false);
            ui->b_two->setVisible(false);
            ui->b_three->setVisible(false);
            ui->b_four->setVisible(false);
            this->GameModeLayer();break;
        default:
            break;
    }
}

void MainWindow::on_b_one_clicked()
{
    this->player_num_ = 1;
    this->ai_num_ = 3;
    if(this->game_type_ == 1)
        this->Game_21();
    else
    {
        TexasLayer = new f_texas;
        this->TexasLayer->show();
    }
}

void MainWindow::on_b_two_clicked()
{
    this->player_num_ = 2;
    this->ai_num_ = 2;
    if(this->game_type_ == 1)
        this->Game_21();
    else
    {
        TexasLayer = new f_texas;
        this->TexasLayer->show();
    }
}

void MainWindow::on_b_three_clicked()
{
    this->player_num_ = 3;
    this->ai_num_ = 1;
    if(this->game_type_ == 1)
        this->Game_21();
    else
    {
        TexasLayer = new f_texas;
        this->TexasLayer->show();
    }
}

void MainWindow::on_b_four_clicked()
{
    this->player_num_ = 4;
    this->ai_num_ = 0;
    if(this->game_type_ == 1)
        this->Game_21();
    else
    {
        TexasLayer = new f_texas;
        this->TexasLayer->show();
    }
}
//to do: b_set & b_help
void MainWindow::Game_21()
{
    ui->frame21->setVisible(true);

    this->CardIni();
    ui->frame_u_norm->setVisible(false);
    ui->frame_set->setVisible(false);
    ui->l_tip->setAlignment(Qt::AlignHCenter);//提示文字居中
    ui->l_ai_sur->setAlignment(Qt::AlignHCenter);
    ui->l_ai_stop->setAlignment(Qt::AlignHCenter);
    ui->l_start->setAlignment(Qt::AlignHCenter);
    ui->label->setAlignment(Qt::AlignHCenter);
    ui->l_stay_bk->setAlignment(Qt::AlignHCenter);
    ui->num0->setAlignment(Qt::AlignHCenter);
    ui->num1->setAlignment(Qt::AlignHCenter);
    ui->num2->setAlignment(Qt::AlignHCenter);
    ui->num3->setAlignment(Qt::AlignHCenter);
    ui->num4->setAlignment(Qt::AlignHCenter);
    ui->num5->setAlignment(Qt::AlignHCenter);//提示文字居中
    ui->num0->setText("");
    ui->label->setText("");
    ui->topcard->setVisible(false);
    ui->i0_0->setVisible(false);
    ui->i0_1->setVisible(false);
    ui->i0_2->setVisible(false);
    ui->i0_3->setVisible(false);
    ui->frame_u_fail->setVisible(false);
    ui->frame_banker->setVisible(false);
    ui->frame_pass->setVisible(false);
    ui->frame_fin->setVisible(false);

    QTimer* tim = new QTimer(this);
    if(tim->isActive()==false)  tim->start(3000);
    connect(tim, &QTimer::timeout, [=](){
        ui->l_start->setVisible(false);
        ui->topcard->setVisible(true);
        //1.洗牌
        shuffing();
        //2.1 庄家给每位玩家发一张明牌
        for(int i = 0; i < this->player_num_; i++)
        {
            if(flag_regame==0)  p[i] = new player(1000);
            else
            {
                p[i]->status = 0;
                p[i]->Com_Card.clear();
                p[i]->bet_money = 0;
            }
            p[i]->start();
        }
        //2.2 庄家给每位AI发一张明牌
        for(int i = 0; i < this->ai_num_; i++)
        {
            if(flag_regame==0)  ai[i] = new AIplayer(1000);
            else
            {
                ai[i]->status = 0;
                ai[i]->Com_Card.clear();
                ai[i]->bet_money = 0;
            }
            ai[i]->start();
        }
        //3. 庄家给自己发一张明牌
        if(flag_regame==1)  b.com_card.clear();
        b.start();
        //显示点数及undercard

        this->DrawBankerCard(b.under_card);
        ui->label->setText(QString::number(b.judge(), 10));
        //4.1 庄家给每位玩家发一张明牌
        for(int i = 0; i < this->player_num_; i++)  p[i]->get_card();
        //4.2 庄家给每位AI发一张明牌
        for(int i = 0; i < this->ai_num_; i++)      ai[i]->get_card();
        //5. 依次询问玩家是否加注/投降/分牌，随后询问玩家是否加牌
        cur_player = 0;
        this->BetLayer_21();
//        this->BankerLayer_21();
        if(tim->isActive()==true)  tim->stop();
    });

}

void MainWindow::BetLayer_21()
{
//    this->CardIni();
    //ini
    //确定是玩家orAI
    int isPlayer;
    if(cur_player<player_num_) isPlayer = 1;
    else                       isPlayer = 0;
    int valid = -1;
    int n_ai = cur_player - player_num_;
    if(isPlayer)//palyer
    {
        //draw cards
        int n_card = p[cur_player]->Com_Card.size();
        for(int i = 1; i <= n_card; i++)
            this->DrawCard(i, p[cur_player]->Com_Card[i-1]);
        valid = p[cur_player]->get_status();
    }
    else
    {
        //draw cards
        n_ai = cur_player - player_num_;
        int n_card = ai[n_ai]->Com_Card.size();
        for(int i = 1; i <= n_card; i++)
            this->DrawCard(i, ai[n_ai]->Com_Card[i-1]);
        valid = ai[n_ai]->get_status();
    }

    if(valid==0)
    {
        this->layer_21 = 0;
        ui->frame_u_norm->setVisible(true);
        if(isPlayer)
        {
            ui->b_skip->setEnabled(true);
            ui->b_do->setEnabled(true);
            ui->b_skip->setVisible(true);
            ui->b_do->setVisible(true);
            ui->l_ai_sur->setVisible(false);
            ui->l_ai_stop->setVisible(false);
            ui->l_payer_num->setText("PLAYER  " + QString::number(cur_player+1, 10));
            ui->n_bet->setText(QString::number(p[cur_player]->bet_money, 10));
            ui->n_money->setText(QString::number(p[cur_player]->money, 10));
            ui->n_points->setText(QString::number(p[cur_player]->judge(), 10));
            ui->l_tip->setText("RAISE BET ?");
        }
        else
        {
            //一次性展示AI的所有行为
            ui->b_skip->setVisible(false);
            ui->b_do->setVisible(false);
            ui->l_payer_num->setText("AI  " + QString::number(n_ai+1, 10));
            int r_bet = ai[n_ai]->Bet_money();
            ui->l_tip->setText("RAISE: " + QString::number(r_bet, 10));
            ui->n_bet->setText(QString::number(ai[n_ai]->bet_money, 10));
            ui->n_money->setText(QString::number(ai[n_ai]->money, 10));
            ui->n_points->setText(QString::number(ai[n_ai]->judge(), 10));
            if(ai[n_ai]->surrender_or_not())//投降
            {
                ui->l_ai_sur->setText("SURRENDER");
                ui->l_ai_sur->setVisible(true);
                ui->l_ai_stop->setVisible(false);
                ai[n_ai]->surrender();
            }
            else//继续
            {
                ui->l_ai_sur->setText("CONTINUE");
                ui->l_ai_sur->setVisible(true);
                if(ai[n_ai]->stop_or_get())//stop
                {
                    ui->l_ai_stop->setText("STAND");
                    ai[n_ai]->stop_card();
                }
                else
                {
                    ui->l_ai_stop->setText("GET CARD");
                    ai[n_ai]->get_card();
                    int last_card = ai[n_ai]->Com_Card.size();
                    this->DrawCard(last_card, ai[n_ai]->Com_Card[last_card-1]);
                    if(ai[n_ai]->judge())
                        ui->n_points->setText(QString::number(ai[n_ai]->judge(), 10));
                    else
                        ui->n_points->setText(" - ");
                }
                ui->l_ai_stop->setVisible(true);
            }
            //4s后跳转至过渡帧
            QTimer* tim = new QTimer(this);
            if(tim->isActive()==false)  tim->start(4000);
            connect(tim, &QTimer::timeout, [=](){
                this->PassLayer_21();
                if(tim->isActive()==true)  tim->stop();
            });
            tim->start();
        }

        ui->t_add_num->setVisible(false);
        ui->b_add->setVisible(false);
        ui->frame_pass->setVisible(false);
        ui->frame_u_fail->setVisible(false);
    }
    else
    {
        ui->frame_u_fail->setVisible(true);
        if(isPlayer)
        {
            ui->l_payer_num_f->setText("PLAYER  " + QString::number(cur_player+1, 10));
            ui->n_bet_f->setText(QString::number(p[cur_player]->bet_money, 10));
            ui->n_money_f->setText(QString::number(p[cur_player]->money, 10));
        }
        else
        {
            ui->l_payer_num_f->setText("AI  " + QString::number(n_ai+1, 10));
            ui->n_bet_f->setText(QString::number(ai[n_ai]->bet_money, 10));
            ui->n_money_f->setText(QString::number(ai[n_ai]->money, 10));
        }
        ui->l_u_status->setAlignment(Qt::AlignHCenter);//status 文字居中
        switch (valid)
        {
            case 3:
                ui->l_u_status->setStyleSheet("QLabel{font: 87 26pt \"Lovelo\"; color: rgb(166, 3, 47);}");//RED
                ui->l_u_status->setText("BUSTED");
                ui->n_points_f->setText(" - ");
                break;
            case 2:
                ui->l_u_status->setStyleSheet("QLabel{font: 87 26pt \"Lovelo\"; color: rgb(86, 85, 85);}");//GREY
                ui->l_u_status->setText("SURRENDER");
                if(isPlayer)
                    ui->n_points_f->setText(QString::number(p[cur_player]->judge(), 10));
                else
                    ui->n_points_f->setText(QString::number(ai[n_ai]->judge(), 10));
                break;
            case 1:
                ui->l_u_status->setStyleSheet("QLabel{font: 87 26pt \"Lovelo\"; color: rgb(255, 249, 87);}");//YELLOW
                ui->l_u_status->setText("STAND");
                if(isPlayer)
                    ui->n_points_f->setText(QString::number(p[cur_player]->judge(), 10));
                else
                    ui->n_points_f->setText(QString::number(ai[n_ai]->judge(), 10));
                break;
            default: break;
        }
            QTimer* tim = new QTimer(this);
            if(tim->isActive()==false)  tim->start(2000);
            connect(tim, &QTimer::timeout, [=](){
                this->PassLayer_21();
                if(tim->isActive()==true)  tim->stop();
            });

        ui->frame_pass->setVisible(false);
        ui->frame_u_norm->setVisible(false);
    }
}

void MainWindow::SurrenderLayer_21()
{
    ui->t_add_num->setVisible(false);
    ui->b_add->setVisible(false);

    ui->l_tip->setText("SURRENDER ?");
    ui->b_skip->setVisible(true);
    ui->b_do->setVisible(true);
}

void MainWindow::StandLayer_21()
{
    ui->l_tip->setText("STAND ?");
}

void MainWindow::on_b_do_clicked()
{
    if(layer_21 == 0)//对bet的追加操作
    {
        ui->l_tip->setText("ADD NUM:");
        ui->t_add_num->setVisible(true);
        ui->t_add_num->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));//number only
        ui->b_add->setVisible(true);

        ui->b_do->setVisible(false);
        ui->b_skip->setVisible(false);
    }
    else if(layer_21 == 1)//对surrend的追加操作
    {
        p[cur_player]->surrender();
        this->StandLayer_21();
        layer_21 = 2;//跳过抽牌阶段
    }
    else//对stand的追加操作
    {
        p[cur_player]->stop_card();
    }
    this->layer_21 += 1;
    if(layer_21==3)
        this->PassLayer_21();
}

void MainWindow::on_b_skip_clicked()
{
    if(layer_21 == 0)       this->SurrenderLayer_21();
    else if(layer_21 == 1)  this->StandLayer_21();
    else //加牌
    {
        p[cur_player]->get_card();
        int cur_point = p[cur_player]->judge();
        if(cur_point)   ui->n_points->setText(QString::number(cur_point, 10));
        else            ui->n_points->setText(" - ");
        ui->b_skip->setEnabled(false);
        ui->b_do->setEnabled(false);//暂时禁用，防止重复点击
        int last_card = p[cur_player]->Com_Card.size();
        this->DrawCard(last_card, p[cur_player]->Com_Card[last_card-1]);
    }
    this->layer_21 += 1;

    if(layer_21==3)
    {
        //加timer，展示抽卡结果->next
        QTimer* tim = new QTimer(this);
        if(tim->isActive()==false)  tim->start(2000);
        connect(tim, &QTimer::timeout, [=](){
            this->PassLayer_21();
            if(tim->isActive()==true)  tim->stop();
        });
//        QTimer::singleShot(2000, this [=]{
//            this->PassLayer_21();
//        });
    }
}

void MainWindow::on_b_add_clicked()//加注处理
{
    if(ui->t_add_num->text().isEmpty())
    {
        ui->t_add_num->setText("COULDN'T BE EMPTY");
        return;
    }
    else
    {
        int add_num = ui->t_add_num->text().toInt();
        if(add_num > p[cur_player]->money)
        {
            ui->t_add_num->setText("NOT THAT RICH");//正经翻译打不下
            return;
        }
        else
        {
            p[cur_player]->bet(add_num);
            //刷新bet & money
            ui->t_add_num->setText("");
            ui->n_bet->setText(QString::number(p[cur_player]->bet_money, 10));
            ui->n_money->setText(QString::number(p[cur_player]->money, 10));
            this->SurrenderLayer_21();
        }
    }
}

void MainWindow::BankerLayer_21()
{
    ui->frame_banker->setVisible(true);
    ui->frame_pass->setVisible(false);
    ui->frame_u_fail->setVisible(false);
    ui->frame_u_norm->setVisible(false);

    ui->l_banker_status->setAlignment(Qt::AlignHCenter);//输入文字居中
    ui->label->setText(QString::number(b.judge(), 10));

    QTimer* tim = new QTimer(this);
    connect(tim, &QTimer::timeout, [=](){
        ui->l_banker_status->setText("GET CARD!");
        b.get_card();
        ui->label->setText(QString::number(b.judge(), 10));
        int last_card = b.com_card.size();
//        ui->l_banker_status->setText("com card = " + QString::number(last_card+1, 10));
//        ui->l_banker_status->setText("last card = " + QString::number( b.com_card[last_card-1].num, 10));
        this->DrawCard(last_card, b.com_card[last_card-1]);
        int v_b = b.judge();
        if(v_b==0 || v_b>=17)
        {
            if(tim->isActive()==true)
                tim->stop();
            if(v_b==0)
                ui->l_banker_status->setText("BANKER BURSTED!");
            QTimer* tim1 = new QTimer(this);
            connect(tim1, &QTimer::timeout, [=](){
                this->Fin_21();
                if(tim1->isActive()==true)  tim1->stop();
            });
            if(tim1->isActive()==false)  tim1->start(2000);
        }
    });
    if(tim->isActive()==false)  tim->start(2000);
}

void MainWindow::PassLayer_21()
{
    this->CardIni();
    ui->frame_u_norm->setVisible(false);
    ui->frame_u_fail->setVisible(false);
    ui->frame_pass->setVisible(true);

    //加timer，hold->next
    QTimer* tim = new QTimer(this);
    if(tim->isActive()==false)  tim->start(2000);
    connect(tim, &QTimer::timeout, [=](){
        if(cur_player < 3)
        {
            cur_player++;
            this->BetLayer_21();
        }
        else
        {
            cur_player = 0;
            int cnt_valid = 0;
//            for(int i = 0; i < 4; i++)
//                if(p[i]->get_status()==0) cnt_valid++;
            for(int i = 0; i < player_num_; i++)
                if(p[i]->get_status()==0)  cnt_valid++;
            for(int i = 0; i < ai_num_; i++)
                if(ai[i]->get_status()==0) cnt_valid++;
            if(cnt_valid==0)    this->BankerLayer_21();
            else                this->BetLayer_21();
        }
        if(tim->isActive()==true)  tim->stop();
    });
}

void MainWindow::Fin_21()
{
//    this->CardIni();
    ui->frame_banker->setVisible(false);
    ui->frame_fin->setVisible(true);
    int v_b = b.judge();
    if(v_b)
    {
        for(int i = 0; i < player_num_; i++)
        {
            int s_p = p[i]->get_status();
            int v_p = p[i]->judge();
            int bet = p[i]->bet_money;
            switch (s_p)
            {
                case 3:
                    GetLabel(i)->setText("Player " + QString::number(i+1, 10) + ": - " + QString::number(bet, 10));
                    break;
                case 2:
                    GetLabel(i)->setText("Player " + QString::number(i+1, 10) + ": - " + QString::number(bet/2, 10));
                    p[i]->money += (p[i]->bet_money)/2;
                    break;
                case 1:
                    if(v_p<v_b)
                    {
                        GetLabel(i)->setText("Player " + QString::number(i+1, 10) + ": - " + QString::number(bet, 10));
                    }
                    else if(v_p==v_b)
                    {
                        GetLabel(i)->setText("Player " + QString::number(i+1, 10) + ": + 0");
                        p[i]->money += bet;
                    }
                    else
                    {
                        GetLabel(i)->setText("Player " + QString::number(i+1, 10) + ": + " + QString::number(bet, 10));
                        p[i]->money += 2*bet;
                    }
                    break;
                case 0:
                default:
                    break;
            }
        }
        for(int i = 0; i < ai_num_; i++)
        {
            int s_p = ai[i]->get_status();
            int v_p = ai[i]->judge();
            int bet = ai[i]->bet_money;
            switch (s_p)
            {
                case 3:
                    GetLabel(player_num_+i)->setText("AI " + QString::number(i+1, 10) + ": - " + QString::number(bet, 10));
                    break;
                case 2:
                    GetLabel(player_num_+i)->setText("AI " + QString::number(i+1, 10) + ": - " + QString::number(bet/2, 10));
                    ai[i]->money += (ai[i]->bet_money)/2;
                    break;
                case 1:
                    if(v_p<v_b)
                    {
                        GetLabel(player_num_+i)->setText("AI " + QString::number(i+1, 10) + ": - " + QString::number(bet, 10));
                    }
                    else if(v_p==v_b)
                    {
                        GetLabel(player_num_+i)->setText("AI " + QString::number(i+1, 10) + ": + 0");
                        ai[i]->money += bet;
                    }
                    else
                    {
                        GetLabel(player_num_+i)->setText("AI " + QString::number(i+1, 10) + ": + " + QString::number(bet, 10));
                        ai[i]->money += 2*bet;
                    }
                    break;
                case 0:
                default:
                    break;
            }
        }
    }
    else
    {
        for(int i = 0; i < player_num_; i++)
        {
            int v_p = p[i]->judge();
            int bet = p[i]->bet_money;
            if(v_p)
            {
                if(p[i]->get_status()==2)
                {
                    GetLabel(i)->setText("Player " + QString::number(i+1, 10) + ": - " + QString::number(bet/2, 10));
                    p[i]->money += bet/2;
                }
                else
                {
                    GetLabel(i)->setText("Player " + QString::number(i+1, 10) + ": + " + QString::number(bet, 10));
                    p[i]->money += 2*bet;
                }
            }
            else
            {
                GetLabel(i)->setText("Player " + QString::number(i+1, 10) + ": - " + QString::number(bet, 10));
            }
        }
        for(int i = 0; i < ai_num_; i++)
        {
            int v_p = ai[i]->judge();
            int bet = ai[i]->bet_money;
            if(v_p)
            {
                if(ai[i]->get_status()==2)
                {
                    GetLabel(player_num_+i)->setText("AI " + QString::number(i+1, 10) + ": - " + QString::number(bet/2, 10));
                    ai[i]->money += bet/2;
                }
                else
                {
                    GetLabel(player_num_+i)->setText("AI " + QString::number(i+1, 10) + ": + " + QString::number(bet, 10));
                    ai[i]->money += 2*bet;
                }
            }
            else
            {
                GetLabel(player_num_+i)->setText("AI " + QString::number(i+1, 10) + ": - " + QString::number(bet, 10));
            }
        }
    }

}

QLabel* MainWindow::GetLabel(int i)
{
    switch (i)
    {
        case 0: return ui->res_1;
        case 1: return ui->res_2;
        case 2: return ui->res_3;
        case 3: return ui->res_4;
        default: break;
    }
}

void MainWindow::on_b_re_clicked()
{
    flag_regame = 1;
    ui->num0->setText("");
    ui->label->setText("");
    ui->l_banker_status->setText("");
    this->Game_21();
}

void MainWindow::on_b_menu_clicked()
{
    ui->frame21->setVisible(false);
    this->GameTypeLayer();
}

void MainWindow::DrawCard(int i, Card c)
{
    if(i==1)
    {
        ui->i1_0->setVisible(false);
        ui->i1_1->setVisible(false);
        ui->i1_2->setVisible(false);
        ui->i1_3->setVisible(false);
        ui->frame_c1->setVisible(true);
        switch (c.num)
        {
        case 1:
            ui->num1->setText("A");
            break;
        case 11:
            ui->num1->setText("J");
            break;
        case 12:
            ui->num1->setText("Q");
            break;
        case 13:
            ui->num1->setText("K");
            break;
        default:
            ui->num1->setText(QString::number(c.num, 10));
            break;
        }
        switch (c.type)
        {
        case 0:
            ui->i1_0->setVisible(true);
            ui->num1->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            break;
        case 1:
            ui->num1->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i1_1->setVisible(true);
            break;
        case 2:
            ui->num1->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            ui->i1_2->setVisible(true);
            break;
        case 3:
            ui->num1->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i1_3->setVisible(true);
            break;
        default:
            break;
        }
     }
    else if(i==2)
    {
        ui->i2_0->setVisible(false);
        ui->i2_1->setVisible(false);
        ui->i2_2->setVisible(false);
        ui->i2_3->setVisible(false);
        ui->frame_c2->setVisible(true);
        switch (c.num)
        {
        case 1:
            ui->num2->setText("A");
            break;
        case 11:
            ui->num2->setText("J");
            break;
        case 12:
            ui->num2->setText("Q");
            break;
        case 13:
            ui->num2->setText("K");
            break;
        default:
            ui->num2->setText(QString::number(c.num, 10));
            break;
        }
        switch (c.type)
        {
        case 0:
            ui->i2_0->setVisible(true);
            ui->num2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            break;
        case 1:
            ui->num2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i2_1->setVisible(true);
            break;
        case 2:
            ui->num2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            ui->i2_2->setVisible(true);
            break;
        case 3:
            ui->num2->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i2_3->setVisible(true);
            break;
        default:
            break;
        }
    }
    else if(i==3)
    {
        ui->i3_0->setVisible(false);
        ui->i3_1->setVisible(false);
        ui->i3_2->setVisible(false);
        ui->i3_3->setVisible(false);
        ui->frame_c3->setVisible(true);
        switch (c.num)
        {
        case 1:
            ui->num3->setText("A");
            break;
        case 11:
            ui->num3->setText("J");
            break;
        case 12:
            ui->num3->setText("Q");
            break;
        case 13:
            ui->num3->setText("K");
            break;
        default:
            ui->num3->setText(QString::number(c.num, 10));
            break;
        }
        switch (c.type)
        {
        case 0:
            ui->i3_0->setVisible(true);
            ui->num3->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            break;
        case 1:
            ui->num3->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i3_1->setVisible(true);
            break;
        case 2:
            ui->num3->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            ui->i3_2->setVisible(true);
            break;
        case 3:
            ui->num3->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i3_3->setVisible(true);
            break;
        default:
            break;
        }
    }
    else if(i==4)
    {
        ui->i4_0->setVisible(false);
        ui->i4_1->setVisible(false);
        ui->i4_2->setVisible(false);
        ui->i4_3->setVisible(false);
        ui->frame_c4->setVisible(true);
        switch (c.num)
        {
        case 1:
            ui->num4->setText("A");
            break;
        case 11:
            ui->num4->setText("J");
            break;
        case 12:
            ui->num4->setText("Q");
            break;
        case 13:
            ui->num4->setText("K");
            break;
        default:
            ui->num4->setText(QString::number(c.num, 10));
            break;
        }
        switch (c.type)
        {
        case 0:
            ui->i4_0->setVisible(true);
            ui->num4->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            break;
        case 1:
            ui->num4->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i4_1->setVisible(true);
            break;
        case 2:
            ui->num4->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            ui->i4_2->setVisible(true);
            break;
        case 3:
            ui->num4->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i4_3->setVisible(true);
            break;
        default:
            break;
        }
    }
    else if(i==5)
    {
        ui->i5_0->setVisible(false);
        ui->i5_1->setVisible(false);
        ui->i5_2->setVisible(false);
        ui->i5_3->setVisible(false);
        ui->frame_c5->setVisible(true);
        switch (c.num)
        {
        case 1:
            ui->num5->setText("A");
            break;
        case 11:
            ui->num5->setText("J");
            break;
        case 12:
            ui->num5->setText("Q");
            break;
        case 13:
            ui->num5->setText("K");
            break;
        default:
            ui->num5->setText(QString::number(c.num, 10));
            break;
        }
        switch (c.type)
        {
        case 0:
            ui->i5_0->setVisible(true);
            ui->num5->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            break;
        case 1:
            ui->num5->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i5_1->setVisible(true);
            break;
        case 2:
            ui->num5->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
            ui->i5_2->setVisible(true);
            break;
        case 3:
            ui->num5->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
            ui->i5_3->setVisible(true);
            break;
        default:
            break;
        }
    }
}

void MainWindow::DrawBankerCard(Card bk)
{
    switch (bk.num)
    {
    case 1:
        ui->num0->setText("A");
        break;
    case 11:
        ui->num0->setText("J");
        break;
    case 12:
        ui->num0->setText("Q");
        break;
    case 13:
        ui->num0->setText("K");
        break;
    default:
        ui->num0->setText(QString::number(bk.num, 10));
        break;
    }
    switch (bk.type)
    {
    case 0:
        ui->i0_0->setVisible(true);
        ui->num0->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
        break;
    case 1:
        ui->num0->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
        ui->i0_1->setVisible(true);
        break;
    case 2:
        ui->num0->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: rgb(166, 3, 47);}");
        ui->i0_2->setVisible(true);
        break;
    case 3:
        ui->num0->setStyleSheet("QLabel{font: 87 12pt \"Lovelo\"; color: black;}");
        ui->i0_3->setVisible(true);
        break;
    default:
        break;
    }
}

void MainWindow::CardIni()//隐藏所有玩家牌
{
    ui->frame_c1->setVisible(false);
    ui->frame_c2->setVisible(false);
    ui->frame_c3->setVisible(false);
    ui->frame_c4->setVisible(false);
    ui->frame_c5->setVisible(false);
}

void MainWindow::on_b_help_clicked()//打开README.txt
{
    QString curPath = QCoreApplication::applicationDirPath();//exe文件所在路径
    QString filePath = "file:///" + curPath + "/README.txt";
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void MainWindow::on_b_set_clicked()
{
    ui->frame_set->setVisible(true);
}

void MainWindow::on_b_continue_clicked()//返回游戏界面
{
    ui->frame_set->setVisible(false);
}

void MainWindow::on_b_main_clicked()//返回游戏选择页面
{
    ui->frame_set->setVisible(false);
    ui->frame21->setVisible(false);
    this->GameTypeLayer();
}
