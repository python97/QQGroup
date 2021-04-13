#include "dialoglist.h"
#include "ui_dialoglist.h"


#include <QMessageBox>
dialogList::dialogList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dialogList)
{
    ui->setupUi(this);
    setWindowTitle("QQ2021");
    setWindowIcon(QPixmap(":/res/images/qq.png"));
    setFixedSize(250,700);
    //准备所有图标和名字
    QList<QString> nameList;
    nameList<<"琛哥"<<"五郎八卦棍"<<"十二路谭腿"<<"洪家铁线拳"<<"包租公"<<"包租婆"<<"火云邪神"<<"星爷"<<"哑女"<<"肥仔聪"<<"鳄鱼帮大佬";
    QList<QString> imgList;
    imgList<<"00.jpeg"<<"01.jpeg"<<"09.jpeg"<<"03.jpeg"<<"07.jpeg"<<"02.jpg"<<"04.jpeg"<<"10.jpeg"<<"05.jpeg"<<"11.jpeg"<<"12.jpeg";


    for(int i=0;i<=10;i++){
        //设置头像
        QToolButton *tb = new QToolButton;
        //设置文字
        tb->setText(nameList[i]);
        //设置头像
        tb->setIcon(QPixmap(QString(":/res/images/%1").arg(imgList[i])));
        //设置头像大小
        tb->setIconSize(QSize(60,60));
        //设置按钮风格  透明
        tb->setAutoRaise(true);
        //设置文字和图片一起显示
        tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        //加载到垂直布局中
        ui->verticalLayout_3->addWidget(tb);
        isFlagOpen.push_back(false);
        connect(tb,&QToolButton::clicked,[=](){
            //弹出对话框
            //构造聊天窗口时，告诉他这个窗口的名字 0表示顶层方式弹出
            // 注意 QWidget构造函数本身没有这两个参数
            if (!isFlagOpen[i]){
                Widget *widget = new Widget(0,tb->text());
                //设置窗口标题
                widget->setWindowTitle(tb->text());
                //设置窗口icon
                widget->setWindowIcon(tb->icon());
                //bug 同一个用户只能开启一个widget  通过一个标识
                widget->show();
                // 这里需要添加mutable关键字，才可以被修改
                connect(widget,&Widget::clossWidget,[=](){
                    isFlagOpen[i] = false;
                });
                isFlagOpen[i] = true;
            }
            else{
                QString str = QString("%1已被弹出").arg(tb->text());
                QMessageBox::warning(this,"警告",str);
                return;
            }

        });

        vToolBtn.push_back(tb);
    }
}

dialogList::~dialogList()
{
    delete ui;
}

