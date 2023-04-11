#include "loading.h"
#include "qmath.h"
#include <QDebug>

Loading::Loading(QWidget *parent) : QWidget(parent), _interval(90)
{
    //设置背景透明
    // this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
    // Qt::WindowMinMaxButtonsHint);
    // this->setAttribute(Qt::WA_TranslucentBackground, true);

    setDotColor(QColor(49, 177, 190));
    setDotCount(12);
    connect(&timer, &QTimer::timeout, this, &Loading::refresh);
    setMaxDiameter(5);
    setMinDiameter(1);
}

//********************************************** 设置部分 *************************************
//设置点的个数
void Loading::setDotCount(int count)
{
    _count = count;
}

//设置点的颜色
void Loading::setDotColor(const QColor &color)
{
    _dotColor = color;
}

//开始动画
void Loading::start()
{
    m_started = true;
    timer.setInterval(_interval);
    timer.start();
}

void Loading::stop()
{
    if(m_started) {
        m_started = false;
        timer.stop();
    }
    repaint();
}

//设置最大直径
void Loading::setMaxDiameter(float max)
{
    _maxDiameter = max;
}

//设置最小直径
void Loading::setMinDiameter(float min)
{
    _minDiameter = min;
}
//********************************************** 绘制部分 *************************************
//刷新界面
void Loading::refresh()
{
    repaint();
}

void Loading::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    calculate();
}

void Loading::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if(m_started) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(_dotColor);
        painter.setBrush(_dotColor);

        //绘制点
        paintDot(painter);
    } else {
        QWidget::paintEvent(event);
    }
}

//计算绘制正方形区域
void Loading::calculate()
{
    _squareWidth = qMin(this->width(), this->height());
    float half = _squareWidth / 2;
    _centerDistance = half - _maxDiameter / 2 - 1;

    float gap = (_maxDiameter - _minDiameter) / (_count - 1) / 2;
    float angleGap = (float)360 / _count;

    locationList.clear();
    radiiList.clear();

    for(int i = 0; i < _count; i++) {
        radiiList << _maxDiameter / 2 - i *gap;
        float radian = qDegreesToRadians(-angleGap * i);
        locationList.append(Location(half + _centerDistance * qCos(radian), half - _centerDistance * qSin(radian)));
    }
}

//绘制圆点
void Loading::paintDot(QPainter &painter)
{
    for(int i = 0; i < _count; i++) {
        painter.setPen(_dotColor);
        //半径
        float radii = radiiList.at((_index + _count - i) % _count);

        //绘制圆点
        painter.drawEllipse(QPointF(locationList.at(i).x, locationList.at(i).y), radii, radii);
        //绘制正方形
        //painter.drawRect(locationList.at(i).x,locationList.at(i).y,radii,radii);
        //绘制文字
        //QFont font("Microsoft YaHei",radii*1.2,75);
        //painter.setFont(font);
        //painter.drawText(QPointF(locationList.at(i).x,locationList.at(i).y),u8"霞");
    }
    _index++;
}
