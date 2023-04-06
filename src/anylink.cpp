#include "anylink.h"
#include "ui_anylink.h"
#include "jsonrpcwebsocketclient.h"
#include "configmanager.h"
#include "profilemanager.h"
#include "logviewer.h"
#include "detaildialog.h"
#include <QtWidgets>
#include <QCloseEvent>
#include <QJsonValue>

AnyLink::AnyLink(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AnyLink), m_vpnConnected(false)
{
    ui->setupUi(this);
#if defined(Q_OS_LINUX)
    setWindowIcon(QIcon(":/images/anylink64.png"));
#endif
    // avoid some people prefer to use minimize
    setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
//    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint /*| Qt::WindowMinimizeButtonHint*/ | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);

    setGeometry(QStyle::alignedRect(
                    Qt::LeftToRight, Qt::AlignCenter, size(), screen()->availableGeometry()
                ));
    setFixedSize(geometry().width(), geometry().height());
    ui->lineEditOTP->setFocus();
    ui->labelVersion->setText(uiVersion);

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
        int x = configManager->config["x"].toInt();
        if(x > 0) {
            move(x, configManager->config["y"].toInt());
        }
    }
    // exit
}

AnyLink::~AnyLink() { delete ui; }

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
    QFile loadFile(tempLocation + "/vpnagent.log");
    if(!loadFile.open(QIODevice::ReadOnly)) {
        error(tr("Couldn't open log file"), this);
        return;
    }
    QByteArray data = loadFile.readAll();
    LogViewer logViewer(this);
    logViewer.setLog(data);
    logViewer.exec();
}

void AnyLink::on_buttonDetails_clicked()
{
    detailDialog->exec();
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
            {"cisco_compat", true}
        };
        rpc->callAsync("config", CONFIG, args, [this](const QJsonValue & result) {
            ui->statusBar->setText(result.toString());
        });
    }
}

void AnyLink::connectVPN(bool reconnect)
{
    if(rpc->isConnected()) {
        ui->progressBar->start();
        trayIcon->setIcon(iconConnecting);
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
            currentProfile = profileManager->profiles[name].toObject();
            const QString otp = ui->lineEditOTP->text();
            if(!otp.isEmpty()) {
                currentProfile["password"] = otp;
            }
        }

        rpc->callAsync(method, id, currentProfile, [this](const QJsonValue & result) {
            ui->progressBar->stop();
            if(result.isObject()) {  // error object
                // dialog
//                ui->statusBar->setText(result.toObject().value("message").toString());
                if(isHidden()) {
                    show();
                }
                error(result.toObject().value("message").toString(), this);
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

    connect(trayIcon,&QSystemTrayIcon::activated,[this](QSystemTrayIcon::ActivationReason reason){
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

    connect(ui->checkBoxAutoLogin, &QCheckBox::toggled, [](bool checked) {
        configManager->config["autoLogin"] = checked;
    });
    connect(ui->checkBoxMinimize, &QCheckBox::toggled, [](bool checked) {
        configManager->config["minimize"] = checked;
    });
    connect(ui->checkBoxBlock, &QCheckBox::toggled, [this](bool checked) {
        configManager->config["block"] = checked;
        configVPN();
    });
    connect(ui->checkBoxDebug, &QCheckBox::toggled, [this](bool checked) {
        // do not save debug state
        Q_UNUSED(checked);
//        configManager->config["debug"] = checked;
        configVPN();
    });
    connect(ui->checkBoxLang, &QCheckBox::toggled, [](bool checked) {
        configManager->config["local"] = checked;
    });
}

void AnyLink::afterShowOneTime()
{
    createTrayActions();
    createTrayIcon();
    initConfig();
    profileManager->afterShowOneTime();
    detailDialog = new DetailDialog(this);

    ui->labelVersionAgent->setText(getAgentVersion());

    connect(this, &AnyLink::vpnConnected, [this]() {
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

    connect(qApp, &QApplication::aboutToQuit, [this]() {
        if(m_vpnConnected) {
            disconnectVPN();
        }
        configManager->config["x"] = x();
        configManager->config["y"] = y();
        //profileManager->saveProfile(Json);
        configManager->saveConfig(Json);
    });

    rpc = new JsonRpcWebSocketClient(this);
    connect(rpc, &JsonRpcWebSocketClient::error, [this](const QString & error) {
        Q_UNUSED(error)
        ui->statusBar->setText(tr("Failed to connect to vpnagent, please reinstall the software!"));
        ui->buttonConnect->setEnabled(false);
        emit vpnClosed();
        if(isHidden()) {
            show();
        }
    });
    connect(rpc, &JsonRpcWebSocketClient::connected, [this]() {
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
        // the server side offline will lose its effect
        if(!activeDisconnect) {
            connectVPN(true);
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
}

QString AnyLink::getAgentVersion()
{
    QString version = "";
    QString agentFile = QCoreApplication::applicationDirPath() + "/vpnagent";
#if defined(Q_OS_WIN)
    agentFile += ".exe";
#endif
    if (QFileInfo::exists(agentFile)) {
        QProcess process;
        // go print to stderr, fmt.Print to stdout
        // process.setProcessChannelMode(QProcess::MergedChannels);
        process.start(agentFile, QStringList() << "version");

        if (!process.waitForFinished())
            qDebug() << "getAgentVersion error:" << process.errorString();
         else
            version = process.readAll();
            //qDebug() << "AgentVersion:" << version;
    }
    return version;
}

void AnyLink::closeEvent(QCloseEvent *event)
{
    if(m_vpnConnected) {
        hide();
        event->accept();
        if(!trayIcon->isVisible()) {
            trayIcon->show();
        }
    } else {
        qApp->quit();
    }
}

void AnyLink::showEvent(QShowEvent *event)
{
    if(trayIcon == nullptr) {
        QTimer::singleShot(50, [this]() {
            afterShowOneTime();
        });
    }
    event->accept();
}
