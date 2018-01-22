#include "../../include/rosgui/qt/t3_af_face.hpp"
#include "ui_t3_af_face.h"



//界面构造函数
T3_AF_face::T3_AF_face(T3Dialog *mainWindow, QWidget *parent) :
    T3Dialog(parent),
    _mainWindow(mainWindow),
    ui(new Ui::T3_AF_face)
{
    _father = new T3Dialog;
    //界面布局初始化
    ui->setupUi(this);
    this->setGeometry(0, 0, _father->_width_, _father->_height_);
//    this->move(0, 0);
//    this->resize(_father->_width_, _father->_height_);
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint);
    this->showFullScreen();
    ui->_dateLabel_->setText("");
    ui->_dateLabel_->setStyleSheet("color:white");
    ui->_timeLabel_->setText("");
    ui->_timeLabel_->setStyleSheet("color:white");
    ui->_exitPushBtn_->setText("");
    ui->_exitPushBtn_->setFocusPolicy(Qt::NoFocus);
    ui->_exitPushBtn_->setStyleSheet("border-image:url(:/Pictures/face_back.png)");
    ui->_faceGifLabel_->setText("");
    ui->_faceGifLabel_->setStyleSheet("border-image:url(:/Pictures/face_gif.png)");
    ui->_textLabel_->setText("");
    ui->_textLabel_->setStyleSheet("border-image:url(:/Pictures/face_text.png)");
    ui->_squareLabel_->setText("");
    ui->_squareLabel_->setStyleSheet("border-image:url(:/Pictures/face_square.png)");
    ui->_videoLabel_->setText("");
    ui->_videoLabel_->setStyleSheet("color:white");
    ui->_logBackgroundLabel_->setText("");
    ui->_logBackgroundLabel_->setStyleSheet("border-image:url(:/Pictures/face_logBackground.png)");
    ui->_morePushBtn_->setFocusPolicy(Qt::NoFocus);
    ui->_enterFaceLogPushBtn_->setFocusPolicy(Qt::NoFocus);
    ui->_vocalPushBtn_->setFocusPolicy(Qt::NoFocus);
    //size
    ui->_dateLabel_->setGeometry(this->width()*0.0688,
                                 this->height()*0.0667,
                                 this->width()*0.1625,
                                 this->height()*0.0667);
    ui->_enterFaceLogPushBtn_->setGeometry(this->width()*0.7775,
                                           this->height()*0.8444,
                                           this->width()*0.1300,
                                           this->height()*0.0578);
    ui->_exitPushBtn_->setGeometry(this->width()*0.9313,
                                   this->height()*0.0178,
                                   this->width()*0.0375,
                                   this->height()*0.0600);
    ui->_faceGifLabel_->setGeometry(this->width()*0.0375,
                                    this->height()*0.3111,
                                    this->width()*0.2000,
                                    this->height()*0.4222);
    ui->_logBackgroundLabel_->setGeometry(this->width()*0.7375,
                                          this->height()*0.2333,
                                          this->width()*0.2188,
                                          this->height()*0.6000);
    ui->_morePushBtn_->setGeometry(this->width()*0.8813,
                                   this->height()*0.2333,
                                   this->width()*0.0750,
                                   this->height()*0.0889);
    ui->_squareLabel_->setGeometry(this->width()*0.2875,
                                   this->height()*0.2333,
                                   this->width()*0.6688,
                                   this->height()*0.6000);
    ui->_textLabel_->setGeometry(this->width()*0.0375,
                                 this->height()*0.8000,
                                 this->width()*0.2000,
                                 this->height()*0.0889);
    ui->_timeLabel_->setGeometry(this->width()*0.0688,
                                 this->height()*0.1111,
                                 this->width()*0.1625,
                                 this->height()*0.1778);
    ui->_videoLabel_->setGeometry(this->width()*0.2888,
                                  this->height()*0.2356,
                                  this->width()*0.4463,
                                  this->height()*0.5933);
    ui->_vocalPushBtn_->setGeometry(this->width()*0.7875,
                                    this->height()*0.1556,
                                    this->width()*0.1000,
                                    this->height()*0.0578);

    //font
    QFont dateLabelFont_;
    dateLabelFont_.setPointSize(ui->_dateLabel_->height() * kLabelFontScal*0.55);
    QFont timeLabelFont_;
    timeLabelFont_.setPointSize(ui->_timeLabel_->height() * kLabelFontScal*0.8);
    QFont vocalPushBtnFont_;
    vocalPushBtnFont_.setPointSize(ui->_vocalPushBtn_->height() * kBtnFontScal);
    QFont morePushBtnFont_;
    morePushBtnFont_.setPointSize(ui->_morePushBtn_->height() * kBtnFontScal * 0.7);
    QFont enterFaceLogPushBtnFont_;
    enterFaceLogPushBtnFont_.setPointSize(ui->_enterFaceLogPushBtn_->height() * kBtnFontScal);

    ui->_dateLabel_->setFont(dateLabelFont_);
    ui->_timeLabel_->setFont(timeLabelFont_);
    ui->_vocalPushBtn_->setFont(vocalPushBtnFont_);
    ui->_morePushBtn_->setFont(morePushBtnFont_);
    ui->_enterFaceLogPushBtn_->setFont(enterFaceLogPushBtnFont_);
    //界面浮现动画
//    QPropertyAnimation *animation_ = new QPropertyAnimation(this, "windowOpacity");
//    animation_->setDuration(150);
//    animation_->setStartValue(0);
//    animation_->setEndValue(1);
//    animation_->start();
    //定时器
    QTimer *timer_ = new QTimer(this);
    timer_->start(100);
    //链接ui部件与功能
    connect(timer_, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    connect(ui->_exitPushBtn_, &QPushButton::clicked, this, &T3_AF_face::exitToMainWindow);
    connect(ui->_enterFaceLogPushBtn_, &QPushButton::clicked, this, &T3_AF_face::enterFaceLog);
    connect(ui->_morePushBtn_, &QPushButton::clicked, this, &T3_AF_face::enterFaceHistory);
    connect(ui->_vocalPushBtn_, &QPushButton::clicked, this, &T3_AF_face::enterVocalText);

    //视频展示
     _netWork = T3_Face_Network::getT3FaceNetwork();
     _decoder = _netWork->_decoder_;

     if(_netWork->_isNetworkConnected_)
     {
       _netWork->getVideo();
     }else
     {
            //T3_AF_warning *warning = new T3_AF_warning(this,"网络未连接");
            //warning->show();
     }

     _frameLineData = _netWork->_frameLineData_;
     connect(_decoder,&Decoder::newFrame,this,&T3_AF_face::printVideo);
     //数据库连接
     _database = QSqlDatabase::addDatabase(kDatabaseEngine);
     _database.setDatabaseName(kDatabaseName);
     _database.setHostName(kServerURL);
     _database.setUserName(kDatabaseUserName);
     _database.setPassword(kDatabasePassword);
     _database.open();
     //记录日志展示

     QStringList strList ;
     strList.append("1");
     strList.append("1");
     strList.append("1");
     strList.append("1");
     strList.append("1");
     strList.append("1");
     strList.append("1");
     strList.append("1");
     strList.append("1");
     strList.append("1");
     _stringListModel = new QStringListModel(strList);
     ui->_logListView_->setModel(_stringListModel);

    //日志
    T3LOG("5+ 人脸界面构造");
}

//显示时间
void T3_AF_face::timeUpdate()
{
    QDateTime dateTime_ = QDateTime::currentDateTime();
    QString dataStr_ = dateTime_.toString("yyyy-MM-dd dddd");
    QString timeStr_ = dateTime_.toString("hh:mm");
    ui->_dateLabel_->setText(dataStr_);
    ui->_timeLabel_->setText(timeStr_);
}

//界面背景绘制
void T3_AF_face::paintEvent(QPaintEvent *)
{
    QPainter paint_(this);
    paint_.drawPixmap(0, 0, this->width(), this->height(), QPixmap(":/Pictures/face_background.png"));
}
//退出
void T3_AF_face::exitToMainWindow()
{
    _mainWindow->show();
    if(_netWork->_isNetworkConnected_)
    {
      _netWork->closeVideo();
    }
    for(int idx = 0; idx < kDelay; idx++){}

    this->close();
    delete this;
}

//enterFaceLog
void T3_AF_face::enterFaceLog()
{
    T3_AF_faceLog *faceLog_ = new T3_AF_faceLog(this);
    faceLog_->show();

    for(int idx = 0; idx < kDelay; idx++){}

    this->hide();
}

//enterFaceHistory
void T3_AF_face::enterFaceHistory()
{
    T3_AF_faceHistory *faceHistory_ = new T3_AF_faceHistory;
    faceHistory_->show();
}

void T3_AF_face::enterVocalText()
{
    T3_AF_vocalText *vocalText_ = new T3_AF_vocalText;
    vocalText_->show();
}

void T3_AF_face::closeEvent(QCloseEvent *event)
{
    event->ignore();
}

void T3_AF_face::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
        break;
    default:
        QDialog::keyPressEvent(event);
    }
}
//绘制视频
void T3_AF_face::printVideo(QImage faceImage)
{
  //faceImage = faceImage.mirrored(true,false);
  QPainter paint(&faceImage);
  QPen pen(Qt::yellow,2);
  paint.setPen(pen);
  paint.setFont(QFont(QString::fromLocal8Bit("宋体"),20,-1,false));
  //qDebug() << _frameLineData->personNum;
 for(int i = 0; i<_frameLineData->personNum; i++)
 {

     paint.drawLine(QPoint(_frameLineData->rightList[i],_frameLineData->topList[i]),QPoint(_frameLineData->leftList[i],_frameLineData->topList[i]));
     paint.drawLine(QPoint(_frameLineData->rightList[i],_frameLineData->bottomList[i]),QPoint(_frameLineData->leftList[i],_frameLineData->bottomList[i]));
     paint.drawLine(QPoint(_frameLineData->rightList[i],_frameLineData->topList[i]),QPoint(_frameLineData->rightList[i],_frameLineData->bottomList[i]));
     paint.drawLine(QPoint(_frameLineData->leftList[i],_frameLineData->topList[i]),QPoint(_frameLineData->leftList[i],_frameLineData->bottomList[i]));
     if(-1 != _frameLineData->idList[i])
     {
         QSqlQuery query_(_database);
         query_.prepare("select name from T3Face where id = ?");
         query_.bindValue(0, _frameLineData->idList[i]);

         query_.exec();
         query_.next();
         QString name_ = query_.value(0).toString();
         paint.drawText(QPointF(_frameLineData->dot2List[i]-1,_frameLineData->dot1List[i]),name_);
     }

 }

  paint.end();
  ui->_videoLabel_->setPixmap(QPixmap::fromImage(faceImage));
}

//界面析构函数
T3_AF_face::~T3_AF_face()
{
    delete ui;
    //日志
    T3LOG("5- 人脸界面析构");
}
