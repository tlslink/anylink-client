#ifndef ANYLINK_H
#define ANYLINK_H

#include <QWidget>
#include <QIcon>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class AnyLink;
}
QT_END_NAMESPACE

class JsonRpcWebSocketClient;
class ProfileManager;
class DetailDialog;
class QSystemTrayIcon;
class QMenu;
class QComboBox;

class AnyLink : public QWidget
{
    Q_OBJECT

public:
    enum { STATUS, CONFIG, CONNECT, DISCONNECT, RECONNECT, INTERFACE, ABORT, STAT };

    AnyLink(QWidget *parent = nullptr);
    ~AnyLink();

    JsonRpcWebSocketClient *rpc = nullptr;

signals:
    void vpnConnected();
    void vpnClosed();
    void callEnd();


    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::AnyLink *ui;
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
    QJsonObject currentProfile = {};

    void loadStyleSheet(const QString &styleSheetFile);

    void createTrayActions();
    void createTrayIcon();
    void initConfig();
    void afterShowOneTime();

    void resetVPNStatus();

private slots:
    void configVPN();
    void connectVPN(bool reconnect = false);
    void disconnectVPN();
    void getVPNStatus();

    //  QMetaObject::connectSlotsByName
    void on_buttonConnect_clicked();
    void on_buttonProfile_clicked();
    void on_buttonViewLog_clicked();
    void on_buttonDetails_clicked();
    void on_buttonSecurityTips_clicked();
};
#endif // ANYLINK_H
