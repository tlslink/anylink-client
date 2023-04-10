#ifndef LOADING_H
#define LOADING_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QMap>

struct Location {
public:
    explicit Location(float _x, float _y) {x = _x; y = _y;}
    float x;
    float y;
};

class Loading : public QWidget
{
    Q_OBJECT
public:
    explicit Loading(QWidget *parent = nullptr);
    //设置圆点个数
    void setDotCount(int);
    //设置点颜色
    void setDotColor(const QColor &);
    //开始
    void start();
    void stop();
    //设置圆点最大直径
    void setMaxDiameter(float);
    //设置圆点最小直径
    void setMinDiameter(float);

private:
    //刷新计数
    int _index = 0;
    //点的颜色
    QColor _dotColor;
    //点的个数
    int _count;
    //圆点最小直径
    float _minDiameter;
    //圆点最大直径
    float _maxDiameter;
    //绘制圆点
    void paintDot(QPainter &);
    //计数
    int _i = 0;
    //时间间隔 单位：毫秒(ms)
    int _interval;
    //定时器
    QTimer timer;
    //绘制区域边长
    float _squareWidth;
    //圆的直径
    float _centerDistance;
    //直径列表
    QList<float> radiiList;
    //圆点坐标列表
    QList<Location> locationList;

    bool m_started = false;

    //计算
    void calculate();

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private slots:
    void refresh();
};

#endif // LOADING_H
