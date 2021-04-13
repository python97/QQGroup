#ifndef DIALOGLIST_H
#define DIALOGLIST_H

#include <QWidget>
#include <widget.h>
#include <QVector>
#include<QToolButton>
namespace Ui {
class dialogList;
}

class dialogList : public QWidget
{
    Q_OBJECT

public:
    explicit dialogList(QWidget *parent = nullptr);
    ~dialogList();



private:
    Ui::dialogList *ui;
    //写一个容器，方便后面维护按钮
    QVector<QToolButton *> vToolBtn;
    // 是否弹出界面的标识
    QVector<bool> isFlagOpen;



};

#endif // DIALOGLIST_H
