
#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

// 编辑器部件

#include <QFont>
#include <QPlainTextEdit>
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
    PlainTextEdit(QWidget *parent = 0);

    void codeLineAreaPaintEvent(QPaintEvent *event);
    int codeLineAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateCodeLineAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateCodeLineArea(const QRect &, int);

private:
    QWidget *codeLineArea;
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
#endif // PLAINTEXTEDIT_H
