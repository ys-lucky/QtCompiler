#include "about.h"
#include "ui_about.h"
#include <QIcon>
about::about(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/image/about.png"));
    setWindowTitle("关于");
}

about::~about()
{
    delete ui;
}
