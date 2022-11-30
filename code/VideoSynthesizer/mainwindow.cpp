#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QDebug>
#include <QPainter>
#include <QFileInfo>
#include <QDesktopServices>
#include <QMessageBox>
#include <QFileDialog>
#include "quick.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    m_dataPath = QString("%0/data").arg(QApplication::applicationDirPath());


    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("QLabel{color:black;font:\"Microsoft YaHei\";}");
    setWindowTitle(u8"BiliBili缓存提取");


    caller = new QProcess();
    caller->setProcessChannelMode(QProcess::SeparateChannels);
    connect(caller,SIGNAL(readyRead()),this,SLOT(slot_readyRead()));
    connect(caller,SIGNAL(readyReadStandardError()),this,SLOT(slot_readyReadStandardError()));
    connect(caller,SIGNAL(readyReadStandardOutput()),this,SLOT(slot_readyReadStandardOutput()));
    connect(caller,SIGNAL(finished(int)),this,SLOT(slot_finished(int)));
    connect(caller,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(slot_errorOccurred(QProcess::ProcessError)));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::doTrans(MetaInfo mi)
{
    m_currentMI = mi;
    QString currentFileName = QString::number(QDateTime::currentMSecsSinceEpoch());
    if(caller->state()!= QProcess::NotRunning) return;
    QStringList arguments;
    arguments << "-i" << QString("%0/audio.m4s").arg(mi.from);
    arguments << "-i" << QString("%0/video.m4s").arg(mi.from);
    arguments << "-c" << "copy" << QString("%0/%1.mp4").arg(mi.dest).arg(currentFileName);
    m_currentTmpNameFilePath =  QString("%0/%1.mp4").arg(mi.dest).arg(currentFileName);
    caller->start("ffmpeg.exe", arguments);
}



void MainWindow::invokeTrans(QQueue<MetaInfo> &queue)
{
    if(queue.isEmpty()) return;
    auto mi = queue.dequeue();
    ui->TB_FILENAME->append(QString("导出%0").arg(mi.name));
    doTrans(mi);
}

void MainWindow::readCache(QString cachePath, QQueue<MetaInfo> &queue, QString destPath)
{
    QDir dir(cachePath);
    if(!dir.exists()) return;

    auto nameList = dir.entryList(QDir::AllDirs);

    foreach (auto name, nameList) {
         QDir dirLv2( cachePath + "/" + name);
         auto cVideos = dirLv2.entryList(QDir::AllDirs);
         foreach (auto videoDirName, cVideos) {
              QString videoRoot( cachePath + "/" + name + "/" + videoDirName);
              QString videoJson( cachePath + "/" + name + "/" + videoDirName + "/entry.json");
              QFile file(videoJson);	//创建QFile对象，并指定json文件路径
              if(!file.open(QIODevice::ReadOnly)) continue;//打开json文件并判断（不成功则返回0）
              QByteArray data(file.readAll());//将文件内容读取到数组中
              file.close();	//关闭文件
              QJsonParseError jError;	//创建QJsonParseError对象,使用QJsonDocument的fromJson函数读取json串，并将QJsonParseError对象传入获取错误值
              QJsonDocument jDoc = QJsonDocument::fromJson(data, &jError);
              //判断QJsonParseError对象获取的error是否包含错误，包含则返回0
              if(jError.error != QJsonParseError::NoError)continue;
              QJsonObject jObj = jDoc.object();
              QString name = jObj["title"].toString();
              name.replace(" ","_");
              QString tagPath = jObj["type_tag"].toString();
              MetaInfo mi(videoRoot + "/" + tagPath,destPath,name);
              queue.enqueue(mi);
         }
    }
}
void MainWindow::on_BtnExport_clicked(bool checked)
{
    if(caller->isOpen())
    {
        ui->TB_FFMPEGLOG->append("STILL RUNNING");
        return;
    }
    readCache(ui->InputCachePath->text(),m_queue4MetaInfo,ui->InputExportPath->text());
    invokeTrans(m_queue4MetaInfo);
    ui->TB_FFMPEGLOG->clear();
}

void MainWindow::slot_finished(int)
{
    caller->close();
    if(QFile::exists(m_currentTmpNameFilePath))
    {
        QFile::rename(m_currentTmpNameFilePath,
                      QString(ui->InputExportPath->text() + "/" + m_currentMI.name + ".mp4"));
    }
    invokeTrans(m_queue4MetaInfo);
}

void MainWindow::slot_errorOccurred(QProcess::ProcessError error)
{
    qDebug () << "error occured:" << error;
    qDebug () << caller->errorString();
}

void MainWindow::slot_readyRead()
{

}

void MainWindow::slot_readyReadStandardError()
{
    qDebug() << __FUNCTION__;
    QString data = caller->readAllStandardError();
    QString  str=  "<font color=\"#FF0000\">" + data + "</font>";
    ui->TB_FFMPEGLOG->append(str);
}

void MainWindow::slot_readyReadStandardOutput()
{
    qDebug() << __FUNCTION__;
    QString data = caller->readAllStandardOutput();

    QString  str= u8"<font color=\"#00FF00\">" + data + "信息"+ "</font>";

    ui->TB_FFMPEGLOG->append(str);
}




void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.fillRect(rect(),QColor(0xf5f6f7));
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    return QMainWindow::eventFilter(watched, event);
}

//void MainWindow::on_BtnOpenDir_clicked()
//{
//    QString path = QString("file:%0").arg(m_dataPath);
//    QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
//}



void MainWindow::on_BtnStopFFmpeg_clicked()
{
    if(caller->isOpen())
    {
        caller->kill();
    }
}


