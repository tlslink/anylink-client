#include "common.h"
#include "configmanager.h"
#include <QMessageBox>

QString agentName = "AnyLink Secure Client";

QString appVersion = "0.8.0";

QString configLocation = "";
QString tempLocation = "";
ConfigManager *configManager = nullptr;

void error(const QString &message, QWidget *parent)
{
    QMessageBox msgBox(QMessageBox::Critical, QObject::tr("Error"), message, QMessageBox::Ok, parent);
    msgBox.setButtonText(QMessageBox::Ok, QObject::tr("OK"));
    msgBox.exec();
}

void info(const QString &message, QWidget *parent)
{
    QMessageBox msgBox(QMessageBox::Information, QObject::tr("Tips"), message, QMessageBox::Ok, parent);
    msgBox.setButtonText(QMessageBox::Ok, QObject::tr("OK"));
    msgBox.exec();
}
