#include "profilemanager.h"
#include "ui_profilemanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QCborValue>
#include <QCborMap>
#include <QStringListModel>
#include <QListView>
#include <QModelIndex>
#include <QPushButton>
#include <QItemSelectionModel>
#include <QRegularExpressionValidator>
#include <QRegularExpression>

ProfileManager::ProfileManager(QWidget *parent) :
    QDialog(parent),
    model(new QStringListModel()), ui(new Ui::ProfileManager)
{
    ui->setupUi(this);
    ui->labelSecretkey->setHidden(true);
    ui->lineEditSecretkey->setHidden(true);

    ui->listProfile->setModel(model);

    setFixedSize(geometry().width(), geometry().height());

    connect(&keyChain,
            &KeyChainClass::keyRestored,
            this,
            [this](const QString &key, const QString &password) {
                QJsonObject value = profiles.value(key).toObject();
                value.insert("password", password);
                profiles.insert(key, value);

                emit keyRestored(key);

                //        qDebug() << "keyRestored" << key << password << profiles;
            });
    //    connect(&keyChain,&KeyChainClass::error,[](const QString& error){
    //        qDebug() << error;
    //    });
    //    connect(&keyChain,&KeyChainClass::keyStored,[](const QString& key){
    //        qDebug() << "keyStored" << key;
    //    });
}

ProfileManager::~ProfileManager()
{
    delete ui;
}

bool ProfileManager::loadProfile(SaveFormat saveFormat)
{
    QFile loadFile(saveFormat == Json
                   ? configLocation + "/profile.json"
                   : configLocation + "/profile.dat");

    // In WriteOnly or ReadWrite mode, if the relevant file does not already exist,
    // this function will try to create a new file before opening it.
    // but the dir must be already exist!
    if(!loadFile.open(QIODevice::ReadWrite)) {
        error(tr("Couldn't open profile file"), parentWidget());
        return false;
    }

    QByteArray data = loadFile.readAll();
    QJsonDocument loadDoc;

    if(data.length()) {
        loadDoc = (saveFormat == Json
                   ? QJsonDocument::fromJson(data)
                   : QJsonDocument(QCborValue::fromCbor(data).toMap().toJsonObject()));
        // Returns an empty object if the document contains an array
        profiles = loadDoc.object();

        readKeys();
    }
    return true;
}

void ProfileManager::saveProfile(SaveFormat saveFormat)
{
    if(m_modified) {
        QFile saveFile(saveFormat == Json
                       ? configLocation + "/profile.json"
                       : configLocation + "/profile.dat");

        if(!saveFile.open(QIODevice::WriteOnly)) {
            error(tr("Couldn't open profile file"), parentWidget());
            return;
        }

        writeKeys();
        QJsonObject saveProfiles = profiles;
        for(auto it = saveProfiles.begin(); it != saveProfiles.end(); it++) {
            const QString key = it.key();
            QJsonObject value = it.value().toObject();
            value.remove("password");
            saveProfiles.insert(key, value);
        }
        saveFile.write(saveFormat == Json
                       ? QJsonDocument(saveProfiles).toJson()
                       : QCborValue::fromJsonValue(saveProfiles).toCbor());
    }
}

void ProfileManager::updateModel()
{
    model->setStringList(profiles.keys());
}

void ProfileManager::afterShowOneTime()
{
    ui->listProfile->setCurrentIndex(model->index(-1));

    connect(ui->listProfile->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this,
            [this](const QModelIndex &current, const QModelIndex &previous) {
                Q_UNUSED(previous)
                const QString currentName = current.data().toString();
                if (!currentName.isEmpty()) {
                    ui->buttonNew->setEnabled(true);
                    ui->buttonDelete->setEnabled(true);
                    ui->lineEditName->setEnabled(false);
                    ui->lineEditName->setClearButtonEnabled(false);
                    // selection changed
                    if (currentName != ui->lineEditName->text()) {
                        QJsonObject selectedProfile = profiles[currentName].toObject();

                        ui->lineEditName->setText(currentName);
                        ui->lineEditHost->setText(selectedProfile["host"].toString());
                        ui->lineEditUsername->setText(selectedProfile["username"].toString());
                        ui->lineEditPassword->setText(selectedProfile["password"].toString());
                        ui->lineEditGroup->setText(selectedProfile["group"].toString());
                        ui->lineEditSecretkey->setText(selectedProfile["secret"].toString());
                    }
                }
                // only new not delete all will go here
            });
    connect(ui->buttonSave, &QPushButton::clicked, this, [this]() {
        const QString name = ui->lineEditName->text().trimmed();
        const QString host = ui->lineEditHost->text().trimmed();
        const QString username = ui->lineEditUsername->text().trimmed();
        if(name.isEmpty() || host.isEmpty() || username.isEmpty()) {
            return;
        }
        const QString password = ui->lineEditPassword->text().trimmed();
        //        if (profiles.isEmpty()) {
        //            info(tr("This software can save passwords in the Keychain of the operating system to avoid plaintext passwords, but you should evaluate whether your usage scenarios allow saving passwords and avoid potential security risks."),this);
        //        }

        QJsonObject newProfile {
            { "host", host },
            { "username", username },
            { "password", password },
            { "group", ui->lineEditGroup->text().trimmed() },
            { "secret", ui->lineEditSecretkey->text().trimmed() }
        };
        profiles.insert(name, newProfile);
        updateModel();
        ui->listProfile->setCurrentIndex(model->index(profiles.keys().indexOf(name)));

        m_modified = true;
        close();

        saveProfile(Json);
    });
    connect(ui->buttonNew, &QPushButton::clicked, this, [this]() { resetForm(); });
    connect(ui->buttonDelete, &QPushButton::clicked, this, [this]() {
        const QString name = ui->lineEditName->text();
        profiles.remove(name);
        updateModel();
        if(profiles.size()) {
            ui->listProfile->setCurrentIndex(model->index(0));
        } else {
            resetForm();
        }
        m_modified = true;

        saveProfile(Json);
        keyChain.deleteKey(name);
    });
}

void ProfileManager::resetForm()
{
    ui->listProfile->selectionModel()->clear();

    ui->buttonNew->setEnabled(false);
    ui->buttonDelete->setEnabled(false);

    ui->lineEditName->setEnabled(true);
    ui->lineEditName->setClearButtonEnabled(true);

    ui->lineEditName->clear();
    ui->lineEditHost->clear();
    ui->lineEditUsername->clear();
    ui->lineEditPassword->clear();
    ui->lineEditGroup->clear();
    ui->lineEditSecretkey->clear();
}

void ProfileManager::readKeys()
{
    for(auto it = profiles.begin(); it != profiles.end(); it++) {
        const QString key = it.key();
        keyChain.readKey(key);
//        qDebug() << "readKey" << key;
    }
}

void ProfileManager::writeKeys()
{
    const QString key = ui->lineEditName->text();
    const QString password = ui->lineEditPassword->text();

    // 不能在调用 aboutToQuit 后使用
    keyChain.writeKey(key, password);
}
