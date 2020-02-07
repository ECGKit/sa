#ifndef SATCPCLIENT_H
#define SATCPCLIENT_H
#include "SAServeGlobal.h"
#include "SAAbstractProtocolParser.h"
#include "SAProtocolHeader.h"
#include <QAbstractSocket>
#include <QObject>

#define __PRINT_FUNCTION_RUN_INFO 1
#if __PRINT_FUNCTION_RUN_INFO
#include <QDebug>
#define FUNCTION_RUN_PRINT() \
    do{\
      qDebug() << " >> fun:" << __FUNCTION__ << " line:" << __LINE__;\
    }while(0)
#else
#define FUNCTION_RUN_PRINT()
#endif

class SATcpClientPrivate;
class SATcpSocket;
/**
 * @brief 客户端的封装
 *
 * 此客户封装了最基本的token请求和心跳检测
 *
 * 具体的客户端需要继承此类，如SATcpDataProcessClient是专门进行数据处理用的客户端
 */
class SASERVE_EXPORT SATcpClient : public QObject
{
    SA_IMPL(SATcpClient)
    Q_OBJECT
public:
    SATcpClient(QObject* par = nullptr);
    ~SATcpClient();
    //向socket写数据
    bool write(const SAProtocolHeader& header,const QByteArray& data);
    //处理协议数据的函数
    virtual bool deal(const SAProtocolHeader &header, const QByteArray &data);
    virtual bool dealResult(const SAProtocolHeader &header, const QVariantHash &data);
    //获取socket
    inline SATcpSocket* getSocket() const;
public slots:
    //连接服务器
    bool connectToServe(int timeout = 5000);
    //断开和服务器的连接
    void disconnectFromServe();
    //心跳检测设置 - 默认为false，30秒
    void setHeartbreakCheck(bool enable,int checkFreS = 30);
    //请求token
    void requestToken(int pid,const QString& appid);
    //主动请求心跳,服务器收到会立马回复一个心跳协议
    void requestHeartbreat();
    //连接和断开的槽，本例方法未做任何事情
    void onSocketConnected();
    //关闭客户端
    void cloese();
private slots:
    void onReceivedData(const SAProtocolHeader &header, const QByteArray &data);
    void onHeartbreatCheckTimeout();
private:
    void createSocket();
    void destroySocket();
    void connectSocket();
    void disconnectSocket();
signals:
    /**
     * @brief 连接服务器超时
     */
    void connectTimeout();
    /**
     * @brief 返回请求的token
     * @param token
     * @see SATcpClient::requestToken
     */
    void replyToken(const QString& token,int sequenceID);
    /**
     * @brief error
     * @param socketError
     */
    void error(QAbstractSocket::SocketError socketError);
    /**
     * @brief socket连接成功的信号
     */
    void connected();
    /**
     * @brief socket连接丢失的信号
     */
    void disconnected();
    /**
     * @brief 心跳超时
     * @param lastDatetime 上次心跳的时间
     */
    void heartbreatTimeout(const QDateTime& lastDatetime);
    /**
     * @brief 客户端结束
     */
    void aboutToClose();
};

#endif // SATCPCLIENT_H
