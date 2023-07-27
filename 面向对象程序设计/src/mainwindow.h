#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "game_21.h"
#include "f_texas.h"
#include <QDesktopServices>
#include <QMainWindow>
#include <QPushButton>
#include <QValidator>
#include <QTimer>
#include <QLabel>
#include <QString>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();         //- layer list -
    void TitleLayer();     //0 - 标题页
    void Unamelayer();     //1 - 用户名页
    void GameTypeLayer();  //2 - 游戏选择页
    void GameModeLayer();  //3 - 模式选择页
    void InterErrLayer();  //4 - 网络错误页
    void PlayerNumLayer(); //5 - 玩家数量页
    void Game_21();        //21点游戏
    void BetLayer_21();
    void SurrenderLayer_21();
    void StandLayer_21();
    void BankerLayer_21();
    void PassLayer_21();
    void Fin_21();
    QLabel* GetLabel(int i);
    void DrawCard(int i, Card c);
    void DrawBankerCard(Card bk);
    void CardIni();
private slots:

    void on_b_start_clicked();

    void on_b_go_clicked();

    void on_b_21_clicked();

    void on_b_tx_clicked();

    void on_b_local_clicked();

    void on_b_online_clicked();

    void on_b_back_clicked();

    void on_b_one_clicked();

    void on_b_two_clicked();

    void on_b_three_clicked();

    void on_b_four_clicked();

    void on_b_do_clicked();

    void on_b_skip_clicked();

    void on_b_add_clicked();

    void on_b_re_clicked();

    void on_b_menu_clicked();

    void on_b_help_clicked();

    void on_b_set_clicked();

    void on_b_continue_clicked();

    void on_b_main_clicked();

private:
    int layer{0};
    QString  uname{""};//用户名
    int player_num_{0};//玩家数量
    int ai_num_{0};     //AI数量
    int game_type_{0};
    /*
     * 游戏类型变量
     * 0 - 未选择
     * 1 - 21点
     * 2 - 德州扑克
     */
    int game_mode_{0};
    /*
     * 游戏模式变量
     * 0 - 未选择
     * 1 - 本地
     * 2 - 联机
     */
    int  layer_21{0};
    /* layer list
     * 0 - raise bet
     * 1 - surrender
     * 2 - stand
     */
    int cur_player{-1};
    int flag_regame{0};
    f_texas* TexasLayer{NULL};
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
