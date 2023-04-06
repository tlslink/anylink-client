#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "common.h"
#include <QObject>
#include <QJsonObject>

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    explicit ConfigManager(QObject *parent = nullptr);

    QJsonObject config {
        {"lastProfile", ""},
        {"autoLogin", false},
        {"minimize", true},
        {"allowLAN", true},
        {"block", false},
        {"debug", false},
        {"local", true},
        {"x", 0},
        {"y", 0}
    };
    bool loadConfig(SaveFormat saveFormat);
    void saveConfig(SaveFormat saveFormat);
};

#endif // CONFIGMANAGER_H
