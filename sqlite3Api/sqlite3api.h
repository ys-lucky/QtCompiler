#ifndef SQLITE3API_H
#define SQLITE3API_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QtSql/QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQueryModel>

class sqlite3Api :public QSqlQueryModel
{
public:
    sqlite3Api();

    //初始化数据库
    void Init_Database(QString path="./database/keyCode.db");
    //初始化默认种别码值
    void Init_defaultValue(void);
    //插入数据
    bool insertData(QString word, QString value);
    //查找数据
    QString FindData(QString table, QString fields, QString value);
    //删除数据
    bool deleteData(QString fields, QString value);
    //修改信息
    bool updateData(QString fields, QString value);

    //QSqlQueryModel重写data函数
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;

    //检查数据库中该值是否存在
    bool isExists(QString table, QString fields,QString value);

    QSqlTableModel *model;
protected:
    //检查文件是否存在 不存在则创建
    bool createFile(QString FilePath);
private:

    QStringList listFields;//0 word,1 code
    QStringList dataValue;//数据库默认值
};

#endif // SQLITE3API_H
