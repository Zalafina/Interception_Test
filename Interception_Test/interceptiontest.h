#ifndef INTERCEPTIONTEST_H
#define INTERCEPTIONTEST_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class InterceptionTest;
}
QT_END_NAMESPACE

class InterceptionTest : public QWidget
{
    Q_OBJECT

public:
    InterceptionTest(QWidget *parent = nullptr);
    ~InterceptionTest();

private:
    Ui::InterceptionTest *ui;
};
#endif // INTERCEPTIONTEST_H
