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
                       {"autoLogin", false},
                       {"minimize", true},
                       {"block", true},
                       {"debug", false},
                       {"local", true},
                       {"cisco_compat", false}
                      };
    bool loadConfig(SaveFormat saveFormat);
    void saveConfig(SaveFormat saveFormat);
    void saveConfig();
};

#endif // CONFIGMANAGER_H
