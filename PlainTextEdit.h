#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

// 编辑器部件

#include <QFont>
#include <QPlainTextEdit>
#include <QPainter>
#include <QTextBlock>
#include "comleter.h"

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class CodeLineArea;


class PlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    PlainTextEdit(QPlainTextEdit *parent = 0);

    void codeLineAreaPaintEvent(QPaintEvent *event);

    int codeLineAreaWidth();

    void setUpCompleteList();

    //QList<QTextEdit::ExtraSelection> extra;
public slots:
    void SendTextToFile();
signals:
    SendText(QString Text);//发送文本给文件保存函数
protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateCodeLineAreaWidth(int newBlockCount);
    void highlightCurrentLine(QList<QTextEdit::ExtraSelection>&);
    void updateCodeLineArea(const QRect &, int);
    void highlightParentheses(QList<QTextEdit::ExtraSelection>&);
    QChar charUnderCursor(int offset=0)const;
    void updateExtraSelection();
    void showCompleteWidget();
private:
    QWidget *codeLineArea;
    QStringList completeList;//储存自动填充的关键字
    Comleter *completeWidget; //临时加载匹配的关键字
    int completeState;
    int getCompleteWidgetX();
    QString getWordOfCursor();
};

// Code line Area & Width Definition
class CodeLineArea : public QWidget
{
public:
    CodeLineArea(PlainTextEdit *editor) : QWidget(editor) {
        codeHighlighting = editor;
    }

    QSize sizeHint() const override {
        return QSize(codeHighlighting->codeLineAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeHighlighting->codeLineAreaPaintEvent(event);
    }

private:
    PlainTextEdit *codeHighlighting;
};
enum CompleteState{
  Ignore=0,
  Showing=1,
  Hide=2
};
#endif // PLAINTEXTEDIT_H



