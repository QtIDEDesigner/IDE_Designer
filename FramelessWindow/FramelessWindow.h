#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include <QGraphicsDropShadowEffect>
#include <QMenu>
#include <QFont>
#include <QAction>
#include <QPlainTextEdit>
#include "MoveWgt.h"
#include "DragWgt.h"
#include "MenuButton.h"
#include "MainMenu.h"
#include "ScreenButton.h"
#include "FileNameLabel.h"
#include "PlainTextEdit.h"
#include "FileList.h"
#include"fileoperation.h"
#include <stdio.h>
#include <QMainWindow>
// 主体窗口

class FramelessWindowPrivate;

class FramelessWindow : public QWidget {
    Q_OBJECT
    Move_Wgt
    Drag_Wgt

public:
    explicit FramelessWindow(QWidget *parent = 0);
    ~FramelessWindow();

    QGridLayout *lo; //阴影部分

    QString fileName;//当前文件名
    FileNameLabel * fileNameLabel; //当前文件名Label

    PlainTextEdit *plainTextEdit; //当前文本框
    FileList *fileList; //当前文件列表



private:
    FramelessWindowPrivate *d;
private slots:
    void maxScreen();
    void setFileName(QString);
    void on_compileAction_triggered();
    void on_saveFileAction_triggered();
    void on_runAction_triggered();
};

#endif // FRAMELESSWINDOW_H
