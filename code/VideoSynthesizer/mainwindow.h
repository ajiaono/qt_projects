#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QBuffer>
#include <QQueue>
namespace Ui {
class MainWindow;
}
struct MetaInfo
{
    QString from,dest,name;
    MetaInfo()
    {

    }
    MetaInfo(QString f,QString d,QString n):from(f),dest(d),name(n)
    {

    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    QProcess    *caller ;
    QString     m_dataPath;
    bool        m_imageOverride;
    QFileInfo   m_cacheFileInfo;
    QBuffer     m_cacheFileBuffer;
    QQueue<MetaInfo> m_queue4MetaInfo;
    MetaInfo    m_currentMI;
    QString     m_currentTmpNameFilePath;
private:
    void doTrans(MetaInfo mi);
    void invokeTrans(QQueue<MetaInfo> &);

    void readCache(QString cachePath,QQueue<MetaInfo> & queue,QString destPath);
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event);
private slots:
    void slot_finished(int);
    void slot_errorOccurred(QProcess::ProcessError);
    void slot_readyRead();
    void slot_readyReadStandardError();
    void slot_readyReadStandardOutput();

    void on_BtnStopFFmpeg_clicked();
    void on_BtnExport_clicked(bool checked);





};

#endif // MAINWINDOW_H
