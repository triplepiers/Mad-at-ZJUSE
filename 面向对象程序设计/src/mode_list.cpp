#include "mode_list.h"
#include "ui_mode_list.h"

mode_list::mode_list(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mode_list)
{
    ui->setupUi(this);

    //按下b_back时，释放back信号，返回game_list
    ui->setupUi(this);
    connect(ui->b_back, &QPushButton::clicked, [=](){
        emit this->back();
    });
}

mode_list::~mode_list()
{
    delete ui;
}
