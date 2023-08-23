#include "anylink.h"
#include "ui_anylink.h"
#include "jsonrpcwebsocketclient.h"
#include "configmanager.h"
#include "profilemanager.h"
#include "textbrowser.h"
#include "detaildialog.h"
#include <QtWidgets>
#include <QCloseEvent>
#include <QJsonValue>
#include <QFile>

AnyLink::AnyLink(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AnyLink), m_vpnConnected(false)
{
    ui->setupUi(this);

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    loadStyleSheet(":/resource/style.qss");
#endif
#if defined(Q_OS_LINUX)
    setWindowIcon(QIcon(":/images/anylink64.png"));
#endif
//    qDebug() << screen()->devicePixelRatio() << geometry().width() << geometry().height() << QSysInfo::kernelType();
    // 需要联合使用 QSysInfo::kernelType() 和  QSysInfo::productType()
    if (screen()->devicePixelRatio() > 1.0 && QSysInfo::kernelType() == "linux") {
        setFixedSize(geometry().width(), geometry().height());
    } else {
        setFixedSize(480, 320);
    }

    setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
//    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint /*| Qt::WindowMinimizeButtonHint*/ | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);

    center();
    ui->lineEditOTP->setFocus();
    connect(ui->lineEditOTP, &QLineEdit::returnPressed, this, [this]() {
        if (!ui->lineEditOTP->text().isEmpty()) {
            if (rpc->isConnected()) {
                connectVPN();
            }
        }
    });

    profileManager = new ProfileManager(this);

    if(profileManager->loadProfile(Json)) {
        profileManager->updateModel();
        ui->comboBoxHost->setModel(profileManager->model);
        // update by vpnConnected
        QString lastProfile = configManager->config["lastProfile"].toString();
        if(!lastProfile.isEmpty() && profileManager->model->stringList().contains(lastProfile)) {
            ui->comboBoxHost->setCurrentText(lastProfile);
        } else {
            ui->comboBoxHost->setCurrentIndex(0);
        }
    }
    // exit
}

AnyLink::~AnyLink() { delete ui; }

void AnyLink::closeEvent(QCloseEvent *event)
{
    if(m_vpnConnected) {
        hide();
        event->accept();
        if(!trayIcon->isVisible()) {
            trayIcon->show();
        }
        // 避免直接关机不能保存状态
        configManager->saveConfig();
    } else {
        qApp->quit();
    }
}

void AnyLink::showEvent(QShowEvent *event)
{
    if(trayIcon == nullptr) {
        QTimer::singleShot(50, this, [this]() { afterShowOneTime(); });
    }
    event->accept();
}

void AnyLink::center()
{
    QRect screenGeometry = screen()->geometry();
    QRect windowGeometry = frameGeometry();
    QPoint centerPoint = screenGeometry.center() - windowGeometry.center();
    if (screen()->devicePixelRatio() > 1.0) {
        centerPoint -= QPoint(0,120);
    }
    // 将窗口移动到居中位置
    move(centerPoint);
}

void AnyLink::loadStyleSheet(const QString &styleSheetFile)

{

    QFile file(styleSheetFile);

    file.open(QFile::ReadOnly);

    if (file.isOpen())

    {

        QString styleSheet = this->styleSheet();

        styleSheet += QLatin1String(file.readAll());//读取样式表文件

        setStyleSheet(styleSheet);//把文件内容传参

        file.close();

    }

}

void AnyLink::createTrayActions()
{
    actionConnect = new QAction(tr("Connect Gateway"), this);
    // not lambda must have this
    connect(actionConnect, &QAction::triggered, this, &AnyLink::connectVPN);

    actionDisconnect = new QAction(tr("Disconnect Gateway"), this);
    connect(actionDisconnect, &QAction::triggered, this, &AnyLink::disconnectVPN);

    actionConfig = new QAction(tr("Show Panel"), this);
    connect(actionConfig, &QAction::triggered, this, &AnyLink::show);

    actionQuit = new QAction(tr("Quit"), this);
    connect(actionQuit, &QAction::triggered, qApp, &QApplication::quit, Qt::QueuedConnection);
}

void AnyLink::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(actionConnect);
    trayIconMenu->addAction(actionDisconnect);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(actionConfig);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(actionQuit);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(iconNotConnected);

    connect(trayIcon,
            &QSystemTrayIcon::activated,
            this,
            [this](QSystemTrayIcon::ActivationReason reason) {
                if (reason == QSystemTrayIcon::Trigger) {
                    show();
                }
            });
}

void AnyLink::initConfig()
{
    ui->checkBoxAutoLogin->setChecked(configManager->config["autoLogin"].toBool());
    ui->checkBoxMinimize->setChecked(configManager->config["minimize"].toBool());
    ui->checkBoxBlock->setChecked(configManager->config["block"].toBool());
    ui->checkBoxDebug->setChecked(configManager->config["debug"].toBool());
    ui->checkBoxLang->setChecked(configManager->config["local"].toBool());
    ui->checkBoxCiscoCompat->setChecked(configManager->config["cisco_compat"].toBool());

    connect(ui->checkBoxAutoLogin, &QCheckBox::toggled, this, [](bool checked) {
        configManager->config["autoLogin"] = checked;
    });
    connect(ui->checkBoxMinimize, &QCheckBox::toggled, this, [](bool checked) {
        configManager->config["minimize"] = checked;
    });
    connect(ui->checkBoxBlock, &QCheckBox::toggled, this, [this](bool checked) {
        configManager->config["block"] = checked;
        configVPN();
    });
    connect(ui->checkBoxDebug, &QCheckBox::toggled, this, [this](bool checked) {
        configManager->config["debug"] = checked;
        configVPN();
    });
    connect(ui->checkBoxLang, &QCheckBox::toggled, this, [](bool checked) {
        configManager->config["local"] = checked;
    });
    connect(ui->checkBoxCiscoCompat, &QCheckBox::toggled, this, [this](bool checked) {
        configManager->config["cisco_compat"] = checked;
        configVPN();
    });
}

void AnyLink::afterShowOneTime()
{
    createTrayActions();
    createTrayIcon();
    initConfig();
    profileManager->afterShowOneTime();
    detailDialog = new DetailDialog(this);

    ui->labelVersion->setText(appVersion);

    connect(this, &AnyLink::vpnConnected, this, [this]() {
        getVPNStatus();
        m_vpnConnected = true;
        activeDisconnect = false;
        trayIcon->setIcon(iconConnected);
        ui->buttonConnect->setText(tr("Disconnect"));
        ui->comboBoxHost->setEnabled(false);
        ui->lineEditOTP->setEnabled(false);
        ui->buttonProfile->setEnabled(false);
        ui->tabSetting->setEnabled(false);

        actionConnect->setEnabled(false);
        actionDisconnect->setEnabled(true);
        if(ui->checkBoxMinimize->isChecked()) {
            close();
            trayIcon->setToolTip(tr("Connected to: ") + currentProfile.value("host").toString());
        }
        configManager->config["lastProfile"] = ui->comboBoxHost->currentText();
    });

    connect(this, &AnyLink::vpnClosed, [this]() {
        m_vpnConnected = false;
        trayIcon->setIcon(iconNotConnected);
        trayIcon->setToolTip("");

        ui->buttonConnect->setText(tr("Connect"));
        ui->comboBoxHost->setEnabled(true);
        ui->lineEditOTP->setEnabled(true);
        ui->buttonProfile->setEnabled(true);
        ui->tabSetting->setEnabled(true);

        actionConnect->setEnabled(true);
        actionDisconnect->setEnabled(false);
        resetVPNStatus();
    });

    connect(qApp, &QApplication::aboutToQuit, this, [this]() {
        if(m_vpnConnected) {
            disconnectVPN();
        }        
    });

    rpc = new JsonRpcWebSocketClient(this);
    connect(rpc, &JsonRpcWebSocketClient::error, this, [this](const QString &error) {
        Q_UNUSED(error)
        ui->statusBar->setText(tr("Failed to connect to vpnagent, please reinstall the software!"));
        ui->buttonConnect->setEnabled(false);
        emit vpnClosed();
        if(isHidden()) {
            show();
        }
    });
    connect(rpc, &JsonRpcWebSocketClient::connected, this, [this]() {
        configVPN();
        if(configManager->config["autoLogin"].toBool()) {
            QString lastProfile = configManager->config["lastProfile"].toString();
            if(!lastProfile.isEmpty() && profileManager->model->stringList().contains(lastProfile)) {
                connectVPN();
            }
        }
    });
    // may be exited normally by other clients, do not automatically reconnect
    rpc->registerCallback(DISCONNECT, [this](const QJsonValue & result) {
        ui->progressBar->stop();
        ui->statusBar->setText(result.toString());
        emit vpnClosed();
        if(!activeDisconnect && isHidden()) {
            show();
        }
    });
    // unusual exited
    rpc->registerCallback(ABORT, [this](const QJsonValue & result) {
        ui->statusBar->setText(result.toString());
        emit vpnClosed();
        if (!activeDisconnect) {
            // 快速重连，不需要再次进行用户认证
            QTimer::singleShot(1500, this, [this]() { connectVPN(true); });
        }
    });
    rpc->connectToServer(QUrl("ws://127.0.0.1:6210/rpc"));
}

void AnyLink::resetVPNStatus()
{
    ui->labelChannelType->clear();
    ui->labelTlsCipherSuite->clear();
    ui->labelDtlsCipherSuite->clear();
    ui->labelDTLSPort->clear();
    ui->labelServerAddress->clear();
    ui->labelLocalAddress->clear();
    ui->labelVPNAddress->clear();
    ui->labelMTU->clear();
    ui->labelDNS->clear();

    ui->buttonDetails->setEnabled(false);
    detailDialog->clear();

    ui->lineEditOTP->clear();
}

/**
 * called by JsonRpcWebSocketClient::connected and every time setting changed
 */
void AnyLink::configVPN()
{
    if(rpc->isConnected()) {
        QJsonObject args {
            { "log_level", ui->checkBoxDebug->isChecked() ? "Debug" : "Info" },
            { "log_path", tempLocation},
            { "skip_verify", !ui->checkBoxBlock->isChecked() },
            {"cisco_compat", ui->checkBoxCiscoCompat->isChecked()},
            {"agent_name", agentName},
            {"agent_version", appVersion}
        };
        rpc->callAsync("config", CONFIG, args, [this](const QJsonValue & result) {
            ui->statusBar->setText(result.toString());
        });
    }
}

void AnyLink::connectVPN(bool reconnect)
{
    if(rpc->isConnected()) {
        // profile may be modified, and may not emit currentTextChanged signal
        // must not affected by QComboBox::currentTextChanged

        QString method = "connect";
        int id = CONNECT;
        if(reconnect) {
            method = "reconnect";
            id = RECONNECT;
        } else {
            const QString name = ui->comboBoxHost->currentText();
            if (name.isEmpty()) {
                return;
            }
            QJsonObject profile = profileManager->profiles[name].toObject();
            currentProfile = profile;
            const QString otp = ui->lineEditOTP->text();
            if(!otp.isEmpty()) {
                currentProfile["password"] = profile["password"].toString() + otp;
            }
        }
        ui->progressBar->start();
        trayIcon->setIcon(iconConnecting);

        rpc->callAsync(method, id, currentProfile, [this, reconnect](const QJsonValue &result) {
            ui->progressBar->stop();
            if(result.isObject()) {  // error object
                // dialog
                //                ui->statusBar->setText(result.toObject().value("message").toString());
                if (reconnect) {
                    // 当快速重连失败，再次尝试完全重新连接，用于服务端可能已经移除session的情况
                    QTimer::singleShot(1500, this, [this]() { connectVPN(); });
                } else {
                    if (isHidden()) {
                        show();
                    }
                    error(result.toObject().value("message").toString(), this);
                }
            } else {
                ui->statusBar->setText(result.toString());
                emit vpnConnected();
            }
        });
    }
}

void AnyLink::disconnectVPN()
{
    if(rpc->isConnected()) {
        ui->progressBar->start();
        // because on_buttonConnect_clicked, must check m_vpnConnected outside
        rpc->callAsync("disconnect", DISCONNECT);
        activeDisconnect = true;
    }
}

void AnyLink::getVPNStatus()
{
    // 不考虑 DTLS 中途关闭情形
    rpc->callAsync("status", STATUS, [this](const QJsonValue & result) {
        const QJsonObject &status = result.toObject();
        if(!status.contains("code")) {
            ui->labelChannelType->setText(status["DtlsConnected"].toBool() ? "DTLS" : "TLS");
            ui->labelTlsCipherSuite->setText(status["TLSCipherSuite"].toString());
            ui->labelDtlsCipherSuite->setText(status["DTLSCipherSuite"].toString());
            ui->labelDTLSPort->setText(status["DTLSPort"].toString());
            ui->labelServerAddress->setText(status["ServerAddress"].toString());
            ui->labelLocalAddress->setText(status["LocalAddress"].toString());
            ui->labelVPNAddress->setText(status["VPNAddress"].toString());
            ui->labelMTU->setText(QString::number(status["MTU"].toInt()));
            ui->labelDNS->setText(status["DNS"].toVariant().toStringList().join(","));

            ui->buttonDetails->setEnabled(true);
            detailDialog->setRoutes(status["SplitExclude"].toArray(), status["SplitInclude"].toArray());
        }
    });
}

void AnyLink::on_buttonConnect_clicked()
{
    if(rpc->isConnected()) {
        if(m_vpnConnected) {
            disconnectVPN();
        } else {
            connectVPN();
        }
    }
}

void AnyLink::on_buttonProfile_clicked()
{
    profileManager->exec();
}

void AnyLink::on_buttonViewLog_clicked()
{
    QString filePath = tempLocation + "/vpnagent.log";
    QFile loadFile(filePath);
    if(!loadFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error(tr("Couldn't open log file"), this);
        return;
    }
    TextBrowser textBrowser(tr("Log Viewer"),this);


    QString data = loadFile.readAll();
    textBrowser.setText(data);
    loadFile.close();

    // 创建文件系统监视器
    QFileSystemWatcher watcher;
    watcher.addPath(filePath);

    // 监视文件变化的信号槽连接
    QObject::connect(&watcher, &QFileSystemWatcher::fileChanged, [&]() {
        QFile updatedFile(filePath);
        if (updatedFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // 重新读取文件内容
            data = updatedFile.readAll();
            textBrowser.setText(data);
            updatedFile.close();
        }
    });

    textBrowser.exec();
}

void AnyLink::on_buttonDetails_clicked()
{
    detailDialog->exec();
}

void AnyLink::on_buttonSecurityTips_clicked()
{
    QString readme = "README.md";
    if (QLocale::system().name() == "zh_CN") {
        readme = "README_zh_CN.md";
    }
    QFile loadFile(":/resource/" + readme);
    if(!loadFile.open(QIODevice::ReadOnly)) {
        error(tr("Couldn't open README.md"), this);
        return;
    }
    QByteArray data = loadFile.readAll();
    TextBrowser textBrowser(tr("Security Tips"),this);
    textBrowser.setMarkdown(data);
    textBrowser.exec();
}
