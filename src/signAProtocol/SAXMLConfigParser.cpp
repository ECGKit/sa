﻿#include "SAXMLConfigParser.h"
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDomDocument>
#include <QDomElement>
#include <QDomText>
#include <QDomNode>
#include <QDebug>
#include "SAVariantCaster.h"
#define CONFIG_FILE_NAME "saconfig.cfg"

#define CONFIG_ROOT_NAME "config"
#define CONFIG_GROUP_NAME "group"
#define CONFIG_CONTENT_PROP_NAME "name"
#define CONFIG_KEY_NAME "key"
#define CONFIG_KEY_PROP_NAME_NAME "name"
#define CONFIG_KEY_PROP_TYPE_NAME "type"
#define CONFIG_KEY_LIST_NAME "list"


class SAXMLConfigParserPrivate
{
    SA_IMPL_PUBLIC(SAXMLConfigParser)
public:
    QString m_cfgPath;
    bool m_isDirty;
    SAXMLConfigParserPrivate(SAXMLConfigParser* par);
    SAXMLConfigParserPrivate(SAXMLConfigParser* par,const QString& cfgPath);
    bool setCfgFile(const QString& cfgPath);
    bool save(const QString& saveFilePath);
};


SAXMLConfigParserPrivate::SAXMLConfigParserPrivate(SAXMLConfigParser *par)
    :q_ptr(par)
    ,m_isDirty(false)
{
}

SAXMLConfigParserPrivate::SAXMLConfigParserPrivate(SAXMLConfigParser *par, const QString &cfgPath)
    :q_ptr(par)
    ,m_isDirty(false)
{
    setCfgFile(cfgPath);
}


bool SAXMLConfigParserPrivate::setCfgFile(const QString &cfgPath)
{
    QFile file(cfgPath);
    if(!file.open(QIODevice::ReadWrite))
    {
        return false;
    }
    if(q_ptr->setProtocolData(file.readAll()))
    {
        m_cfgPath = cfgPath;
        return true;
    }
    return false;
}

bool SAXMLConfigParserPrivate::save(const QString &saveFilePath)
{
    QFile file(saveFilePath);
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        return false;
    }
    QTextStream txt(&file);
    txt.setCodec("UTF-8");
    txt << q_ptr->toString();
    txt.flush();
    file.close();
    m_isDirty = false;
    return true;
}


SAXMLConfigParser::SAXMLConfigParser(QObject *par):SAXMLProtocolParser(par)
    ,d_ptr(new SAXMLConfigParserPrivate(this))
{
    
}

SAXMLConfigParser::SAXMLConfigParser(const QString &filepath, QObject *par):SAXMLProtocolParser(par)
    ,d_ptr(new SAXMLConfigParserPrivate(this,filepath))
{
    
}

SAXMLConfigParser::~SAXMLConfigParser()
{

}

/**
 * @brief 设置配置文件路径
 * @param filePath
 * @return
 */
bool SAXMLConfigParser::setFilePath(const QString &filePath)
{
    return d_ptr->setCfgFile(filePath);
}

/**
 * @brief 获取配置文件路径
 * @return
 */
QString SAXMLConfigParser::getFilePath() const
{
    return d_ptr->m_cfgPath;
}

/**
 * @brief 设置内容，调用此函数会使dirty为true
 * @param groupName
 * @param keyName
 * @param var
 */
void SAXMLConfigParser::setValue(const QString &groupName, const QString &keyName, const QVariant &var)
{
    d_ptr->m_isDirty = true;
    SAXMLProtocolParser::setValue(groupName,keyName,var);
}

/**
 * @brief 判断是否有改变
 * @return
 */
bool SAXMLConfigParser::isDirty() const
{
    return d_ptr->m_isDirty;
}

/**
 * @brief 保存修改
 * 如果有打开文件，会保存到已有文件路径，如果没有打开文件，此函数不做任何动作
 * @return 成功返回true
 */
bool SAXMLConfigParser::save()
{
    if(d_ptr->m_cfgPath.isEmpty())
    {
        return false;
    }
    return saveAs(d_ptr->m_cfgPath);
}

/**
 * @brief 另存为
 * @param filePath 文件路径
 * @return 成功返回true
 */
bool SAXMLConfigParser::saveAs(const QString &filePath)
{
    return d_ptr->save(filePath);
}

/**
 * @brief 保护的setProtocolData函数，此函数在此类不能使用，用setFilePath代替
 * @param data
 * @return
 */
bool SAXMLConfigParser::setProtocolData(const QByteArray &data)
{
    return SAXMLProtocolParser::setProtocolData(data);
}

/**
 * @brief SAXMLConfigParser::splitNamePath
 * @param namePath
 * @param groupName
 * @param keyName
 */
void SAXMLConfigParser::splitNamePath(const QString &namePath, QString &groupName, QString &keyName)
{
    QStringList pl = namePath.split("/");
    if(1 == pl.size())
    {
        groupName = "";
        keyName = pl[0];
    }
    else if(2 == pl.size())
    {
        groupName = pl[0];
        keyName = pl[1];
    }
    else
    {
        groupName = "";
        keyName = namePath;
    }
}




