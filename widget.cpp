#include "widget.h"
#include "ui_widget.h"
#include <QDataStream>
#include <QDateTime>
#include <QColorDialog>
#include <QFileDialog>
Widget::Widget(QWidget *parent,QString name) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->uName = name;
    port = 23333;
    //建立套接字对象
    udpSocket = new QUdpSocket(this);
    //绑定端口号
    udpSocket->bind(port,QUdpSocket::ShareAddress |QUdpSocket::ReuseAddressHint);
    //监听信号
    connect(udpSocket,&QUdpSocket::readyRead,this,&Widget::receiveUdp);

    connect(ui->sendbtn,&QPushButton::clicked,this,[=](){
            sendMsg(Msg);
        });
    // 开启一个聊天界面就发送xxx上线的消息
    sendMsg(UsrEnter);

    connect(ui->exitbtn,&QPushButton::clicked,[=](){
        this->close();
    });

    // ------------------------辅助功能-----------------
    //字体
    connect(ui->fontcbx,&QFontComboBox::currentFontChanged,this,[=](const QFont &f){
        ui->textEdit->setCurrentFont(f);
        ui->textEdit->setFocus();
    });
    //字号
    // 有重载，利用函数指针
    void (QComboBox:: *fontsize)(const QString &text) = &QComboBox::currentIndexChanged;
    connect(ui->fontsizecbx,fontsize,[=](const QString &text){
        ui->textEdit->setFontPointSize(text.toDouble());
        ui->textEdit->setFocus();
    });
    //加粗
    connect(ui->boldbtn,&QToolButton::clicked,[=](bool checked){
        if(checked){
            ui->textEdit->setFontWeight(QFont::Bold);
        }
        else{
            ui->textEdit->setFontWeight(QFont::Normal);
        }
        ui->textEdit->setFocus();
    });
    //倾斜
    connect(ui->italicbtn,&QToolButton::clicked,[=](bool checked){
           ui->textEdit->setFontItalic(checked);
           ui->textEdit->setFocus();
    });
    //下划线
    connect(ui->underlinebtn,&QToolButton::clicked,[=](bool checked){
           ui->textEdit->setFontUnderline(checked);
           ui->textEdit->setFocus();
    });
    //颜色
    connect(ui->colorbtn,&QToolButton::clicked,[=](){
        color = QColorDialog::getColor(color,this,"选择字体颜色");
        if(color.isValid()){
            ui->textEdit->setTextColor(color);
            ui->textEdit->setFocus();
        }
    });
    //保存聊天记录
    connect(ui->savebtn,&QToolButton::clicked,[=](){
        if(!ui->textEdit->document()->isEmpty()){
            QString filepath = QFileDialog::getSaveFileName(this,"保存聊天记录","(*.txt");
            if(!filepath.isEmpty()){
                QFile file(filepath);
                file.open(QIODevice::WriteOnly|QFile::Text);
                QTextStream stream(&file);
                stream<<ui->textBrowser->toPlainText();
                file.close();
            }
        }
    });
    // 清空聊天记录
    connect(ui->clearbtn,&QToolButton::clicked,[=](){
        ui->textBrowser->clear();
    });
}

Widget::~Widget()
{
    delete ui;
}
void Widget::closeEvent(QCloseEvent *e){
    emit this->clossWidget();
    sendMsg(UsrLeft);
    //断开套接字
    udpSocket->close();
    udpSocket->destroyed();
    QWidget::closeEvent(e);
}

void Widget::sendMsg(MsgType type){
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    //数据流一共放三段
    out<<type<<getUsername();
    switch (type) {
    case Msg:
        if(ui->textEdit->toPlainText() == ""){
            return;
        }
        out<<getMessage();

        break;
    case UsrEnter:
        out<<getUsername();
        break;
    case UsrLeft:
        out<<getUsername();
        break;
    default:
        break;
    }
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast,port);
}
QString Widget::getUsername(){
    return this->uName;
}
QString Widget::getMessage(){
    QString str = ui->textEdit->toHtml();
    ui->textEdit->clear();
    ui->textEdit->setFocus();
    return str;
}
void Widget::receiveUdp(){

    QByteArray datagram;
    datagram.resize(udpSocket->pendingDatagramSize());  //设置大小
    udpSocket->readDatagram(datagram.data(),datagram.size()); //读取数据流
    QDataStream in (&datagram,QIODevice::ReadOnly);
    int msgType;
    in >> msgType; //用户类型获取

    QString usrName,msg;  //用户名、信息
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    switch (msgType){
    case Msg:
        in >> usrName  >>msg;
        ui->textBrowser->setTextColor(Qt::blue);
        ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
        ui->textBrowser->append("[ " + usrName + " ]" + time);
        ui->textBrowser->append(msg);
        break;
    case UsrEnter:
        in >> usrName ;
        userEnter(usrName);
        break;
    case UsrLeft:
        in >> usrName;
        userLeft(usrName,time);
        break;
    default:
        break;
    }
}
// 处理用户进入
void Widget::userEnter(QString username){
    // 更新xxx进入、更新在线人数、更新右侧在线用户列表
    //判断这个用户是否不在右侧列表中
    if(ui->tableWidget->findItems(username,Qt::MatchExactly).isEmpty()){
        QTableWidgetItem *qtw = new QTableWidgetItem(username);
        // 插入行
        ui->tableWidget->insertRow(0);
        ui->tableWidget->setItem(0,0,qtw);
        // 追加聊天记录
        ui->textBrowser->setTextColor(Qt::gray);
        ui->textBrowser->append(QString("%1上线了").arg(username));
         //在线用户人数更新
        ui->numberlbl->setText(QString("在线用户%1人").arg(ui->tableWidget->rowCount()));
        //把自己信息放进别人聊天界面去
        sendMsg(UsrEnter);
    }
}
void Widget::userLeft(QString username,QString time){
    // 获取这个用户所在行数
    if(!ui->tableWidget->findItems(username,Qt::MatchExactly).isEmpty()){
    int rows = ui->tableWidget->findItems(username,Qt::MatchExactly).first()->row();
    ui->tableWidget->removeRow(rows);
    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->append(QString("%1于%2离开了").arg(username).arg(time));
    //在线用户人数更新
    ui->numberlbl->setText(QString("在线用户%1人").arg(ui->tableWidget->rowCount()));
    sendMsg(UsrLeft);}
}

