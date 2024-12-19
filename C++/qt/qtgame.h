#ifndef QTGAME_H
#define QTGAME_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class qtgame; }
QT_END_NAMESPACE

class qtgame : public QMainWindow
{
    Q_OBJECT

public:
    qtgame(QWidget *parent = nullptr);
    ~qtgame();

private:
    Ui::qtgame *ui;
};
#endif // QTGAME_H
