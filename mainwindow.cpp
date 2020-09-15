#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QFileDialog>
#include <QDebug>
#include <QIODevice>
#include <QMessageBox>
#include <QGridLayout>
#include <QFontDialog>
#include <QColorDialog>

#include "about.h"
#include <QStatusBar>
#include <QLabel>
#include<QTextEdit>
#include <QDockWidget>//浮动窗口所需的头文件
#include <QClipboard>
#include <QTextLayout>
#include <QStringLiteral>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/image/logo.jpg"));
    setWindowTitle("词法语法分析器");//设计窗口标题
    //设置窗口大小
    setMinimumSize(1100,700);

    //初始化变量
    bufIndex = 0;
    legal = true;
    clipboard = QApplication::clipboard();  //获取系统剪贴板指针

    //状态栏
    cursorPos = new QLabel();
    ui->statusBar->addWidget(cursorPos);

    //初始化数据库
    sqlite.Init_Database();

    QString sql = "select * from keyCode";
    //执行sql语句
    QSqlQuery query;
    if(query.exec(sql))
    {
        qDebug()<<query.lastError().text();
    }

    //将单词符号和种别码映射关联起来
    while(query.next())
    {
        QString word = query.value("word").toString();
        int code = query.value("code").toInt();
        mapKeyCode[word] = code;
    }


    //设置菜单快捷键
//    ui->actionnew->setShortcut(tr("Ctrl+N"));
//    ui->actionSave->setShortcut(tr("Ctrl+S"));
//    ui->actionopen->setShortcut(tr("Ctrl+O"));
//    ui->actionClose->setShortcut(tr("Ctrl+W"));
//    ui->actionSave_As->setShortcut(tr("Ctrl+shift+S"));

    ui->tabWidget->clear();//关闭其他所有页面
    //tab关闭信号
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(removeSubTab(int)));

    //运行结果浮动窗口
    runDock = new QDockWidget("运行结果",this);
    addDockWidget(Qt::RightDockWidgetArea,runDock);
    //在浮动窗口上添加一个编辑器 即给浮动的窗口添加控件
    runTextEdit = new QTextEdit(this);
    //runTextEdit->setReadOnly(true);//设置只读 不可编辑
    runDock->setWidget(runTextEdit);

    //错误结果浮动窗口
    errorDock = new QDockWidget("错误结果",this);
    addDockWidget(Qt::RightDockWidgetArea,errorDock);
    //在浮动窗口上添加一个编辑器 即给浮动的窗口添加控件
    errorTextEdit = new QTextEdit(this);
    //errorTextEdit->setReadOnly(true);//设置只读 不可编辑
    errorDock->setWidget(errorTextEdit);

    tabifyDockWidget(runDock,errorDock);//将两个悬浮窗口合并到一起
    runDock->raise();//首先显示运行窗口
}

MainWindow::~MainWindow()
{
    delete ui;
}

//得到当前tab名
QString MainWindow::GetCurTabName()
{
    return ui->tabWidget->tabText(ui->tabWidget->currentIndex());
}

//获取编辑当前选择tab编辑框
QTextEdit *MainWindow::GetCurEditBox()
{
    int index = ui->tabWidget->currentIndex();
    QTextEdit *text = qobject_cast<QTextEdit*>(ui->tabWidget->widget(index));
    return text;
}
//创建新tab标签
void MainWindow::CreateNewTab(QString str)
{
    //新增一个tab
    QTextEdit *text = new QTextEdit();
    //关联编辑框光标位置显示
    connect(text, &QTextEdit::cursorPositionChanged, this, &MainWindow::showPosition);
    int ret = ui->tabWidget->addTab(text, str);
    ui->tabWidget->setCurrentIndex(ret);
}

//新建
void MainWindow::on_actionnew_triggered()
{
    CreateNewTab();
}
//打开
void MainWindow::on_actionopen_triggered()
{
    //实列化一个对象

    QFileDialog fileDialog(this);

    //设置窗口的标题
    fileDialog.setWindowTitle("打开文件");
    fileDialog.setNameFilter("Any files (*)"); //设置一个过滤器
    fileDialog.setDirectory("../测试数据");
    //这个标志用来设置选择的类型，比如默认是单个文件。QFileDialog::ExistingFiles 多个文件,还可以用来选择文件夹QFileDialog::Directory。QFileDialog::ExistingFile 单个文件。注意这个ExistingFile，单词后面多了一个s 表示选择多个文件。要看清楚了。
    fileDialog.setFileMode(QFileDialog::ExistingFiles);

    //弹出对话框
    if (fileDialog.exec() == QDialog::Accepted)
    {
        //strPathList  返回值是一个list，如果是单个文件选择的话，只要取出第一个来就行了。
        QStringList strPathList = fileDialog.selectedFiles();

        for(int i = 0; i < strPathList.size(); i++)
        {
            QString FilePath = strPathList[i];
            QFile file(FilePath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QMessageBox::warning(this, tr("Read File"),
                                     tr("Cannot open file:\n%1").arg(FilePath));
                return;
            }

            //获取文件名
            QFileInfo info = QFileInfo(FilePath);
            QString fileName = info.fileName();
            //新创建个页面
            CreateNewTab(fileName);
            //获取编辑当前选择tab编辑框
            QTextEdit *text = GetCurEditBox();
            //关联编辑框光标位置显示
            connect(text, &QTextEdit::cursorPositionChanged, this, &MainWindow::showPosition);
            //映射编辑框和文件路径对应关系
            mapPath[text] = FilePath;
            //将文件内容输入到编辑框中
            QTextStream in(&file);

            text->setText(in.readAll());
            file.close();
        }
    }
}

//保存文件
void MainWindow::on_actionSave_triggered()
{
    QTextEdit *text = GetCurEditBox();
    if(text->toPlainText().isEmpty())
        return ;
    //如果映射路径是空则另存为
    if(mapPath[text].isEmpty())
        on_actionSave_As_triggered();

    QString path = mapPath[text];
    if(path.isEmpty())
        return ;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Read File"),
                             tr("Cannot open file:\n%1").arg(path));
        return;
    }
    QTextStream out(&file);
    out << text->toPlainText();
    file.close();
}
//另存为
void MainWindow::on_actionSave_As_triggered()
{
    QTextEdit *text = GetCurEditBox();
    if(text->toPlainText().isEmpty())
        return ;

    QString path = QFileDialog::getSaveFileName(this,
                   tr("save File"), "./"+ GetCurTabName() +"", tr("Text files (*.txt);;C source files(*.c);;C++ source files(*.cpp *.cc *.cxx *.cp);;Header files(*.h *.hpp);;All Files(*.*)"));
    if(!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Read File"),
                                 tr("Cannot open file:\n%1").arg(path));
            return;
        }
        //获取文件名
        QFileInfo info = QFileInfo(path);
        QString fileName = info.fileName();
        //将编辑框内容另存为到文件中
        QTextStream out(&file);
        out <<  text->toPlainText();
        file.close();
        //设置文本框为文件名
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), fileName);
    }
}

//关闭当前编辑框
void MainWindow::on_actionClose_triggered()
{
    int index = ui->tabWidget->currentIndex();
    ui->tabWidget->removeTab(index);
}

//设置字体
void MainWindow::on_actionFont_triggered()
{
    //获取当前字体样式
    bool t;
    QFont font;
    QTextEdit *text = (QTextEdit *)ui->tabWidget->currentWidget();
    if(text == NULL)
        font = QFontDialog::getFont(&t,NULL);
    else
    {
        font = ui->tabWidget->currentWidget()->font();//获取当前编辑框样式
        font = QFontDialog::getFont(&t,font);
    }


    if(t==true)
    {
        QString decoration;
        QStringList list = font.toString().split(",");

        if(list[6] == "1" || list[7] == "1")
        {
            decoration = "text-decoration: ";
            if(list[6] == "1" && list[7] == "1")
                decoration.append("underline line-through;}");
            else if(list[6] == "1")
                decoration.append("underline;}");
            else if(list[7] == "1")
                decoration.append("line-through;}");

        }
        fontStyle = QString("QTextEdit{font: %1pt \"%2\";").arg(list[1]).arg(list[0]);

        if(!decoration.isEmpty())
            fontStyle.append(decoration);
        else
            fontStyle.append("}");

        //设置编辑框样式
        setQTextEditStyle();
    }
}

//设置字体颜色
void MainWindow::on_actionColot_triggered()
{
    QColor color = QColorDialog::getColor();

    colorStyle = QString("QTextEdit{color:%1}").arg(color.name());
    //设置编辑框样式
    setQTextEditStyle();
}

//设置背景图片
void MainWindow::on_actionBackground_triggered()
{
    QString path = QFileDialog::getOpenFileName(this,
                   tr("select Image File"), "../picture", tr("Image (*.png *.jpg *.bmp);;all file(*.*)"));
    if(!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Read File"),
                                 tr("Cannot open file:\n%1").arg(path));
            return;
        }

        background = QString("QTextEdit{border-image: url(%1);}").arg(path);
        //设置编辑框样式
        setQTextEditStyle();
    }
}

//设置编辑框样式
void MainWindow::setQTextEditStyle()
{
    QString style;

    if(!fontStyle.isEmpty())//字体
        style+=fontStyle;
    if(!colorStyle.isEmpty())//颜色
        style+=colorStyle;
    if(!background.isEmpty())//背景
        style+=background;

    ui->tabWidget->setStyleSheet(style);
}

void MainWindow::removeSubTab(int index)
{
    ui->tabWidget->removeTab(index);
}

//设置种别码表
void MainWindow::on_actionTable_triggered()
{
    tb = new table();
    tb->maindlg = this;
    tb->show();
}

//显示浮动窗口
void MainWindow::showDockWidget(QString title)
{
    showRunInfo(title);//显示编译结果信息
    showErrorInfo(title);//显示错误结果信息
}

//显示运行结果信息
void MainWindow::showRunInfo(QString title)
{
    runTextEdit->clear();//清空文本框
    int i=0;
    runTextEdit->append("-----"+title+"运行结果信息-----");

    if(runDock->isHidden())
        runDock->show();

    if(title == "语法分析")
    {
        if(0==errorList.size())
           runTextEdit->append("success!");
        else
           runTextEdit->append("error!");
    }
    else
    {
        if(0<runList.size())//如果运行列表不为空
        {
            for(i=0; i<runList.size(); i++)
            {
                QString str = QString("%1: %2").arg(i+1).arg(runList[i]);
                runTextEdit->append(str);
            }
        }
        runTextEdit->append(QString(title+"结束 - %1 序列").arg(i));
    }

}

// 显示错误结果信息
void MainWindow::showErrorInfo(QString title)
{
    errorTextEdit->clear();//清空文本框
    int i=0;
    errorTextEdit->append("-----"+title+"错误信息-----");

    if(errorDock->isHidden())
        errorDock->show();

    if(0<errorList.size())//如果错误列表不为空
    {
        for(i=0; i<errorList.size(); i++)
        {
            QString str = QString("%1: %2").arg(i+1).arg(errorList[i]);
            errorTextEdit->append(str);
        }
    }

    errorTextEdit->append(QString(title+"结束 - %1 error(s)").arg(i));
}

//更新种别码关联map
void MainWindow::updateCodeInfo()
{
    QString sql = "select * from keyCode";
    //执行sql语句
    QSqlQuery query;
    if(query.exec(sql))
    {
        qDebug()<<query.lastError().text();
    }

    //将单词符号和种别码映射关联起来
    while(query.next())
    {
        QString word = query.value("word").toString();
        int code = query.value("code").toInt();
        mapKeyCode[word] = code;
    }
}

//词法分析
void MainWindow::on_actionLexer_triggered()
{
    runList.clear();
    errorList.clear();

    QTextEdit *text = (QTextEdit *)ui->tabWidget->currentWidget();
    if(text == NULL)
        return ;

    bufIndex = 0;
    strBuf = text->toPlainText();

    if(strBuf.isEmpty())//判断内容是否为空
        return ;

    while(bufIndex < strBuf.size())
    {
        Scaner();

        if(keyCode == -2)
        {
            errorList.append("未以'#'符号结尾");
            break;
        }
        else if(keyCode != -1)
            runList.append(QString("(%1,%2)").arg(keyCode).arg(token));
    }

    //显示编译信息
    showDockWidget("词法分析");
}

void MainWindow::Scaner(void)//扫描
{
    token = "";//清空token字符串

    ch = strBuf[bufIndex++];
    while(ch == " ")//过滤空格
        ch = strBuf[bufIndex++];
    token+=ch;//先拼接字符串

    if (ch.isLetter())//如果开头是字符
    {
        ch = strBuf[bufIndex++];//读取下一个字符
        if(ch.isLetter() || ch.isDigit())//判断下一个是不是字母和数字
        {
            while(ch.isLetter() || ch.isDigit())//如果是字母和数字则循环拼接
            {
                token+=ch;//字符串联接
                ch = strBuf[bufIndex++];//读取一个字符
            }

            if(mapKeyCode.contains(token))//检查是否存在键值
                keyCode=mapKeyCode[token];//查找对应种别码
            else
                keyCode=mapKeyCode["letter"];//返回letter种别码
        }
        else
        {
            bufIndex--;//指针回退
            keyCode = mapKeyCode["letter"];//单个字符返回字符letter种别码
        }
    }
    else if(ch.isDigit())//如果是数字
    {
        ch = strBuf[bufIndex++];//读取下一个字符
        if(ch.isDigit())//如果下一个还是数字
        {
            while (ch.isDigit())//拼接数字
            {
                token+=ch;//字符串联接
                ch = strBuf[bufIndex++];//读取一个字符
            }
        }
        else
            bufIndex--;//指针回退

        keyCode=mapKeyCode["digit"];//返回数字种别码
    }
    else
    {
        switch(ch.unicode())
        {
            case '<':
                ch = strBuf[bufIndex++];//读取下一个字符
                if (ch == '=')//<=
                {
                    token+=ch;//字符串联接
                    keyCode=mapKeyCode[token];
                }
                else if(ch == '>')//<>
                {
                    token+=ch;//字符串联接
                    keyCode=mapKeyCode[token];
                }
                else
                {
                    bufIndex--;//指针回退
                    keyCode=mapKeyCode[token];
                }
            break;
            case '>':
                ch = strBuf[bufIndex++];//读取下一个字符
                if (ch == '=')//>=
                {
                    token+=ch;//字符串联接
                    keyCode=mapKeyCode[token];
                }
                else
                {
                    bufIndex--;//指针回退
                    keyCode=mapKeyCode[token];
                }
            break;
            case':':
                ch = strBuf[bufIndex++];//读取下一个字符
                if (ch == '=')
                {
                    token+=ch;//字符串联接
                    keyCode=mapKeyCode[token];
                }
                else
                {
                    bufIndex--;//指针回退
                    keyCode=mapKeyCode[token];
                }
            break;
            default:
                if(mapKeyCode.contains(token))//如果key存在 返回对应key（种别码）
                    keyCode=mapKeyCode[token];
                else
                {
                    legal = false;//语法分析 结果
                    keyCode=-1;
                    if(ch == '\0' || ch == ' ' || ch == '\n')
                        return ;
                    errorList.append(QString("第%1处 未识别字符: "+token).arg(bufIndex));
                    Scaner();//递归调用
                }
            break;
        }
    }
}

//语法分析
void MainWindow::on_actionParse_triggered()
{
    int i=0;
    legal = true;
    runList.clear();
    errorList.clear();

    QTextEdit *text = (QTextEdit *)ui->tabWidget->currentWidget();
    if(text == NULL)
        return ;

    bufIndex = 0;
    strBuf = text->toPlainText();

    if(strBuf.isEmpty())//判断内容是否为空
        return ;

    //<程序>::=begin<语句串>end
    Scaner();
    if (keyCode != mapKeyCode["begin"])//检查是否是begin关键字开头
    {
        errorList.append("缺少'begin'关键字开头!");
        while (i++ < token.length()) { bufIndex--; }//错误指针回退 便利下一次检测错误
    }

    StatementStr();//语句串

    //检查是否是'#'字符结尾
    bufIndex = strBuf.length();
    ch = strBuf[--bufIndex];
    while(ch == ' ' || ch == '\0' || ch == '\n') ch = strBuf[bufIndex--];//过滤空格
    if (ch != '#')
    {
        bufIndex = strBuf.length();//指针回退 重新从末尾开始
        errorList.append("缺少'#'字符结尾!");
    }

    //检测是否有end关键字结尾
    ch = strBuf[--bufIndex];
    while(ch == ' ' || ch == '\0' || ch == '\n') ch = strBuf[bufIndex--];//过滤空格
    bufIndex-=2;
    Scaner();
    if (keyCode != mapKeyCode["end"])//检查是否是end关键字结尾
    {
        errorList.append("缺少'end'关键字结尾!");
        while (i++ < token.length()) { bufIndex--; }//错误指针回退 便利下一次检测错误
    }

    //显示编译信息
    showDockWidget("语法分析");
}

//语句串
void MainWindow::StatementStr()
{

    //<语句串>::=<语句>{；<语句>}
    while(1)
    {
        Scaner();
        if(keyCode == mapKeyCode["end"] || keyCode == mapKeyCode["#"] || keyCode == -1 || keyCode == 0)
            break;
        Statement();//语句
        if(legal)
        {
            if(keyCode != mapKeyCode[";"])
            {
                int i=0;
                while (i++ <= token.length()) { bufIndex--; }//错误指针回退 便利下一次检测错误
                errorList.append(QString("第%1处 语句错误缺少';'结尾").arg(bufIndex));
                if(keyCode == mapKeyCode["end"] || keyCode == mapKeyCode["#"] || keyCode == -1 || keyCode == 0)
                    break;
            }

        }
        else
            legal = true;//赋值为真 检查下一个语句是否合法
    }
}
//语句
void MainWindow::Statement() {
    int i=0;
    //为标识符
    if (keyCode == mapKeyCode["letter"])
    {
        Scaner();
        if (keyCode == mapKeyCode[":="]) //:=
        {
            Scaner();
            expression();
        }
        else {
            errorList.append(QString("第%1处 赋值语句错误").arg(bufIndex));
            legal = false;
            while (i++ < token.length()) { bufIndex--; }//错误指针回退 便利下一次检测错误
        }
    }
    else if (keyCode == mapKeyCode["if"]) //if
    {
        Scaner();
        condition();
        if (keyCode == mapKeyCode["then"]) //then
        {
            Scaner();
            Statement();//表达式
        }
        else {
            errorList.append(QString("第%1处 语法错误缺少'then'关键字").arg(bufIndex));
            legal = false;
            while(1)//语句有问题 则跳过该语句 测试下一个语句
            {
                ch = strBuf[bufIndex];
                if(ch==' ' || ch=='\0')
                    break;
                bufIndex++;
            }
            //while (i++ < token.length()) { bufIndex--; }//错误指针回退 便利下一次检测错误
        }
    }
    else {
        errorList.append(QString("第%1处 语句错误").arg(bufIndex));
        legal = false;
        while(1)//语句有问题 则跳过该语句 测试下一个语句
        {
            ch = strBuf[bufIndex];
            if(ch==' ' || ch=='\0')
                break;
            bufIndex++;
        }
        //while (i++ < token.length()) { bufIndex--; }//错误指针回退 便利下一次检测错误
    }
}
//条件
void MainWindow::condition()
{
    expression();
    if (keyCode == mapKeyCode["<"] || keyCode == mapKeyCode["<>"] || keyCode == mapKeyCode["<="] ||
        keyCode == mapKeyCode[">"] || keyCode == mapKeyCode[">="] || keyCode == mapKeyCode["="])
    {
        Scaner();
    }
    else
    {
        errorList.append(QString("第%1处 条件语句错误").arg(bufIndex));
        legal = false;
        while(1)//语句有问题 则跳过该语句 测试下一个语句
        {
            ch = strBuf[bufIndex];
            if(ch==' ' || ch=='\0')
                break;
            bufIndex++;
        }
    }
    expression();
}
//表达式
void MainWindow::expression()
{
    term();
    //<表达式>::=<项>{+<项> | -<项>}
    while (keyCode == mapKeyCode["+"] || keyCode == mapKeyCode["-"]) // + -
    {
        Scaner();
        term();
    }
}
//项
void MainWindow::term()
{
    factor();
    //<项>：：=<因子>{*<因子> | /<因子>}
    while (keyCode == mapKeyCode["*"] || keyCode == mapKeyCode["/"])
    {
        Scaner();
        factor();
    }
}
//因子
void MainWindow::factor()
{
    if (keyCode == mapKeyCode["letter"] || keyCode == mapKeyCode["digit"])
    {
        //为标识符或整常数时，读下一个单词符号
        Scaner();
    }
    else if (keyCode == mapKeyCode["("])
    {
        Scaner();
        expression();
        if (keyCode == mapKeyCode[")"])
        {
            Scaner();
        }
        else
        {
            errorList.append(QString("第%1处 缺少')'符号").arg(bufIndex));
            legal = false;
            while(1)//语句有问题 则跳过该语句 测试下一个语句
            {
                ch = strBuf[bufIndex];
                if(ch==' ' || ch=='\0')
                    break;
                bufIndex++;
            }
        }
    }
    else {
        errorList.append(QString("第%1处 表达式错误").arg(bufIndex));
        legal = false;
        while(1)//语句有问题 则跳过该语句 测试下一个语句
        {
            ch = strBuf[bufIndex];
            if(ch==' ' || ch=='\0')
                break;
            bufIndex++;
        }
    }
    return;
}

//关于
void MainWindow::on_actionAbout_triggered()
{
    about *ab = new about();
    ab->show();
}

//显示光标位置
void MainWindow::showPosition()
{
    int row,col;
    //当前光标
    QTextEdit *te = (QTextEdit *)ui->tabWidget->currentWidget();
    QTextCursor tc = te->textCursor();

    col = tc.columnNumber();    //获取列
    row = tc.blockNumber();     //获取行

    cursorPos->setText(QString("行：%1 列：%2").arg(row).arg(col));
}

//拷贝
void MainWindow::on_actionCopy_triggered()
{
    QTextEdit *te = (QTextEdit *)ui->tabWidget->currentWidget();//获取当前编辑框指针
    if(te == NULL)
        return ;

    QString str = te->textCursor ().selectedText();

    clipboard->setText(str);                            //设置剪贴板内容
}

//粘贴
void MainWindow::on_actionPaste_triggered()
{
    QTextEdit *te = (QTextEdit *)ui->tabWidget->currentWidget();//获取当前编辑框指针
    if(te == NULL)
        return ;

    te->insertPlainText(clipboard->text());     //粘贴剪贴板上文本信息
}

void MainWindow::on_actionCut_triggered()
{
    QTextEdit *te = (QTextEdit *)ui->tabWidget->currentWidget();//获取当前编辑框指针
    if(te == NULL)
        return ;

    QString str = te->textCursor ().selectedText();
    clipboard->setText(str);                            //设置剪贴板内容

    te->textCursor().removeSelectedText();              //删除选择内容
}
//恢复默认设置
void MainWindow::on_actionReset_triggered()
{
    ui->tabWidget->setStyleSheet("");
}
