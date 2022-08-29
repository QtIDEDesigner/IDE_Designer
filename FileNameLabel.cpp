#include "FileNameLabel.h"

FileNameLabel::FileNameLabel(QString fileName, QLabel *parent):QLabel(parent)
{
    QFont fileFont("Microsoft YaHei", 12, 70);
    this->setText(fileName);
    this->setFont(fileFont);
    // this->setStyleSheet("QLabel{background-color: rgb(187, 160, 250);color:white;border:none;border-radius:15px}");
    this->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
}
