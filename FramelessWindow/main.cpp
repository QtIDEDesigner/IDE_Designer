#include "FramelessWindow.h"

#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QDesktopWidget>
#include <QVBoxLayout>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();

//    QString qss;
//    QFile qssFile(":/qss/purple-light.qss");
//    qssFile.open(QFile::ReadOnly);

//    if (qssFile.isOpen())
//    {
//        qss = QLatin1String(qssFile.readAll());
//        app.setStyleSheet(qss);
//        qssFile.close();
//    }


    // 创建窗口
    FramelessWindow *window = new FramelessWindow();
    window->resize(width / 3 * 2, height / 3 * 2);
    window->show();

    return app.exec();
}
