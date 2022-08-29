#include "PlainTextEdit.h"
#include<QDebug>
static QVector<QPair<QString, QString>> parentheses = {
    {"(", ")"},
    {"{", "}"},
    {"[", "]"},
    {"\"", "\""},
    {"'", "'"}
};
PlainTextEdit::PlainTextEdit(QPlainTextEdit *parent) : QPlainTextEdit(parent)
{
    QFont codeFont("Consolas", 12, 2);
//    this->setStyleSheet("QPlainTextEdit{background-color:rgb(255,255,255);border:none; border-radius:20px;color:rgb(93, 93, 94);"
//                            "selection-background-color:rgb(228, 228, 228);selection-color:rgb(147, 115, 238);"
//                            "padding: 20px;}");
    this->setFont(codeFont);

    //
    codeLineArea = new CodeLineArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateCodeLineAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateCodeLineArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightParentheses()));

    updateCodeLineAreaWidth(0);
    highlightCurrentLine();

    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(updateExtraSelection()));
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

void PlainTextEdit::updateExtraSelection()
{
    QList<QTextEdit::ExtraSelection> extra;



    setExtraSelections(extra);
}

void PlainTextEdit::highlightParentheses()
{
    auto currentSymbol = charUnderCursor();
    auto prevSymbol = charUnderCursor(-1);
    QList<QTextEdit::ExtraSelection> extraSelection;

    for (auto& pair : parentheses)
    {
        int direction;

        QChar counterSymbol;
        QChar activeSymbol;
        auto position = textCursor().position();

        if (pair.first == currentSymbol)
        {
            direction = 1;
            counterSymbol = pair.second[0];
            activeSymbol = currentSymbol;
        }
        else if (pair.second == prevSymbol)
        {
            direction = -1;
            counterSymbol = pair.first[0];
            activeSymbol = prevSymbol;
            position--;
        }
        else
        {
            continue;
        }

        auto counter = 1;

        while (counter != 0 &&
               position > 0 &&
               position < (document()->characterCount() - 1))
        {
            //qDebug()<<position<<document()->characterCount();
            // Moving position
            position += direction;

            auto character = document()->characterAt(position);
            // Checking symbol under position
            if (character == activeSymbol)
            {
                ++counter;
            }
            else if (character == counterSymbol)
            {
                --counter;
            }
        }

        QColor color; color.setRgb(240,161,168);

        // Found
        if (counter == 0)
        {
            QTextEdit::ExtraSelection selection;

            auto directionEnum =
                 direction < 0 ?
                 QTextCursor::MoveOperation::Left
                 :
                 QTextCursor::MoveOperation::Right;
            selection.format.setFontWeight(5);
            selection.format.setBackground(color);
            selection.cursor = textCursor();
            selection.cursor.clearSelection();
            selection.cursor.movePosition(
                directionEnum,
                QTextCursor::MoveMode::MoveAnchor,
                std::abs(textCursor().position() - position)
            );

            selection.cursor.movePosition(
                QTextCursor::MoveOperation::Right,
                QTextCursor::MoveMode::KeepAnchor,
                1
            );

            extraSelection.append(selection);

            selection.cursor = textCursor();
            selection.cursor.clearSelection();
            selection.cursor.movePosition(
                directionEnum,
                QTextCursor::MoveMode::KeepAnchor,
                1
            );

            extraSelection.append(selection);
        }

        break;
    }
    setExtraSelections(extraSelection);
}

QChar PlainTextEdit::charUnderCursor(int offset) const
{
    auto block = textCursor().blockNumber();
    auto index = textCursor().positionInBlock();
    auto text = document()->findBlockByNumber(block).text();

    index += offset;

    if (index < 0 || index >= text.size())
    {
        return {};
    }

    return text[index];
}
