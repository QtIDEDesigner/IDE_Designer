#include "PlainTextEdit.h"

PlainTextEdit::PlainTextEdit(QPlainTextEdit *parent) : QPlainTextEdit(parent)
{
    QFont codeFont("Consolas", 12, 2);
//    this->setStyleSheet("QPlainTextEdit{background-color:rgb(255,255,255);border:none; border-radius:20px;color:rgb(93, 93, 94);"
//                            "selection-background-color:rgb(228, 228, 228);selection-color:rgb(147, 115, 238);"
//                            "padding: 20px;}");
    this->setFont(codeFont);
    this->setLineWrapMode(LineWrapMode::NoWrap);

    //
    codeLineArea = new CodeLineArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateCodeLineAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateCodeLineArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateCodeLineAreaWidth(0);
    highlightCurrentLine();
}
void PlainTextEdit::SendTextToFile(){//编辑发送文本内容至文件
    QString Text=this->toPlainText();
    emit(SendText(Text));
}

// codeLineAreaWidth Constructor
int PlainTextEdit::codeLineAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}


void PlainTextEdit::updateCodeLineAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(codeLineAreaWidth(), 0, 0, 0);
}


void PlainTextEdit::updateCodeLineArea(const QRect &rect, int dy)
{
    if (dy)
        codeLineArea->scroll(0, dy);
    else
        codeLineArea->update(0, rect.y(), codeLineArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateCodeLineAreaWidth(0);
}


void PlainTextEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    codeLineArea->setGeometry(QRect(cr.left(), cr.top(), codeLineAreaWidth(), cr.height()));
}

// resizeEvent
// cursorPositionChanged
void PlainTextEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lightPurple;
        lightPurple.setRgb(200, 200, 200);

        selection.format.setBackground(lightPurple);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}


void PlainTextEdit::codeLineAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(codeLineArea);
    QColor lightPurple;
    lightPurple.setRgb(200, 200, 200);
    QColor purple;
    purple.setRgb(93, 93, 94);
    painter.fillRect(event->rect(), lightPurple);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setFont(QFont("Consolas"));
            painter.setPen(purple);
            painter.drawText(0, top, codeLineArea->width(), fontMetrics().height(),
                             Qt::AlignCenter, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }

}
