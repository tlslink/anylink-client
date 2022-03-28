#include "detaildialog.h"
#include "ui_detaildialog.h"
#include <QHostAddress>

DetailDialog::DetailDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DetailDialog)
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
    const int size = includes.size();
    if(size) {
        for(int i = 0; i < size; i++) {
            QPair<QHostAddress, int> cidr = QHostAddress::parseSubnet(includes[i].toString());
            QTableWidgetItem *ip = new QTableWidgetItem(static_cast<QHostAddress>(cidr.first).toString());
            QTableWidgetItem *mask = new QTableWidgetItem(QString::number(cidr.second));

            ui->tableSecured->insertRow(i);
            ui->tableSecured->setItem(i, 0, ip);
            ui->tableSecured->setItem(i, 1, mask);
        }
    } else {
        QTableWidgetItem *ip = new QTableWidgetItem("0.0.0.0");
        QTableWidgetItem *mask = new QTableWidgetItem("0");

        ui->tableSecured->insertRow(0);
        ui->tableSecured->setItem(0, 0, ip);
        ui->tableSecured->setItem(0, 1, mask);
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
    ui->labelBytesRecived->clear();
}

void DetailDialog::setTunName(const QString &tunName)
{
    m_tunName = tunName;
}

QString DetailDialog::format(const uint &bytesTotal)
{
    QString unit;
    double bytes = bytesTotal;
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
    connect(&timer, &QTimer::timeout, [this]() {
#if defined(Q_OS_LINUX)
        tx_bytes_file.seek(0);
        rx_bytes_file.seek(0);
        QByteArray tx_bytes = tx_bytes_file.readAll().trimmed();
        QByteArray rx_bytes = rx_bytes_file.readAll().trimmed();
        ui->labelBytesSent->setText(format(tx_bytes.toUInt()));
        ui->labelBytesRecived->setText(format(rx_bytes.toUInt()));
#endif
    });

#if defined(Q_OS_LINUX)
    tx_bytes_file.setFileName(QString("/sys/class/net/%1/statistics/tx_bytes").arg(m_tunName));
    rx_bytes_file.setFileName(QString("/sys/class/net/%1/statistics/rx_bytes").arg(m_tunName));
    if(!tx_bytes_file.open(QIODevice::ReadOnly)) {
        return;
    }
    if(!rx_bytes_file.open(QIODevice::ReadOnly)) {
        return;
    }
#endif

    timer.start(1000);
}


void DetailDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    timer.stop();

#if defined(Q_OS_LINUX)
    rx_bytes_file.close();
    rx_bytes_file.close();
#endif
}
