#ifndef COMMON_H
#define COMMON_H

#include <QString>

class QWidget;
class ConfigManager;

enum SaveFormat {
    Json, Binary
};

extern QString agentName;

extern QString appVersion;
extern QString configLocation;
extern QString tempLocation;

extern ConfigManager *configManager;

void error(const QString &message, QWidget *parent = nullptr);
void info(const QString &message, QWidget *parent = nullptr);

#endif // COMMON_H
