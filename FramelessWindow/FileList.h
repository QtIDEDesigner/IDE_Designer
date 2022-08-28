#ifndef FILELIST_H
#define FILELIST_H

// 文件列表部件类

#include <QListWidget>
#include <QScrollBar>
#include <QFont>
#include <QPainter>
// class FileList : public QListWidget
class FileList : public QListWidget
{
    Q_OBJECT
public:
    explicit FileList(QListWidget *parent = 0);
    QString currentFile;//当前文件

private:
    QString shownName;//所展示文件名
    QString curPlainText;//当前编辑区文本
    bool curIsModified;//编辑区是否修改过
    bool MaybeSave();//可能需要保存文件
    void loadFile(const QString &filename);//加载文件
    void setCurrentFile(const QString &filename);//重置当前文件
//    void paintEvent(QPaintEvent *event);
signals:
    LoadData(QString Data);//从文件中获取的内容
    ClearText();//清屏
    GetText();//保存文件时向编辑区获取文本
    SetTitle(QString name);
public slots:
    bool Save();
    void NewFile();
    void OpenFile();
    bool SaveFile(const QString &filename);
    bool SaveAs();
    void Close();

    void SetCurrentFilemodified();//重置编辑区修改
    void GetPlainText(QString Text);//从编辑区获取文本
};

#endif // FILELIST_H
