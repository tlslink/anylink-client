#ifndef ANYLINK_H
#define ANYLINK_H

#include <QWidget>
#include <QIcon>
#include <QJsonObject>
#include <QTimer>

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
    QTimer timer;

    QIcon iconConnected = QIcon(":/assets/connected.png");
    QIcon iconNotConnected = QIcon(":/assets/notconnected.png");
    QIcon iconConnecting = QIcon(":/assets/connecting.png");

    ProfileManager *profileManager;
    bool m_vpnConnected;
    bool activeDisconnect = false;
    DetailDialog *detailDialog = nullptr;
    QJsonObject currentProfile = {};

    void center();
    void loadStyleSheet(const QString &styleSheetFile);

    void createTrayActions();
    void createTrayIcon();
    void initConfig();
    void afterShowOneTime();

    void resetVPNStatus();

    void saveConfig();

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
