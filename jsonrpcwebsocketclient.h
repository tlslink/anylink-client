#ifndef JSONRPCWEBSOCKETCLIENT_H
#define JSONRPCWEBSOCKETCLIENT_H

#include <QObject>
#include <QHash>
#include <functional>

class QWebSocket;

class JsonRpcWebSocketClient : public QObject
{
    Q_OBJECT
public:
    explicit JsonRpcWebSocketClient(QObject *parent = nullptr);
    ~JsonRpcWebSocketClient();

    void connectToServer(const QUrl &url);
    bool isConnected();

    void callAsync(const QString &method, const int id, const QJsonObject &args,
                   std::function<void(QJsonValue)> callback = nullptr);
    void callAsync(const QString &method, const int id,
                   std::function<void(QJsonValue)> callback = nullptr);

    void registerCallback(const int id, std::function<void(QJsonValue)> callback);

signals:
    void error(const QString &error) const;
    void connected();
private slots:
    void onTextMessageReceived(const QString &message);
private:
    bool m_connected = false;
    QHash<int, std::function<void(QJsonValue)>> m_callbacks;

    QWebSocket *webSocket;
};

#endif // JSONRPCWEBSOCKETCLIENT_H
