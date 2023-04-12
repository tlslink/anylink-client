#include "common.h"
#include "configmanager.h"
#include <QMessageBox>

QString uiVersion = "0.4";

QString configLocation = "";
QString tempLocation = "";
ConfigManager *configManager = nullptr;

void error(const QString &message, QWidget *parent)
{
    QMessageBox msgBox(QMessageBox::Critical, QObject::tr("Error"), message, QMessageBox::Ok, parent);
    msgBox.setButtonText(QMessageBox::Ok, QObject::tr("OK"));
    msgBox.exec();
}
