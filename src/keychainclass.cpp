#include <QDebug>
#include "keychainclass.h"

using namespace QKeychain;


KeyChainClass::KeyChainClass(QObject* parent) :
    QObject(parent)
{
    // Job will auto delete after emit finished signal
}

void KeyChainClass::readKey(const QString &key)
{
    ReadPasswordJob  *readCredentialJob = new ReadPasswordJob(QLatin1String("keychain.anylink"));
    readCredentialJob->setKey(key);

    QObject::connect(readCredentialJob, &QKeychain::ReadPasswordJob::finished, [=](){
            if (readCredentialJob->error()) {
                emit error(tr("Read key failed: %1").arg(qPrintable(readCredentialJob->errorString())));
            return;
        }
        emit keyRestored(key, readCredentialJob->textData());
    });

    readCredentialJob->start();
}

void KeyChainClass::writeKey(const QString &key, const QString &value)
{
    WritePasswordJob  *writeCredentialJob = new WritePasswordJob(QLatin1String("keychain.anylink"));
    writeCredentialJob->setKey(key);

    QObject::connect(writeCredentialJob, &QKeychain::WritePasswordJob::finished, [=](){
        if (writeCredentialJob->error()) {
            emit error(tr("Write key failed: %1").arg(qPrintable(writeCredentialJob->errorString())));
            return;
        }

        emit keyStored(key);
    });

    writeCredentialJob->setTextData(value);
    writeCredentialJob->start();
}

void KeyChainClass::deleteKey(const QString &key)
{
    DeletePasswordJob  *deleteCredentialJob = new DeletePasswordJob(QLatin1String("keychain.anylink"));
    deleteCredentialJob->setKey(key);

    QObject::connect(deleteCredentialJob, &QKeychain::DeletePasswordJob::finished, [=](){
        if (deleteCredentialJob->error()) {
            emit error(tr("Delete key failed: %1").arg(qPrintable(deleteCredentialJob->errorString())));
            return;
        }
        emit keyDeleted(key);
    });

    deleteCredentialJob->start();
}
