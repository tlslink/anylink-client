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
    ui->listProfile->setModel(model);

    setFixedSize(geometry().width(), geometry().height());
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
        saveFile.write(saveFormat == Json
                       ? QJsonDocument(profiles).toJson()
                       : QCborValue::fromJsonValue(profiles).toCbor());
    }
}

void ProfileManager::updateModel()
{
    model->setStringList(profiles.keys());
}

void ProfileManager::afterShowOneTime()
{
    ui->listProfile->setCurrentIndex(model->index(-1));

    connect(ui->listProfile->selectionModel(), &QItemSelectionModel::currentRowChanged, [this](const QModelIndex & current, const QModelIndex & previous) {
        Q_UNUSED(previous)
        const QString currentName = current.data().toString();
        if(!currentName.isEmpty()) {
            ui->buttonNew->setEnabled(true);
            ui->buttonDelete->setEnabled(true);
            ui->lineEditName->setEnabled(false);
            // selection changed
            if(currentName != ui->lineEditName->text()) {
                QJsonObject selectedProfile = profiles[currentName].toObject();

                ui->lineEditName->setText(currentName);
                ui->lineEditHost->setText(selectedProfile["host"].toString());
                ui->lineEditUsername->setText(selectedProfile["username"].toString());
                ui->lineEditPassword->setText(selectedProfile["password"].toString());
                ui->lineEditGroup->setText(selectedProfile["group"].toString());
            }
        }
        // only new not delete all will go here
    });
    connect(ui->buttonSave, &QPushButton::clicked, [this]() {
        const QString name = ui->lineEditName->text();
        const QString host = ui->lineEditHost->text();
        const QString username = ui->lineEditUsername->text();
        if(name.isEmpty() || host.isEmpty() || username.isEmpty()) {
            return;
        }
        QJsonObject newProfile {
            { "host", host },
            { "username", username },
            { "password", ui->lineEditPassword->text() },
            { "group", ui->lineEditGroup->text() }
        };
        profiles.insert(name, newProfile);
        updateModel();
        ui->listProfile->setCurrentIndex(model->index(profiles.keys().indexOf(name)));

        m_modified = true;
        close();
    });
    connect(ui->buttonNew, &QPushButton::clicked, [this]() {
        resetForm();
    });
    connect(ui->buttonDelete, &QPushButton::clicked, [this]() {
        profiles.remove(ui->lineEditName->text());
        updateModel();
        if(profiles.size()) {
            ui->listProfile->setCurrentIndex(model->index(0));
        } else {
            resetForm();
        }
        m_modified = true;
    });
}

void ProfileManager::resetForm()
{
    ui->listProfile->selectionModel()->clear();

    ui->buttonNew->setEnabled(false);
    ui->buttonDelete->setEnabled(false);

    ui->lineEditName->setEnabled(true);
    ui->lineEditName->clear();
    ui->lineEditHost->clear();
    ui->lineEditUsername->clear();
    ui->lineEditPassword->clear();
    ui->lineEditGroup->clear();
}
