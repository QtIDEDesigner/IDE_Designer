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
#include <QTextCodec>   // 字符编码转换头文件
#include <QFileDialog>  // 文件对话框
#include "MoveWgt.h"
#include "DragWgt.h"
#include "MenuButton.h"
#include "MainMenu.h"
#include "ScreenButton.h"
#include "FileNameLabel.h"
#include "PlainTextEdit.h"
#include "FileList.h"
#include "HighLighter.h"

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

    QWidget *menuWidget; //标题栏
    QString fileName;//当前文件名
    FileNameLabel * fileNameLabel; //当前文件名Label

    PlainTextEdit *plainTextEdit; //当前文本框
    FileList *fileList; //当前文件列表
    MenuButton* fileButton; //文件按钮
    MainMenu* fileMenu; //文件菜单
    MenuButton* buildButton; //构建按钮
    MainMenu* buildMenu; //构建菜单
    MenuButton* themeButton; //主题按钮
    MainMenu* themeMenu; //主题菜单

    HighLighter *highlighter; // 语法高亮类

private:
    FramelessWindowPrivate *d;
private slots:

    void setPurple();
    void setDark();
    void setRed();
    void setOrange();
    void setGreen();
    void setBlue();

    void maxScreen();
    void setFileName(QString);
    void on_compileAction_triggered();
//    void on_saveFileAction_triggered();
    void on_runAction_triggered();
};

#endif // FRAMELESSWINDOW_H
