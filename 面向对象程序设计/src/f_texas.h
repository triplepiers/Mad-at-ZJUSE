#ifndef F_TEXAS_H
#define F_TEXAS_H

#include <QMainWindow>
#include "game_21.h"
#include "global.h"

namespace Ui {
class f_texas;
}

class f_texas : public QMainWindow
{
    Q_OBJECT

public:
    explicit f_texas(QWidget *parent = nullptr);
    void player_turn();
    void drawcard1(Card c);
    void drawcard2(Card c);
    void nextturn(int state);
    void DrawCard(int i, Card c);
    void fin(int type);
    void regame();
    ~f_texas();

private slots:

    void on_c_call_clicked();

    void on_c_bet_clicked();

    void on_c_flop_clicked();

    void on_b_set_clicked();
signals:
    void back();
private:
    Ui::f_texas *ui;
    int cur_player{0};
    int state{0};
};

#endif // F_TEXAS_H
