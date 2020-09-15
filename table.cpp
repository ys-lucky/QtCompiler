#include "table.h"
#include "ui_table.h"
#include <QMessageBox>
#include <QCloseEvent>
table::table(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::table)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/image/table.png"));
    setWindowTitle("种别码对应表");

    //初始化 种别码表
    init_codeTable();

    setAttribute(Qt::WA_DeleteOnClose);//关闭窗口时调用析构函数
}

table::~table()
{
    delete ui;
    maindlg->updateCodeInfo();
}

//初始化种别码表
void table::init_codeTable()
{
    sqlite.model  = new QSqlTableModel;

    //设置事务手动提交--默认是自动提交
    sqlite.model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    //绑定表格
    sqlite.model->setTable("keyCode");
    //查询数据
    sqlite.model->select();
    //sqlite.model->removeColumn(0);//去除ID字段

    //设置表水平头显示
    sqlite.model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    sqlite.model->setHeaderData(1, Qt::Horizontal, tr("单词符号"));
    sqlite.model->setHeaderData(2, Qt::Horizontal, tr("种别码"));

    ui->tableView->setAlternatingRowColors(true);//使表格颜色交错功能为真
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//表格宽度

    //模型与视图绑定
    ui->tableView->setModel(sqlite.model);
}

//查找
void table::on_pushButton_find_clicked()
{
    QString word = ui->lineEdit_word->text();//单词符号
    QString code = ui->lineEdit_code->text();//种别码
    QString filter;
    if(!word.isEmpty())
    {
        filter.append(QString("word='%1'").arg(word));
    }
    if(!code.isEmpty())
    {
        if(!filter.isEmpty())
            filter.append(" and ");
        filter.append(QString("code='%1'").arg(code));
    }

    //设置条件过滤 sql--where的条件
    sqlite.model->setFilter(filter);
    //查询数据
    sqlite.model->select();
}
//更新
void table::on_pushButton_update_clicked()
{
    sqlite.model->database().transaction(); //开始事务操作
    if (sqlite.model->submitAll()) // 提交所有被修改的数据到数据库中
    {
        sqlite.model->database().commit(); //提交成功，事务将真正修改数据库数据
    } else {
        sqlite.model->database().rollback(); //提交失败，事务回滚
        QMessageBox::warning(this, tr("tableModel"),tr("数据库错误: %1").arg(sqlite.model->lastError().text()));
    }
    sqlite.model->revertAll(); //撤销修改

}
//删除
void table::on_pushButton_delete_clicked()
{
    int curRow = ui->tableView->currentIndex().row();
    if(curRow<0) return ;
    if(!sqlite.model->removeRow(curRow))
    {
        QMessageBox::warning(this,"警告","删除失败！",QMessageBox::Ok);
        return ;
    }

    //提交事务
    sqlite.model->submitAll();
}
//插入
void table::on_pushButton_insert_clicked()
{
    QString word = ui->lineEdit_word->text();//单词符号
    QString code = ui->lineEdit_code->text();//种别码

    if(word.isEmpty() || code.isEmpty())
    {
        QMessageBox::warning(this,"警告","单词符号和种别码不能为空！",QMessageBox::Ok);
        return ;
    }

    //创建一条数据记录--根据当前表格创建（才能知道有那些字段）
    QSqlRecord record = sqlite.model->record();
    record.setValue("word",word);
    record.setValue("code", code);

    //把记录添加到表格模型中
    sqlite.model->insertRecord(0, record);
    //提交事务
    sqlite.model->submitAll();
}
