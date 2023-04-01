#ifndef COMMON_H
#define COMMON_H

#include <QString>

class QWidget;
class ConfigManager;

enum SaveFormat {
    Json, Binary
};

extern QString uiVersion;
extern QString configLocation;
extern QString tempLocation;

extern ConfigManager *configManager;

void error(const QString &message, QWidget *parent = nullptr);

#endif // COMMON_H
