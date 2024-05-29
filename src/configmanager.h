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

    QJsonObject config{{"lastProfile", ""},
                       {"autoLogin", true},
                       {"minimize", true},
                       {"block", false},
                       {"debug", false},
                       {"local", true},
                       {"no_dtls", false},
                       {"cisco_compat", true}};
    bool loadConfig(SaveFormat saveFormat);
    void saveConfig(SaveFormat saveFormat);
    void saveConfig();
};

#endif // CONFIGMANAGER_H
