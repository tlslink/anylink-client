#ifndef DETAILDIALOG_H
#define DETAILDIALOG_H

#include <QDialog>
#include <QJsonArray>
#include <QTimer>
#if defined(Q_OS_LINUX)
#include <QFile>
#endif

namespace Ui {
class DetailDialog;
}

class DetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DetailDialog(QWidget *parent = nullptr);
    ~DetailDialog();

    void setRoutes(const QJsonArray &excludes, const QJsonArray &includes);
    void clear();

    void setTunName(const QString &tunName);

private:
    Ui::DetailDialog *ui;
    QString m_tunName;
    QTimer timer;

#if defined(Q_OS_LINUX)
    QFile tx_bytes_file;
    QFile rx_bytes_file;
#endif

    QString format(const uint &bytesTotal);
    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
};

#endif // DETAILDIALOG_H
