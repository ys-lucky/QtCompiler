#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "table.h"
#include "sqlite3Api/sqlite3api.h"
#include <QMainWindow>
#include <QTextEdit>
#include <QMap>
#include <QLabel>
namespace Ui {
class MainWindow;
}
class table;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateCodeInfo();              //更新种别码关联map
protected:
    //得到当前tab名
    QString GetCurTabName();
    //获取编辑当前选择tab编辑框
    QTextEdit *GetCurEditBox();
    //创建新的编辑框
    void CreateNewTab(QString str = "new");

    void setQTextEditStyle();           //设置编辑框样式
    void Scaner();//扫描

    //语法分析
    void StatementStr();    //语句串
    void Statement();       //语句

    void condition();       //条件
    void expression();      //表达式
    void term();            //项
    void factor();          //因子

    //浮动窗口
    void showDockWidget(QString title);     //显示浮动窗口
    void showRunInfo(QString title);        //显示编译结果信息
    void showErrorInfo(QString title);      //显示错误结果信息
private slots:
    //File
    void on_actionnew_triggered();      //创建新的编辑框
    void on_actionopen_triggered();     //打开文件
    void on_actionSave_triggered();     //保存文件
    void on_actionSave_As_triggered();  //另存文件
    void on_actionClose_triggered();    //关闭当前页面

    void removeSubTab(int index);       //删除当前页面
    //Set
    void on_actionFont_triggered();         //设置字体
    void on_actionColot_triggered();        //设置字体颜色
    void on_actionBackground_triggered();   //设置背景图片
    void on_actionReset_triggered();    //恢复默认设置

    //词法分析
    void on_actionTable_triggered();    //设置种别码对应表
    void on_actionLexer_triggered();

    //语法分析
    void on_actionParse_triggered();    //语法分析

    void on_actionCut_triggered();      //剪切
    void on_actionCopy_triggered();     //复制
    void on_actionPaste_triggered();    //粘贴

    void on_actionAbout_triggered();    //关于

    void showPosition();              //显示光标位置
private:
    Ui::MainWindow *ui;

    QClipboard *clipboard;      // 剪切板

    QLabel *cursorPos;          // 当前光标位置
    QString fontStyle;          // 编辑框字体样式
    QString colorStyle;         // 编辑框颜色样式
    QString background;         // 编辑框背景图样式

    table *tb;//table对话框对象

    QDockWidget *runDock;       //运行结果浮动窗口
    QDockWidget *errorDock;     //错误结果浮动窗口

    QTextEdit *runTextEdit;     //运行浮动窗口文本框
    QTextEdit *errorTextEdit;   //错误浮动窗口文本框

    QMap<QTextEdit*, QString> mapPath;//映射编辑框文件路径
    QMap<QString, int> mapKeyCode;   //种别码映射

    sqlite3Api sqlite;          //数据库对象

    //词法分析
    int keyCode;            //种别码
    QChar ch;               //存储读取字符
    QString token;          //存放规则字符数组
    QString strBuf;         //存储输入字符串
    int bufIndex;           //strbuf索引

    QStringList runList;    //运行结果列表
    QStringList errorList;  //错误列表

    //语法分析
    bool legal;             //单条语句是否合法
};

#endif // MAINWINDOW_H
