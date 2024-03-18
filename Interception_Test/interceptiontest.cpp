#include "interceptiontest.h"
#include "ui_interceptiontest.h"

InterceptionTest::InterceptionTest(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InterceptionTest)
{
    ui->setupUi(this);
}

InterceptionTest::~InterceptionTest()
{
    delete ui;
}
