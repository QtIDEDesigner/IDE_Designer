#include "PlainTextEdit.h"
#include <QtWidgets>

// CodeHighlighting Constructor
PlainTextEdit::PlainTextEdit(QWidget *parent) : QPlainTextEdit(parent)
{

    codeLineArea = new CodeLineArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateCodeLineAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateCodeLineArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateCodeLineAreaWidth(0);
    highlightCurrentLine();

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

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

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

        QColor lineColor = QColor(Qt::gray).lighter(120);

        selection.format.setBackground(lineColor);
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
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, codeLineArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }

}
