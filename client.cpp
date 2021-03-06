#include "client.h"
#include "ui_client.h"

client::client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::client),player1(10,5),player2(17,10)
{
    ui->setupUi(this);
}

//开始按钮触发
void client::on_startbtn_clicked()
{
    //利用填入的ip连接
    ip=ui->serverip->text();
    mSocket=new QTcpSocket(this);

    //当链接上执行conn()
    QObject::connect(mSocket,&QTcpSocket::connected,this,&client::conn);
    //当发来数据就进行解读
    QObject::connect(mSocket,&QIODevice::readyRead,this,&client::ReadData);
    //连接225端口
    mSocket->connectToHost(ip,225);

    //设定连接时间
    if(mSocket->waitForConnected(1000))
        qDebug()<<"连接成功";
    else qDebug()<<"连接失败";

    //设定速度
    speed=1000;
    Timer= new QTimer(this);

    //初始化好果子坏果子
    goodfood=QPoint(2,4);
    badfood=QPoint(4,5);

    //设定计时器到时间后的事件
    QObject::connect(Timer,SIGNAL(timeout()),this,SLOT(timeout()));
    score1=0;
    score2=0;

    //设定积分榜
    ui->score->setText("1号："+QString::number(score1)+"分 2号："+QString::number(score2)+"分");
}

client::~client()
{
    delete ui;
}

//开始连接
void client::conn()
{
    //发送开始信号
    send(QString("00"));
}

//读数据
void client::ReadData()
{
    QByteArray buffer;//缓存区
    buffer=mSocket->readAll();
    mSocket->flush();//刷新

    //本工程里均采用Utf8编码
    QString str=QString(buffer).toUtf8();
    //解码
    Dcode(str);
}

//发送数据
void client::send(QString str)
{
    //以Utf8写入
    mSocket->write(str.toUtf8());
    mSocket->flush();
}

//解码
//编码规则:
//前两位为信号类型，00为开始信号，01为键位信号，02为分数信号，03为好果子，04为坏果子，05为结束
//之后的位数为信息位
//00:后面不接任何信息位
//01:之后接1234代表上下左右
//02:后面接两位分数位
//03:后面接四位代表坐标，x,y各两位
//04:后面接四位代表坐标，x,y各两位
//05:后面不接
void client::Dcode(QString str)
{
    qDebug()<<str;
    QString type=str.mid(0,2);
    //00信号触发计时器
    if(type=="00") Timer->start(speed);

    if(type=="01")
    {
        int tmp=str.mid(2,1).toInt();
        switch(tmp)
        {
        case 1: player1.move(0,-1);  break;
        case 2: player1.move(0,1);   break;
        case 3: player1.move(-1,0);  break;
        case 4: player1.move(1,0);   break;
        }
    }

    if(type=="02")
    {
        int tmp=str.mid(2,str.length()-2).toInt();
        score1=tmp;
    }

    if(type=="03")
    {
        int x=str.mid(2,2).toInt();
        int y=str.mid(4,2).toInt();
        goodfood.setX(x);
        goodfood.setY(y);
    }

    if(type=="04")
    {
        int x=str.mid(2,2).toInt();
        int y=str.mid(4,2).toInt();
        badfood.setX(x);
        badfood.setY(y);
    }

    if(type=="05")
    {
        QMessageBox::about(this, "游戏结束", "游戏结束");
        exit(0);
    }
}

//绘图模块
void client::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    //绘制地图
    QPainter paint(this);
    for(int i = 0; i<15*20;i+=15)
    {
        for(int j = 0; j<15*25;j+=15)
        {
            paint.drawRect(i,j,15,15);  //绘制方格
        }
    }

    //绘制食物
    //paint.drawImage(food[0]*20,food[1]*20,QImage(":/images/red").scaled(QSize(20,20)));

    //绘制蛇
    for(int i = 0; i<player1.getlength(); i++)
    {
        paint.fillRect(QRect(player1.list[i].x()*15,player1.list[i].y()*15,15,15),Qt::red);
        qDebug()<<player1.list[i].x()<<","<<player1.list[i].y()<<endl;

    }

    for(int i = 0; i<player2.getlength(); i++)
    {
        paint.fillRect(QRect(player2.list[i].x()*15,player2.list[i].y()*15,15,15),Qt::blue);
        qDebug()<<player2.list[i].x()<<","<<player2.list[i].y()<<endl;

    }

    //画好果子
    paint.fillRect(QRect(goodfood.x()*15,goodfood.y()*15,15,15),Qt::yellow);
    //画坏果子
    paint.fillRect(QRect(badfood.x()*15,badfood.y()*15,15,15),Qt::black);
}

//键盘监听
void client::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Up:    player2.move(0,-1); send("011");  break;
        case Qt::Key_W:     player2.move(0,-1); send("011"); break;
        case Qt::Key_Down:  player2.move(0,1);  send("012"); break;
        case Qt::Key_S:     player2.move(0,1);  send("012");  break;
        case Qt::Key_Left:  player2.move(-1,0); send("013"); break;
        case Qt::Key_A:     player2.move(-1,0); send("013"); break;
        case Qt::Key_Right: player2.move(1,0);  send("014"); break;
        case Qt::Key_D:     player2.move(1,0);  send("014"); break;
    }
}


//计时器触发
void client::timeout()
{
    //两蛇移动
    player1.moveon();
    player2.moveon();

    //1号碰到了墙
    if(player1.getHead().x()<0||player1.getHead().x()>=20||player1.getHead().y()<0||player1.getHead().y()>=25)
    {
        QMessageBox::about(this, "1号碰到了墙", "游戏结束");
        send("05");
        exit(0);
    }

    //2号碰到了墙
    if(player2.getHead().x()<0||player2.getHead().x()>=20||player2.getHead().y()<0||player2.getHead().y()>=25)
    {
        QMessageBox::about(this, "2号碰到了墙", "游戏结束");
        send("05");
        exit(0);
    }

    //吃到了自己
    if(player1.eatself()||player2.eatself())
    {
        QMessageBox::about(this, "吃到了自己", "游戏结束");
        send("05");
        exit(0);
    }

    //两蛇相撞
    if(crash())
    {
        QMessageBox::about(this, "相撞", "游戏结束");
        send("05");
        exit(0);
    }

    //1吃到好果子
    if(player1.getHead()==goodfood)
    {
        player1.eatplus();
        updategood();
        score1++;
    }

    //2吃到好果子
    if(player2.getHead()==goodfood)
    {
        player2.eatplus();
        updategood();
        score2++;
    }

    //1吃到坏果子
    if(player1.getHead()==badfood)
    {
        player1.eatdev();
        updatebad();
        score1--;
    }

    //2吃到坏果子
    if(player2.getHead()==badfood)
    {
        player2.eatdev();
        updatebad();
        score2--;
    }

    //发送分数
    if(score2>10)
    {
        send("02"+QString::number(score2));
    }
    else
    {
        send("020"+QString::number(score2));
    }

    //显示分数
    ui->score->setText("1号："+QString::number(score1)+"分 2号："+QString::number(score2)+"分");
    //重绘界面
    this->update();
    repaint();

    //计时器重新开始计时
    Timer->start(speed);
    //qDebug()<<"yes"<<endl;
}

//更新好果子
void client::updategood()
{
    //找出一个不与蛇重合，坏果子重合的点
    while(true)
    {
        goodfood=QPoint(rand()%20,rand()%25);
        bool flag=true;
        for(int i=0;i<player1.getlength();i++)
            if(player1.list[i].x()==goodfood.x()&&player1.list[i].y()==goodfood.y())
            {
                flag=false;
                break;
            }
        for(int i=0;i<player2.getlength();i++)
            if(player2.list[i].x()==goodfood.x()&&player2.list[i].y()==goodfood.y())
            {
                flag=false;
                break;
            }
        if(goodfood==badfood) flag=false;
        if(flag) break;
    }
}

//更新坏果子
void client::updatebad()
{
    //找出一个不与蛇重合，好果子重合的点
    while(true)
    {
        badfood=QPoint(rand()%20,rand()%25);
        bool flag=true;
        for(int i=0;i<player1.getlength();i++)
            if(player1.list[i].x()==badfood.x()&&player1.list[i].y()==badfood.y())
            {
                flag=false;
                break;
            }
        for(int i=0;i<player2.getlength();i++)
            if(player2.list[i].x()==badfood.x()&&player2.list[i].y()==badfood.y())
            {
                flag=false;
                break;
            }
        if(goodfood==badfood) flag=false;
        if(flag) break;
    }
}

//碰撞判断
bool client::crash()
{
    //两只蛇互相判断蛇头是否重叠
    for(int i=0;i<player1.getlength();i++)
        if(player1.list[i]==player2.getHead()) return true;

    for(int i=0;i<player2.getlength();i++)
        if(player2.list[i]==player1.getHead()) return true;

    return false;
}
