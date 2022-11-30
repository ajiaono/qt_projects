#include "quick.h"

#include <QDataStream>


void write_buffer( QFile &file, QBuffer& buffer)  //写入缓冲区方法
{
    if( buffer.open(QIODevice::WriteOnly) && file.open(QIODevice::ReadOnly))  //以只写方式打开缓冲区
    {
        QDataStream out(&buffer);
        out << file.readAll();
        buffer.close();        //关闭缓冲区
        file.close();
    }
}
