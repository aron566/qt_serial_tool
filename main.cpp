#include "mainwindow.h"

#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QTextCodec *codec = QTextCodec::codecForName("System"); //获取系统编码
    QTextCodec::setCodecForLocale(codec);
    w.setWindowTitle("串口助手");
    w.show();
    return a.exec();
}
