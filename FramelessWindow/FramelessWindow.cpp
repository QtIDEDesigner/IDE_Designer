#include "FramelessWindow.h"


QString path;
QTextCodec *codec;

class FramelessWindowPrivate {
public:
    FramelessWindowPrivate(QWidget *contentWidget) : contentWidget(contentWidget) {}

    QWidget *contentWidget;
};

FramelessWindow::FramelessWindow(QWidget *parent) : QWidget(parent) {

    setWindowFlags(Qt::FramelessWindowHint);    // 去掉边框
    setAttribute(Qt::WA_TranslucentBackground); // 背景透明
    setMouseTracking(true); // 随时监测鼠标位置

    //ScreenButton类中包含左上角三个按钮，分别控制关闭窗口，窗口最小化和窗口最大化
    ScreenButton *screenButton = new ScreenButton();
    // 连接三个按钮的槽函数
    connect(screenButton->quitButton, &QPushButton::clicked, this, &QWidget::close);
    connect(screenButton->minButton, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(screenButton->maxButton, &QPushButton::clicked, this, &FramelessWindow::maxScreen);

    // 左侧的文件列表
    fileList = new FileList();
    // 右侧的编辑器
    plainTextEdit = new PlainTextEdit();
    // 设置编辑器语法高亮
    highlighter = new HighLighter(plainTextEdit -> document());

    // 测试用，显示一个文件名
    //setFileName("FileName.c");
    fileName="未命名";
    // 顶端中部显示文件名
    fileNameLabel = new FileNameLabel(fileName);


    // 创建MenuButton类对象，分别对应每一个菜单栏按钮
    fileButton = new MenuButton("   Files");
    // 创建MainMenu类对象，对应一个菜单
    fileMenu = new MainMenu();
    fileButton->setMenu(fileMenu);
    // 菜单里的数个选项
    //add,新建文件
    QAction* newFileAction = new QAction(this);
    fileMenu->addAction(newFileAction);
    newFileAction->setText(tr("New"));
    //
    QAction* openFileAction = new QAction(this);
    fileMenu->addAction(openFileAction);
    openFileAction->setText(tr("Open"));
    QAction* closeFileAction = new QAction(this);
    fileMenu->addAction(closeFileAction);
    closeFileAction->setText(tr("Close"));
    QAction* saveFileAction = new QAction(this);
    fileMenu->addAction(saveFileAction);
    saveFileAction->setText(tr("Save"));
    QAction* saveAsAction = new QAction(this);
    fileMenu->addAction(saveAsAction);
    saveAsAction->setText(tr("Save As"));
    connect(newFileAction, SIGNAL(triggered()), fileList, SLOT(NewFile()));//新建
    connect(openFileAction, SIGNAL(triggered()), fileList, SLOT(OpenFile()));//打开
    connect(closeFileAction, SIGNAL(triggered()), fileList, SLOT(Close()));//关闭
    connect(saveFileAction, SIGNAL(triggered()), fileList, SLOT(Save()));//保存
    connect(saveAsAction, SIGNAL(triggered()), fileList, SLOT(SaveAs()));//另存

    connect(plainTextEdit,SIGNAL(textChanged()),fileList,SLOT(SetCurrentFilemodified()));//编辑区做出修改

    connect(fileList,SIGNAL(LoadData(QString)),plainTextEdit,SLOT(setPlainText(QString)));//打开文件
    connect(fileList,SIGNAL(ClearText()),plainTextEdit,SLOT(clear()));//关闭或新建
    connect(fileList,SIGNAL(GetText()),plainTextEdit,SLOT(SendTextToFile()));//保存文件，需要编辑区发送文本
    connect(plainTextEdit,SIGNAL(SendText(QString)),fileList,SLOT(GetPlainText(QString)));//编辑区发送文本内容
    connect(fileList,SIGNAL(SetTitle(QString)),this,SLOT(setFileName(QString)));//设置当前文件名

    // 创建MenuButton类对象，分别对应每一个菜单栏按钮
    buildButton = new MenuButton("  Build");
    // 创建MainMenu类对象，对应一个菜单
    buildMenu = new MainMenu();
    buildButton->setMenu(buildMenu);
    // 菜单里的数个选项
    QAction* compileAction = new QAction(this);
    buildMenu->addAction(compileAction);
    compileAction->setText(tr("Compile"));
    connect(compileAction, SIGNAL(triggered()), this, SLOT(on_compileAction_triggered()));
    QAction* runAction = new QAction(this);
    buildMenu->addAction(runAction);
    runAction->setText(tr("Run"));
    connect(runAction, SIGNAL(triggered()), this, SLOT(on_runAction_triggered()));

    themeButton = new MenuButton("  Theme");
    themeMenu = new MainMenu();
    themeButton->setMenu(themeMenu);
    // 菜单里的数个选项
    QAction* purpleAction = new QAction(this);
    themeMenu->addAction(purpleAction);
    purpleAction->setText(tr("Purple"));
    connect(purpleAction, SIGNAL(triggered()), this, SLOT(setPurple()));
    QAction* darkAction = new QAction(this);
    themeMenu->addAction(darkAction);
    darkAction->setText(tr("Dark"));
    connect(darkAction, SIGNAL(triggered()), this, SLOT(setDark()));
    QAction* redAction = new QAction(this);
    themeMenu->addAction(redAction);
    redAction->setText(tr("Red"));
    connect(redAction, SIGNAL(triggered()), this, SLOT(setRed()));
    QAction* orangeAction = new QAction(this);
    themeMenu->addAction(orangeAction);
    orangeAction->setText(tr("Orange"));
    connect(orangeAction, SIGNAL(triggered()), this, SLOT(setOrange()));
    QAction* greenAction = new QAction(this);
    themeMenu->addAction(greenAction);
    greenAction->setText(tr("Green"));
    connect(greenAction, SIGNAL(triggered()), this, SLOT(setGreen()));
    QAction* blueAction = new QAction(this);
    themeMenu->addAction(blueAction);
    blueAction->setText(tr("Blue"));
    connect(blueAction, SIGNAL(triggered()), this, SLOT(setBlue()));




    // 设计整体布局
    QVBoxLayout *layout = new QVBoxLayout();
    // 整个的标题题栏底
    menuWidget = new QWidget();
    QHBoxLayout *menuLayout = new QHBoxLayout();
    QWidget *emptyWidget = new QWidget();
    // 将标题栏所有部件放到布局中
    menuLayout->addWidget(screenButton->quitButton);
    menuLayout->addWidget(screenButton->minButton);
    menuLayout->addWidget(screenButton->maxButton);
    menuLayout->addWidget(emptyWidget);
    menuLayout->addWidget(fileNameLabel);
    menuLayout->addWidget(emptyWidget);
    menuLayout->addWidget(themeButton);
    menuLayout->addWidget(buildButton);
    menuLayout->addWidget(fileButton);
    menuWidget->setLayout(menuLayout);

    layout->addWidget(menuWidget);


    // 编辑区布局，包括文件列表和编辑器
    QHBoxLayout *editRegion = new QHBoxLayout();
    editRegion->addWidget(fileList);
    editRegion->addWidget(plainTextEdit);
    editRegion->setStretch(0, 2);
    editRegion->setStretch(1, 8);

    layout->addLayout(editRegion);

    QWidget *contentWidget = new QWidget();
    //此处为当前窗口，传递到Frameless窗口
    contentWidget->setLayout(layout);
    contentWidget->setObjectName("contentWidget");
    contentWidget->setStyleSheet("background-color: rgba(228, 228, 228, 150); border-radius: 20px;");

    setPurple();

    d = new FramelessWindowPrivate(contentWidget);

    // 添加阴影
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(contentWidget);
    shadowEffect->setColor(Qt::gray);
    shadowEffect->setBlurRadius(15); // 阴影的大小
    shadowEffect->setOffset(0, 0);
    contentWidget->setGraphicsEffect(shadowEffect);

    // 添加到窗口中
    lo = new QGridLayout();
    lo->addWidget(contentWidget, 0, 0);
    lo->setContentsMargins(10, 10, 10, 10); // 注意和阴影大小的协调
    setLayout(lo);
    codec = QTextCodec::codecForName("GBK");
}

FramelessWindow::~FramelessWindow() {
    delete d;
}


//实现最大化和标准大小的窗口切换
void FramelessWindow::maxScreen()
{
    if (this->isMaximized()) {
        lo->setContentsMargins(10, 10, 10, 10);
        this->showNormal();
    } else {
        lo->setContentsMargins(0, 0, 0, 0);
        this->showMaximized();
    }
}

// 文件名的setter
void FramelessWindow::setFileName(QString name)
{
    this->fileNameLabel->setText(name);
}

//编译
void FramelessWindow::on_compileAction_triggered()
{
    //保存文件
    // this->on_saveFileAction_triggered();
    fileList->Save();
    QString demo = fileList->currentFile;
    // 生成的目标文件名
    demo.replace(".c", "");
    // gcc filename.c -o filename
    QString cmd = QString("gcc %1 -o %2").arg(fileList->currentFile).arg(demo);
    system(codec->fromUnicode(cmd).data());
    cmd = QString("gcc %1.o -o %2").arg(demo).arg(demo);
    system(codec->fromUnicode(cmd).data());
}

//编译并执行
void FramelessWindow::on_runAction_triggered()
{
    //保存文件
    // this->on_saveFileAction_triggered();
    fileList->Save();
    QString demo = fileList->currentFile;
    // 生成的目标文件名
    demo.replace(".c", "");

    // gcc filename.c -o filename
    QString cmd = QString("gcc %1 -o %2").arg(fileList->currentFile).arg(demo);

    // system执行成返回0
    int ret = system(codec->fromUnicode(cmd).data());
    if(ret != 0)
    {
        // cmd /k 停留在终端
        cmd = QString("cmd /k gcc %1 -o %2").arg(fileList->currentFile).arg(demo);
        system(codec->fromUnicode(cmd).data());
        return;
    }
    QString target = QString("cmd /k %1").arg(demo);
    system(codec->fromUnicode(target).data());
}

//保存文件
//void FramelessWindow::on_saveFileAction_triggered()
//{
//    // 如果刚打开编辑器开始写东西，此时path为空就需要通过getSaveFileName获取一个路径
//    if(path.isEmpty())
//    {
//        path = QFileDialog::getSaveFileName();
//    }
//    const char *fileName = codec->fromUnicode(path).data();
//    // 打开文件并读取内容,并放进编辑区
//    FILE *fp = fopen(fileName, "wb");
//    if(fp == NULL)
//    {
//        qDebug() << "on_actionSAVE_triggered open file err";
//    }
//    // 获取编辑区的内容，返回QString
//    QString str=this->plainTextEdit->toPlainText();
//    // 将QString转换为char * 类型
//    const char *buf = str.toStdString().data();
//    fputs(buf, fp);
//    // 关闭文件
//    fclose(fp);
//}

void FramelessWindow::setPurple()
{
    menuWidget->setStyleSheet("border:none; border-radius:20px;background-color:rgb(147, 115, 238)");

    fileList->setStyleSheet(
                    "QListWidget {background-color:rgb(242, 239, 249);outline: 0px;padding: 10px;border: none;border-radius: 20px }"
                    "QListWidget::item {border: none;border-radius: 15px;padding: 10px;color: rgb(93, 93, 94) }"
                    "QListWidget::item:hover {background: rgb(187, 160, 250)}"
                    "QListWidget::item:selected { background: rgb(147, 115, 238);color: white}"
                    );

    fileList->verticalScrollBar()->setStyleSheet(
                    "QScrollBar:vertical{background-color:none;width:12px;border-radius:6px}"
                    "QScrollBar::handle:vertical{background:rgb(187, 160, 250);border-radius:6px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(147, 115, 238);border-radius:6px;min-height:20;}"
                    "QScrollBar::add-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{width:0px;background:rgb(242, 239, 249);}"
                    );
    fileList->horizontalScrollBar()->setStyleSheet(
                    "QScrollBar:horizontal{background:none;height:12px;border-radius:6px}"
                    "QScrollBar::handle:horizontal{background:rgb(187, 160, 250);border-radius:6px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(147, 115, 238);border-radius:6px;min-width:20;}"
                    "QScrollBar::add-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::sub-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{height:0px;background:rgb(242, 239, 249);}"
                    );
    fileNameLabel->setStyleSheet("QLabel{background-color: rgb(187, 160, 250);color:white;border:none;border-radius:15px}");

    fileMenu->setStyleSheet("QMenu{background-color: rgb(187, 160, 250); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(147, 115, 238);}"
                        "QMenu::item:selected{background-color:rgb(147, 115, 238);}");

    buildMenu->setStyleSheet("QMenu{background-color: rgb(187, 160, 250); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(147, 115, 238);}"
                        "QMenu::item:selected{background-color:rgb(147, 115, 238);}");

    themeMenu->setStyleSheet("QMenu{background-color: rgb(187, 160, 250); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(147, 115, 238);}"
                        "QMenu::item:selected{background-color:rgb(147, 115, 238);}");

    fileButton->setStyleSheet("QPushButton{background-color: rgb(147, 115, 238);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(187, 160, 250)}"
                        "QPushButton::pressed{background-color: rgb(187, 160, 250)}"
                        "QPushButton::menu-indicator{ image:none;}");

    buildButton->setStyleSheet("QPushButton{background-color: rgb(147, 115, 238);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(187, 160, 250)}"
                        "QPushButton::pressed{background-color: rgb(187, 160, 250)}"
                        "QPushButton::menu-indicator{ image:none;}");

    themeButton->setStyleSheet("QPushButton{background-color: rgb(147, 115, 238);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(187, 160, 250)}"
                        "QPushButton::pressed{background-color: rgb(187, 160, 250)}"
                        "QPushButton::menu-indicator{ image:none;}");

    plainTextEdit->setStyleSheet("QPlainTextEdit{background-color:rgb(255, 255, 255);border:none; border-radius:20px;color:rgb(93, 93, 94);"
                        "selection-background-color:rgb(242, 239, 249);selection-color:rgb(147, 115, 238);"
                        "padding: 20px;}");
    plainTextEdit->verticalScrollBar()->setStyleSheet(
                    "QScrollBar:vertical{background-color:none;width:12px;border-radius:6px}"
                    "QScrollBar::handle:vertical{background:rgb(187, 160, 250);border-radius:6px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(147, 115, 238);border-radius:6px;min-height:20;}"
                    "QScrollBar::add-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{width:0px;background:rgb(255, 255, 255);}"
                    );
    plainTextEdit->horizontalScrollBar()->setStyleSheet(
                    "QScrollBar:horizontal{background:none;height:12px;border-radius:6px}"
                    "QScrollBar::handle:horizontal{background:rgb(187, 160, 250);border-radius:6px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(147, 115, 238);border-radius:6px;min-width:20;}"
                    "QScrollBar::add-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::sub-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{height:0px;background:rgb(255, 255, 255);}"
                    );
}

void FramelessWindow::setDark()
{
    menuWidget->setStyleSheet("border:none; border-radius:20px;background-color:rgb(75,75,75)");

    fileList->setStyleSheet(
                    "QListWidget {background-color:rgb(150, 150, 150);outline: 0px;padding: 10px;border: none;border-radius: 20px }"
                    "QListWidget::item {border: none;border-radius: 15px;padding: 10px;color: rgb(255, 255, 255) }"
                    "QListWidget::item:hover {background: rgb(110, 110, 110)}"
                    "QListWidget::item:selected { background: rgb(75, 75, 75);color: white}"
                    );

    fileList->verticalScrollBar()->setStyleSheet(
                    "QScrollBar:vertical{background-color:none;width:12px;border-radius:6px}"
                    "QScrollBar::handle:vertical{background:rgb(110, 110, 110);border-radius:6px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(75, 75, 75);border-radius:6px;min-height:20;}"
                    "QScrollBar::add-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{width:0px;background:rgb(150, 150, 150);}"
                    );
    fileList->horizontalScrollBar()->setStyleSheet(
                    "QScrollBar:horizontal{background:none;height:12px;border-radius:6px}"
                    "QScrollBar::handle:horizontal{background:rgb(110, 110, 110);border-radius:6px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(75, 75, 75);border-radius:6px;min-width:20;}"
                    "QScrollBar::add-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::sub-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{height:0px;background:rgb(150, 150, 150);}"
                    );
    fileNameLabel->setStyleSheet("QLabel{background-color: rgb(110, 110, 110);color:white;border:none;border-radius:15px}");

    fileMenu->setStyleSheet("QMenu{background-color: rgb(110, 110, 110); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(75, 75, 75);}"
                        "QMenu::item:selected{background-color:rgb(75, 75, 75);}");

    buildMenu->setStyleSheet("QMenu{background-color: rgb(110, 110, 110); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(75, 75, 75);}"
                        "QMenu::item:selected{background-color:rgb(75, 75, 75);}");

    themeMenu->setStyleSheet("QMenu{background-color: rgb(110, 110, 110); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(75, 75, 75);}"
                        "QMenu::item:selected{background-color:rgb(75, 75, 75);}");

    fileButton->setStyleSheet("QPushButton{background-color: rgb(75, 75, 75);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(110, 110, 110)}"
                        "QPushButton::pressed{background-color: rgb(110, 110, 110)}"
                        "QPushButton::menu-indicator{ image:none;}");

    buildButton->setStyleSheet("QPushButton{background-color: rgb(75, 75, 75);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(110, 110, 110)}"
                        "QPushButton::pressed{background-color: rgb(110, 110, 110)}"
                        "QPushButton::menu-indicator{ image:none;}");

    themeButton->setStyleSheet("QPushButton{background-color: rgb(75, 75, 75);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(110, 110, 110)}"
                        "QPushButton::pressed{background-color: rgb(110, 110, 110)}"
                        "QPushButton::menu-indicator{ image:none;}");

    plainTextEdit->setStyleSheet("QPlainTextEdit{background-color:rgb(100, 100, 100);border:none; border-radius:20px;color:rgb(255, 255, 255);"
                        "selection-background-color:rgb(150,150,150);selection-color:rgb(75, 75, 75);"
                        "padding: 20px;}");
    plainTextEdit->verticalScrollBar()->setStyleSheet(
                    "QScrollBar:vertical{background-color:none;width:12px;border-radius:6px}"
                    "QScrollBar::handle:vertical{background:rgb(110, 110, 110);border-radius:6px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(75, 75, 75);border-radius:6px;min-height:20;}"
                    "QScrollBar::add-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{width:0px;background:rgb(100, 100, 100);}"
                    );
    plainTextEdit->horizontalScrollBar()->setStyleSheet(
                    "QScrollBar:horizontal{background:none;height:12px;border-radius:6px}"
                    "QScrollBar::handle:horizontal{background:rgb(110, 110, 110);border-radius:6px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(75, 75, 75);border-radius:6px;min-width:20;}"
                    "QScrollBar::add-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::sub-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{height:0px;background:rgb(100, 100, 100);}"
                    );
}

void FramelessWindow::setRed()
{
    menuWidget->setStyleSheet("border:none; border-radius:20px;background-color:rgb(253, 104, 116)");

    fileList->setStyleSheet(
                    "QListWidget {background-color:rgb(250, 238, 239);outline: 0px;padding: 10px;border: none;border-radius: 20px }"
                    "QListWidget::item {border: none;border-radius: 15px;padding: 10px;color: rgb(93, 93, 94) }"
                    "QListWidget::item:hover {background: rgb(253, 142, 151)}"
                    "QListWidget::item:selected { background: rgb(253, 104, 116);color: white}"
                    );

    fileList->verticalScrollBar()->setStyleSheet(
                    "QScrollBar:vertical{background-color:none;width:12px;border-radius:6px}"
                    "QScrollBar::handle:vertical{background:rgb(253, 142, 151);border-radius:6px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(253, 104, 116);border-radius:6px;min-height:20;}"
                    "QScrollBar::add-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{width:0px;background:rgb(250, 238, 239);}"
                    );
    fileList->horizontalScrollBar()->setStyleSheet(
                    "QScrollBar:horizontal{background:none;height:12px;border-radius:6px}"
                    "QScrollBar::handle:horizontal{background:rgb(253, 142, 151);border-radius:6px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(253, 104, 116);border-radius:6px;min-width:20;}"
                    "QScrollBar::add-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::sub-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{height:0px;background:rgb(250, 238, 239);}"
                    );
    fileNameLabel->setStyleSheet("QLabel{background-color: rgb(253, 142, 151);color:white;border:none;border-radius:15px}");

    fileMenu->setStyleSheet("QMenu{background-color: rgb(253, 142, 151); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(253, 104, 116);}"
                        "QMenu::item:selected{background-color:rgb(253, 104, 116);}");

    buildMenu->setStyleSheet("QMenu{background-color: rgb(253, 142, 151); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(253, 104, 116);}"
                        "QMenu::item:selected{background-color:rgb(253, 104, 116);}");

    themeMenu->setStyleSheet("QMenu{background-color: rgb(253, 142, 151); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(253, 104, 116);}"
                        "QMenu::item:selected{background-color:rgb(253, 104, 116);}");

    fileButton->setStyleSheet("QPushButton{background-color: rgb(253, 104, 116);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(253, 142, 151)}"
                        "QPushButton::pressed{background-color: rgb(253, 142, 151)}"
                        "QPushButton::menu-indicator{ image:none;}");

    buildButton->setStyleSheet("QPushButton{background-color: rgb(253, 104, 116);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(253, 142, 151)}"
                        "QPushButton::pressed{background-color: rgb(253, 142, 151)}"
                        "QPushButton::menu-indicator{ image:none;}");

    themeButton->setStyleSheet("QPushButton{background-color: rgb(253, 104, 116);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(253, 142, 151)}"
                        "QPushButton::pressed{background-color: rgb(253, 142, 151)}"
                        "QPushButton::menu-indicator{ image:none;}");

    plainTextEdit->setStyleSheet("QPlainTextEdit{background-color:rgb(255, 255, 255);border:none; border-radius:20px;color:rgb(93, 93, 94);"
                        "selection-background-color:rgb(250, 238, 239);selection-color:rgb(253, 104, 116);"
                        "padding: 20px;}");
    plainTextEdit->verticalScrollBar()->setStyleSheet(
                    "QScrollBar:vertical{background-color:none;width:12px;border-radius:6px}"
                    "QScrollBar::handle:vertical{background:rgb(253, 142, 151);border-radius:6px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(253, 104, 116);border-radius:6px;min-height:20;}"
                    "QScrollBar::add-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{width:0px;background:rgb(255, 255, 255);}"
                    );
    plainTextEdit->horizontalScrollBar()->setStyleSheet(
                    "QScrollBar:horizontal{background:none;height:12px;border-radius:6px}"
                    "QScrollBar::handle:horizontal{background:rgb(253, 142, 151);border-radius:6px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(253, 104, 116);border-radius:6px;min-width:20;}"
                    "QScrollBar::add-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::sub-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{height:0px;background:rgb(255, 255, 255);}"
                    );
}

void FramelessWindow::setOrange()
{
    menuWidget->setStyleSheet("border:none; border-radius:20px;background-color:rgb(250, 140, 22)");

    fileList->setStyleSheet(
                    "QListWidget {background-color:rgb(250, 241, 232);outline: 0px;padding: 10px;border: none;border-radius: 20px }"
                    "QListWidget::item {border: none;border-radius: 15px;padding: 10px;color: rgb(93, 93, 94) }"
                    "QListWidget::item:hover {background: rgb(250, 164, 117)}"
                    "QListWidget::item:selected { background: rgb(250, 140, 22);color: white}"
                    );

    fileList->verticalScrollBar()->setStyleSheet(
                    "QScrollBar:vertical{background-color:none;width:12px;border-radius:6px}"
                    "QScrollBar::handle:vertical{background:rgb(250, 164, 117);border-radius:6px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(250, 140, 22);border-radius:6px;min-height:20;}"
                    "QScrollBar::add-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{width:0px;background:rgb(250, 241, 232);}"
                    );
    fileList->horizontalScrollBar()->setStyleSheet(
                    "QScrollBar:horizontal{background:none;height:12px;border-radius:6px}"
                    "QScrollBar::handle:horizontal{background:rgb(250, 164, 117);border-radius:6px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(250, 140, 22);border-radius:6px;min-width:20;}"
                    "QScrollBar::add-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::sub-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{height:0px;background:rgb(250, 241, 232);}"
                    );
    fileNameLabel->setStyleSheet("QLabel{background-color: rgb(250, 164, 117);color:white;border:none;border-radius:15px}");

    fileMenu->setStyleSheet("QMenu{background-color: rgb(250, 164, 117); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(250, 140, 22);}"
                        "QMenu::item:selected{background-color:rgb(250, 140, 22);}");

    buildMenu->setStyleSheet("QMenu{background-color: rgb(250, 164, 117); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(250, 140, 22);}"
                        "QMenu::item:selected{background-color:rgb(250, 140, 22);}");

    themeMenu->setStyleSheet("QMenu{background-color: rgb(250, 164, 117); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(250, 140, 22);}"
                        "QMenu::item:selected{background-color:rgb(250, 140, 22);}");

    fileButton->setStyleSheet("QPushButton{background-color: rgb(250, 140, 22);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(250, 164, 117)}"
                        "QPushButton::pressed{background-color: rgb(250, 164, 117)}"
                        "QPushButton::menu-indicator{ image:none;}");

    buildButton->setStyleSheet("QPushButton{background-color: rgb(250, 140, 22);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(250, 164, 117)}"
                        "QPushButton::pressed{background-color: rgb(250, 164, 117)}"
                        "QPushButton::menu-indicator{ image:none;}");

    themeButton->setStyleSheet("QPushButton{background-color: rgb(250, 140, 22);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(250, 164, 117)}"
                        "QPushButton::pressed{background-color: rgb(250, 164, 117)}"
                        "QPushButton::menu-indicator{ image:none;}");

    plainTextEdit->setStyleSheet("QPlainTextEdit{background-color:rgb(255, 255, 255);border:none; border-radius:20px;color:rgb(93, 93, 94);"
                        "selection-background-color:rgb(250, 241, 232);selection-color:rgb(250, 140, 22);"
                        "padding: 20px;}");
    plainTextEdit->verticalScrollBar()->setStyleSheet(
                    "QScrollBar:vertical{background-color:none;width:12px;border-radius:6px}"
                    "QScrollBar::handle:vertical{background:rgb(250, 164, 117);border-radius:6px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(250, 140, 22);border-radius:6px;min-height:20;}"
                    "QScrollBar::add-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{width:0px;background:rgb(255, 255, 255);}"
                    );
    plainTextEdit->horizontalScrollBar()->setStyleSheet(
                    "QScrollBar:horizontal{background:none;height:12px;border-radius:6px}"
                    "QScrollBar::handle:horizontal{background:rgb(250, 164, 117);border-radius:6px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(250, 140, 22);border-radius:6px;min-width:20;}"
                    "QScrollBar::add-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::sub-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{height:0px;background:rgb(255, 255, 255);}"
                    );

}

void FramelessWindow::setGreen()
{
    menuWidget->setStyleSheet("border:none; border-radius:20px;background-color:rgb(3, 158, 116)");

    fileList->setStyleSheet(
                    "QListWidget {background-color:rgb(230, 242, 239);outline: 0px;padding: 10px;border: none;border-radius: 20px }"
                    "QListWidget::item {border: none;border-radius: 15px;padding: 10px;color: rgb(93, 93, 94) }"
                    "QListWidget::item:hover {background: rgb(46, 178, 151)}"
                    "QListWidget::item:selected { background: rgb(3, 158, 116);color: white}"
                    );

    fileList->verticalScrollBar()->setStyleSheet(
                    "QScrollBar:vertical{background-color:none;width:12px;border-radius:6px}"
                    "QScrollBar::handle:vertical{background:rgb(46, 178, 151);border-radius:6px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(3, 158, 116);border-radius:6px;min-height:20;}"
                    "QScrollBar::add-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{width:0px;background:rgb(230, 242, 239);}"
                    );
    fileList->horizontalScrollBar()->setStyleSheet(
                    "QScrollBar:horizontal{background:none;height:12px;border-radius:6px}"
                    "QScrollBar::handle:horizontal{background:rgb(46, 178, 151);border-radius:6px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(3, 158, 116);border-radius:6px;min-width:20;}"
                    "QScrollBar::add-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::sub-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{height:0px;background:rgb(230, 242, 239);}"
                    );
    fileNameLabel->setStyleSheet("QLabel{background-color: rgb(46, 178, 151);color:white;border:none;border-radius:15px}");

    fileMenu->setStyleSheet("QMenu{background-color: rgb(46, 178, 151); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(3, 158, 116);}"
                        "QMenu::item:selected{background-color:rgb(3, 158, 116);}");

    buildMenu->setStyleSheet("QMenu{background-color: rgb(46, 178, 151); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(3, 158, 116);}"
                        "QMenu::item:selected{background-color:rgb(3, 158, 116);}");

    themeMenu->setStyleSheet("QMenu{background-color: rgb(46, 178, 151); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(3, 158, 116);}"
                        "QMenu::item:selected{background-color:rgb(3, 158, 116);}");

    fileButton->setStyleSheet("QPushButton{background-color: rgb(3, 158, 116);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(46, 178, 151)}"
                        "QPushButton::pressed{background-color: rgb(46, 178, 151)}"
                        "QPushButton::menu-indicator{ image:none;}");

    buildButton->setStyleSheet("QPushButton{background-color: rgb(3, 158, 116);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(46, 178, 151)}"
                        "QPushButton::pressed{background-color: rgb(46, 178, 151)}"
                        "QPushButton::menu-indicator{ image:none;}");

    themeButton->setStyleSheet("QPushButton{background-color: rgb(3, 158, 116);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(46, 178, 151)}"
                        "QPushButton::pressed{background-color: rgb(46, 178, 151)}"
                        "QPushButton::menu-indicator{ image:none;}");

    plainTextEdit->setStyleSheet("QPlainTextEdit{background-color:rgb(255, 255, 255);border:none; border-radius:20px;color:rgb(93, 93, 94);"
                        "selection-background-color:rgb(230, 242, 239);selection-color:rgb(3, 158, 116);"
                        "padding: 20px;}");
    plainTextEdit->verticalScrollBar()->setStyleSheet(
                    "QScrollBar:vertical{background-color:none;width:12px;border-radius:6px}"
                    "QScrollBar::handle:vertical{background:rgb(46, 178, 151);border-radius:6px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(3, 158, 116);border-radius:6px;min-height:20;}"
                    "QScrollBar::add-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{width:0px;background:rgb(255, 255, 255);}"
                    );
    plainTextEdit->horizontalScrollBar()->setStyleSheet(
                    "QScrollBar:horizontal{background:none;height:12px;border-radius:6px}"
                    "QScrollBar::handle:horizontal{background:rgb(46, 178, 151);border-radius:6px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(3, 158, 116);border-radius:6px;min-width:20;}"
                    "QScrollBar::add-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::sub-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{height:0px;background:rgb(255, 255, 255);}"
                    );
}

void FramelessWindow::setBlue()
{
    menuWidget->setStyleSheet("border:none; border-radius:20px;background-color:rgb(0, 195, 238)");

    fileList->setStyleSheet(
                    "QListWidget {background-color:rgb(230, 245, 249);outline: 0px;padding: 10px;border: none;border-radius: 20px }"
                    "QListWidget::item {border: none;border-radius: 15px;padding: 10px;color: rgb(93, 93, 94) }"
                    "QListWidget::item:hover {background: rgb(104, 215, 243)}"
                    "QListWidget::item:selected { background: rgb(0, 195, 238);color: white}"
                    );

    fileList->verticalScrollBar()->setStyleSheet(
                    "QScrollBar:vertical{background-color:none;width:12px;border-radius:6px}"
                    "QScrollBar::handle:vertical{background:rgb(104, 215, 243);border-radius:6px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(0, 195, 238);border-radius:6px;min-height:20;}"
                    "QScrollBar::add-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{width:0px;background:rgb(230, 245, 249);}"
                    );
    fileList->horizontalScrollBar()->setStyleSheet(
                    "QScrollBar:horizontal{background:none;height:12px;border-radius:6px}"
                    "QScrollBar::handle:horizontal{background:rgb(104, 215, 243);border-radius:6px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(0, 195, 238);border-radius:6px;min-width:20;}"
                    "QScrollBar::add-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::sub-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{height:0px;background:rgb(230, 245, 249);}"
                    );
    fileNameLabel->setStyleSheet("QLabel{background-color: rgb(104, 215, 243);color:white;border:none;border-radius:15px}");

    fileMenu->setStyleSheet("QMenu{background-color: rgb(104, 215, 243); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(0, 195, 238);}"
                        "QMenu::item:selected{background-color:rgb(0, 195, 238);}");

    buildMenu->setStyleSheet("QMenu{background-color: rgb(104, 215, 243); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(0, 195, 238);}"
                        "QMenu::item:selected{background-color:rgb(0, 195, 238);}");

    themeMenu->setStyleSheet("QMenu{background-color: rgb(104, 215, 243); border:none;border-radius: 10px;}"
                        "QMenu::item{border-radius: 10px;color:rgb(255, 255, 255);padding:3px 20px;margin:10px 10px;}"
                        "QMenu::item:hover{background-color:rgb(0, 195, 238);}"
                        "QMenu::item:selected{background-color:rgb(0, 195, 238);}");

    fileButton->setStyleSheet("QPushButton{background-color: rgb(0, 195, 238);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(104, 215, 243)}"
                        "QPushButton::pressed{background-color: rgb(104, 215, 243)}"
                        "QPushButton::menu-indicator{ image:none;}");

    buildButton->setStyleSheet("QPushButton{background-color: rgb(0, 195, 238);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(104, 215, 243)}"
                        "QPushButton::pressed{background-color: rgb(104, 215, 243)}"
                        "QPushButton::menu-indicator{ image:none;}");

    themeButton->setStyleSheet("QPushButton{background-color: rgb(0, 195, 238);color:rgb(255, 255, 255);}"
                        "QPushButton::hover{background-color: rgb(104, 215, 243)}"
                        "QPushButton::pressed{background-color: rgb(104, 215, 243)}"
                        "QPushButton::menu-indicator{ image:none;}");

    plainTextEdit->setStyleSheet("QPlainTextEdit{background-color:rgb(255, 255, 255);border:none; border-radius:20px;color:rgb(93, 93, 94);"
                        "selection-background-color:rgb(230, 245, 249);selection-color:rgb(0, 195, 238);"
                        "padding: 20px;}");

    plainTextEdit->verticalScrollBar()->setStyleSheet(
                    "QScrollBar:vertical{background-color:none;width:12px;border-radius:6px}"
                    "QScrollBar::handle:vertical{background:rgb(104, 215, 243);border-radius:6px;min-height:20;}"
                    "QScrollBar::handle:vertical:hover{background:rgb(0, 195, 238);border-radius:6px;min-height:20;}"
                    "QScrollBar::add-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::sub-line:vertical{height:0px;width:0px;}"
                    "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{width:0px;background:rgb(255, 255, 255);}"
                    );
    plainTextEdit->horizontalScrollBar()->setStyleSheet(
                    "QScrollBar:horizontal{background:none;height:12px;border-radius:6px}"
                    "QScrollBar::handle:horizontal{background:rgb(104, 215, 243);border-radius:6px;min-width:20;}"
                    "QScrollBar::handle:horizontal:hover{background:rgb(0, 195, 238);border-radius:6px;min-width:20;}"
                    "QScrollBar::add-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::sub-line:horizontal{height:0px;width:0px;}"
                    "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{height:0px;background:rgb(255, 255, 255);}"
                    );
}
