#include "sqlite3api.h"

#include <QDir>
#include <QFileInfo>
#include <QMessageBox>

sqlite3Api::sqlite3Api()
{
    listFields.append("word");
    listFields.append("code");

    //默认种别码表值
    dataValue.append("begin");dataValue.append("1");
    dataValue.append("if");dataValue.append("2");
    dataValue.append("then");dataValue.append("3");
    dataValue.append("while");dataValue.append("4");
    dataValue.append("do");dataValue.append("5");
    dataValue.append("end");dataValue.append("6");
    dataValue.append("letter");dataValue.append("10");
    dataValue.append("dight");dataValue.append("11");
    dataValue.append("+");dataValue.append("13");
    dataValue.append("-");dataValue.append("14");
    dataValue.append("*");dataValue.append("15");
    dataValue.append("/");dataValue.append("16");
    dataValue.append(":");dataValue.append("17");
    dataValue.append(":=");dataValue.append("18");
    dataValue.append("<");dataValue.append("20");
    dataValue.append("<>");dataValue.append("21");
    dataValue.append("<=");dataValue.append("22");
    dataValue.append(">");dataValue.append("23");
    dataValue.append(">=");dataValue.append("24");
    dataValue.append("=");dataValue.append("25");
    dataValue.append(";");dataValue.append("26");
    dataValue.append("(");dataValue.append("27");
    dataValue.append(")");dataValue.append("28");
    dataValue.append("#");dataValue.append("0");
}

void sqlite3Api::Init_Database(QString path)
{
    //查看文件是否存在 不存在则创建
    bool result = createFile(path);

    //创建数据库连接句柄
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //设置数据库名称
    db.setDatabaseName(path);
    if(!db.open())
    {
        qDebug() << db.lastError().text();
    }

    //创建种别码对应表
    QString sql = "create table if not exists keyCode(id integer primary key autoincrement,"
                  "word varchar(150) not null, "
                  "code varchar(25) not null UNIQUE)";
    //执行sql语句
    QSqlQuery query;
    if(query.exec(sql))
    {
        qDebug()<<query.lastError().text();
    }
    query.finish();

    //创建样式存储表 存储字体 颜色 背景样式
    sql = "create table if not exists style(id integer primary key autoincrement,"
                  " fontStyle varchar(250),colorStyle varchar(250), background varchar(250))";
    //执行sql语句
    if(query.exec(sql))
    {
        qDebug()<<query.lastError().text();
    }
    query.finish();

    if(result)//创建了新db文件
        Init_defaultValue();//插入默认数据

}
//初始化默认种别码值
void sqlite3Api::Init_defaultValue()
{
    for(int i=0;i<dataValue.size();i+=2)
    {
        //插入默认数据 单词符号 种别码
        insertData(dataValue[i],dataValue[i+1]);
    }
}

//创建文件
bool sqlite3Api::createFile(QString FilePath)
{
    QFileInfo info = QFileInfo(FilePath);
    QString path = info.path();
    QString fileName = info.fileName();

    //创建目录
    QDir dir;
    if(!dir.exists(path))
    {
        dir.mkpath(path);
    }
    else
        return false;

    //创建文件
    QFile file;
    if(!file.exists(FilePath))
    {
        file.setFileName(FilePath);
        if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
        {
            qDebug()<<QObject::tr("打开失败");
            return false;
        }
    }
    else
    {
        file.close();
        return false;
    }
    file.close();
    return true;

}

//插入数据
bool sqlite3Api::insertData(QString word, QString value)
{
    QString sql = "insert into keyCode(word, code) values(?,?)";
    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(0,word);//单词符号
    query.bindValue(1,value);//种别码

    if(!query.exec())
    {
        qDebug()<<query.lastError().text();
        return true;
    }

    query.finish();
    return false;
}

//查找数据
QString sqlite3Api::FindData(QString table, QString fields, QString value)
{
    if(table.isEmpty() || fields.isEmpty() || value.isEmpty())
        return "";

    //查找对应表字段的值
    QString sql = QString("select * from %1 where %2 = '%3'").arg(table).arg(fields).arg(value);
    QSqlQuery query;
    if(!query.exec())
    {
        qDebug()<<query.lastError().text();
    }

    QSqlRecord record = query.record();

    //返回该字段的值
    return record.value(fields).toString();
}

//删除数据
bool sqlite3Api::deleteData(QString fields, QString value)
{
    QString sql = QString("delete from keyCode where %2 = '%3'").arg(fields).arg(value);
    QSqlQuery query;
    if(!query.exec())
    {
        qDebug()<<query.lastError().text();
        return false;
    }

    return true;
}

//更新数据
bool sqlite3Api::updateData(QString fields, QString value)
{
    QString sql = QString("update from keyCode set %2 = '%3'").arg(fields).arg(value);
    QSqlQuery query;
    if(!query.exec())
    {
        qDebug()<<query.lastError().text();
        return false;
    }

    return true;
}
//重写QSqlQueryModel data函数
QVariant sqlite3Api::data(const QModelIndex &item, int role) const
{
    if (role == Qt::TextAlignmentRole)//默认代理的对齐方式
    {
        QVariant value = (Qt::AlignCenter);
        return value;
    }
    else
        return QSqlQueryModel::data(item,role);
}

//检查数据库中该值是否存在
bool sqlite3Api::isExists(QString table, QString fields, QString value)
{
    if(table.isEmpty() || fields.isEmpty() || value.isEmpty())
        return "";

    //查找对应表字段的值
    QSqlQuery query;

    QString sql = QString("select * from %1 where %2 = '%3'").arg(table).arg(fields).arg(value);
    if(!query.next()){
        return false;
    }

    return true;
}
