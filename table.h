#ifndef TABLE_H
#define TABLE_H

#include "sqlite3Api/sqlite3api.h"
#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class table;
}

class MainWindow;
class table : public QWidget
{
    Q_OBJECT

public:
    explicit table(QWidget *parent = nullptr);
    ~table();

    //初始化种别码表
    void init_codeTable();
    //初始化种别码表值
    void init_Tablevalue();

    //QSqlTableModel重写data函数
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;

    MainWindow *maindlg;
private slots:
    //查找
    void on_pushButton_find_clicked();
    //更新
    void on_pushButton_update_clicked();
    //删除
    void on_pushButton_delete_clicked();
    //插入
    void on_pushButton_insert_clicked();

private:
    Ui::table *ui;
    sqlite3Api sqlite;//sqlite对象
};

#endif // TABLE_H
