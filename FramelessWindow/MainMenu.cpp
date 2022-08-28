#include "MainMenu.h"

MainMenu::MainMenu(QMenu *parent) : QMenu(parent)
{
    QFont actionFont("Microsoft YaHei", 10, 75);
    this->setFont(actionFont);
    this->setWindowFlags(this->windowFlags()  | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground,true);
//    this->setStyleSheet("QMenu{background-color: rgb(187, 160, 250); border:none;border-radius: 10px;}"
//                       "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
//                       "QMenu::item:hover{background-color:rgb(147, 115, 238);}"
//                       "QMenu::item:selected{background-color:rgb(147, 115, 238);}");
}
