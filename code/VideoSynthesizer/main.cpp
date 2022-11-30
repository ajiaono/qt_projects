#include "mainwindow.h"
#include <QApplication>
//#include <QTextCodec>
int main(int argc, char *argv[])
{

//#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif
    QApplication a(argc, argv);
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

    MainWindow w;
    w.show();

    return a.exec();
}
