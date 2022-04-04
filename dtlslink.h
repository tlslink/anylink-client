#ifndef DTLSLINK_H
#define DTLSLINK_H

#include <QWidget>
#include <QIcon>

QT_BEGIN_NAMESPACE
namespace Ui {
class DtlsLink;
}
QT_END_NAMESPACE

class JsonRpcWebSocketClient;
class ProfileManager;
class DetailDialog;
class QSystemTrayIcon;
class QMenu;
class QComboBox;
class QJsonObject;

class DtlsLink : public QWidget
{
    Q_OBJECT

public:
    enum { STATUS, CONFIG, CONNECT, DISCONNECT, RECONNECT, INTERFACE, ABORT, STAT };

    DtlsLink(QWidget *parent = nullptr);
    ~DtlsLink();

    JsonRpcWebSocketClient *rpc = nullptr;

signals:
    void vpnConnected();
    void vpnClosed();
    void callEnd();
private slots:
    //  QMetaObject::connectSlotsByName
    void on_buttonConnect_clicked();
    void on_buttonProfile_clicked();
    void on_buttonViewLog_clicked();
    void on_buttonDetails_clicked();

    void configVPN();
    void connectVPN(bool reconnect = false);
    void disconnectVPN();
    void getVPNStatus();

private:
    Ui::DtlsLink *ui;
    QSystemTrayIcon *trayIcon = nullptr;
    QMenu *trayIconMenu;
    QComboBox *iconComboBox;
    QAction *actionConnect, *actionDisconnect, *actionQuit, *actionConfig;

    QIcon iconConnected = QIcon(":/images/connected.png");
    QIcon iconNotConnected = QIcon(":/images/notconnected.png");
    QIcon iconConnecting = QIcon(":/images/connecting.png");

    ProfileManager *profileManager;
    bool m_vpnConnected;
    bool activeDisconnect = false;
    DetailDialog *detailDialog = nullptr;

    void createTrayActions();
    void createTrayIcon();
    void initConfig();
    void afterShowOneTime();

    void resetVPNStatus();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
};
#endif // DTLSLINK_H
