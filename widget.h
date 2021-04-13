#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUdpSocket>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    //分别代表 聊天信息、新用户加入、用户退出
    enum MsgType {Msg,UsrEnter,UsrLeft};

public:
    explicit Widget(QWidget *parent = 0,QString name = "");
    ~Widget();
    //重写窗口关闭事件
    void closeEvent(QCloseEvent *e);
    // 发送广播的udp消息
    void sendMsg(MsgType type);

    // 处理新用户进入
    void userEnter(QString username);
    //处理用户离开
    void userLeft(QString username,QString time);
    QString getUsername();
    QString getMessage();
signals:
    //自定义关闭窗口信号
    void clossWidget();
private:
    Ui::Widget *ui;
    QUdpSocket *udpSocket; //UDP套接字
    qint16 port; //端口
    QString uName; //用户名
    QColor color; //字体颜色
private slots:
    //接收UDP消息
    void receiveUdp();
};

#endif // WIDGET_H
