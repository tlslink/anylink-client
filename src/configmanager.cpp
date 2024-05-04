#include "configmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QCborValue>
#include <QCborMap>
#include <QWidget>

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
}

bool ConfigManager::loadConfig(SaveFormat saveFormat)
{
    QFile loadFile(saveFormat == Json
                   ? configLocation + "/config.json"
                   : configLocation + "/config.dat");

    // In WriteOnly or ReadWrite mode, if the relevant file does not already exist,
    // this function will try to create a new file before opening it.
    // but the dir must be already exist!
    if(!loadFile.open(QIODevice::ReadWrite)) {
        error(tr("Couldn't open config file"));
        return false;
    }

    QByteArray data = loadFile.readAll();

    if (data.length()) {
        QJsonDocument loadDoc = (saveFormat == Json
                                     ? QJsonDocument::fromJson(data)
                                     : QJsonDocument(
                                         QCborValue::fromCbor(data).toMap().toJsonObject()));
        // Returns an empty object if the document contains an array
        const QJsonObject obj = loadDoc.object();
        for (auto it = config.begin(); it != config.end(); it++) {
            if (obj.contains(it.key())) {
                config[it.key()] = obj.value(it.key());
            }
        }
    }
    return true;
}

void ConfigManager::saveConfig(SaveFormat saveFormat)
{
    // as we need save widget coordinate,so no need m_modified
    QFile saveFile(saveFormat == Json
                   ? configLocation + "/config.json"
                   : configLocation + "/config.dat");

    if(!saveFile.open(QIODevice::WriteOnly)) {
        error(tr("Couldn't open config file"));
        return;
    }
    saveFile.write(saveFormat == Json
                   ? QJsonDocument(config).toJson()
                   : QCborValue::fromJsonValue(config).toCbor());

}

void ConfigManager::saveConfig()
{
    saveConfig(Json);
}
