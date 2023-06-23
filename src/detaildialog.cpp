#include "detaildialog.h"
#include "ui_detaildialog.h"
#include "anylink.h"
#include "jsonrpcwebsocketclient.h"
#include <QHostAddress>
#include <QJsonObject>

DetailDialog::DetailDialog(AnyLink *parent) :
    QDialog(parent),
    ui(new Ui::DetailDialog), anylink(parent)
{
    ui->setupUi(this);
    ui->tableExcluded->setColumnCount(2);
    ui->tableSecured->setColumnCount(2);

    QStringList head = { tr("Destination"), tr("Prefix")};
    ui->tableExcluded->setHorizontalHeaderLabels(head);
    ui->tableSecured->setHorizontalHeaderLabels(head);

    setFixedSize(geometry().width(), geometry().height());
}

DetailDialog::~DetailDialog()
{
    delete ui;
}

void DetailDialog::setRoutes(const QJsonArray &excludes, const QJsonArray &includes)
{
//    for(auto it = excludes->constBegin(); it != excludes->constEnd(); it++) {}
    for(int i = 0; i < excludes.size(); i++) {
        QPair<QHostAddress, int> cidr = QHostAddress::parseSubnet(excludes[i].toString());
        QTableWidgetItem *ip = new QTableWidgetItem(static_cast<QHostAddress>(cidr.first).toString());
        QTableWidgetItem *mask = new QTableWidgetItem(QString::number(cidr.second));

        ui->tableExcluded->insertRow(i);
        ui->tableExcluded->setItem(i, 0, ip);
        ui->tableExcluded->setItem(i, 1, mask);
    }

    for(int i = 0; i < includes.size(); i++) {
        QPair<QHostAddress, int> cidr = QHostAddress::parseSubnet(includes[i].toString());
        QTableWidgetItem *ip = new QTableWidgetItem(static_cast<QHostAddress>(cidr.first).toString());
        QTableWidgetItem *mask = new QTableWidgetItem(QString::number(cidr.second));

        ui->tableSecured->insertRow(i);
        ui->tableSecured->setItem(i, 0, ip);
        ui->tableSecured->setItem(i, 1, mask);
    }

    ui->tableExcluded->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->tableSecured->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

    ui->tableExcluded->setCurrentCell(-1, -1);
}

void DetailDialog::clear()
{
    ui->tableExcluded->clearContents();
    ui->tableSecured->clearContents();

    ui->labelBytesSent->clear();
    ui->labelBytesReceived->clear();
}

QString DetailDialog::format(double bytes)
{
    QString unit;
    // use 1000 not 1024 same as ip -h -s link show dev tun0
    if(bytes < 1000) {
        unit = "B";
    } else if(bytes < 1000000) {  // 1024*1024
        bytes /= 1000;
        unit = "KB";
    } else if(bytes < 1000000000) {  // 1024*1024*1024
        bytes /= 1000000;
        unit = "MB";
    } else {
        bytes /= 1000000000;
        unit = "GB";
    }
    return QString("%1 %2").arg(QString::number(bytes, 'f', 2)).arg(unit);
}


void DetailDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    // 每隔 1 秒获取流量统计
    connect(&timer, &QTimer::timeout, this, [this]() {
        if(anylink->rpc->isConnected()) {
            anylink->rpc->callAsync("stat", AnyLink::STAT, [this](const QJsonValue & result) {
                const QJsonObject &stat = result.toObject();
                if(!stat.contains("code")) {
                    ui->labelBytesSent->setText(format(stat["bytesSent"].toDouble()));
                    ui->labelBytesReceived->setText(format(stat["bytesReceived"].toDouble()));
                }
            });
        }
    });

    timer.start(1000);
}


void DetailDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    timer.stop();
}
