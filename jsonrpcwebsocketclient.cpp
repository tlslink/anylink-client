#include "jsonrpcwebsocketclient.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWebSocket>

JsonRpcWebSocketClient::JsonRpcWebSocketClient(QObject *parent)
    : QObject(parent)
{
    webSocket = new QWebSocket();

    connect(webSocket, &QWebSocket::connected, [this]() {
        m_connected = true;
        emit connected();
    });
    connect(webSocket, &QWebSocket::disconnected, [this]() {
        m_connected = false;
    });
    connect(webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), [this]() {
        m_connected = false;
        emit error(webSocket->errorString());
    });
    connect(webSocket, &QWebSocket::textMessageReceived, this, &JsonRpcWebSocketClient::onTextMessageReceived);
}

JsonRpcWebSocketClient::~JsonRpcWebSocketClient()
{
    webSocket->close();
    webSocket->deleteLater();
}

void JsonRpcWebSocketClient::connectToServer(const QUrl &url)
{
    webSocket->open(url);
}

bool JsonRpcWebSocketClient::isConnected()
{
    return m_connected;
}

void JsonRpcWebSocketClient::callAsync(const QString &method, const int id, const QJsonObject &args,
                                       std::function<void(QJsonValue)> callback)
{
    QJsonObject jsonRpc {
        {"method", method},
        {"jsonrpc", "2.0"},
        {"params", args},
        {"id", id},
    };

    QByteArray data = QJsonDocument(jsonRpc).toJson(QJsonDocument::Compact);

    if(webSocket->isValid()) {
        if(!m_callbacks.contains(id) && callback != nullptr) {
            // If there is already an item with the key, that item's value is replaced with value
            m_callbacks.insert(id, callback);
        }
        webSocket->sendBinaryMessage(data);
    }
}

void JsonRpcWebSocketClient::callAsync(const QString &method, const int id, std::function<void(QJsonValue)> callback)
{
    callAsync(method, id, QJsonObject(), callback);
}

/**
 * @brief don't send anything, just wait for the server to call.
 * @param id
 * @param callback
 */
void JsonRpcWebSocketClient::registerCallback(const int id, std::function<void (QJsonValue)> callback)
{
    if(!m_callbacks.contains(id)) {
        m_callbacks.insert(id, callback);
    }
}

void JsonRpcWebSocketClient::onTextMessageReceived(const QString &message)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

    if(parseError.error != QJsonParseError::NoError) {
        qWarning("Error parsing json document");
        return;
    }
    const QJsonObject &result = doc.object();
    if(!result.contains("id")) {
        qWarning("Invalid result");
        return;
    }

    int id = result.value("id").toInt();

    if(m_callbacks.contains(id)) {
        if(result.contains("error")) {
//            auto future = QtConcurrent::run(m_callbacks.value(id), result.value("error"));
            m_callbacks.value(id)(result.value("error"));
        } else {
//            auto future = QtConcurrent::run(m_callbacks.value(id), result.value("result"));
            m_callbacks.value(id)(result.value("result"));
        }
    }
}
