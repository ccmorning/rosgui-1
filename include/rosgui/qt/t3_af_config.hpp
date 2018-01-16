#ifndef T3_AF_CONFIG_H
#define T3_AF_CONFIG_H

//常用库
#include <QPropertyAnimation>
#include <QPainter>
#include <QTimer>
#include <QDateTime>
#include <QMovie>
#include "t3_af_getweather.hpp"

//T3LOG
#include <QDebug>
#include <string.h>
#include <stdio.h>

#define T3LOG(format, ...) qDebug(format , ##__VA_ARGS__);
#define kMapScal 9.0
#define kPi 3.141592653

#endif // T3_AF_CONFIG_H
