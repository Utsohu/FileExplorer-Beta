#include "mysearchthread.h"
#include "mainwindow.h"
#include <QDir>
#include <QDateTime>
#include <QMetaType>
#include <QTimer>
#include <QEventLoop>
#include "myfileinfo.h"

mySearchThread::mySearchThread()
    : QThread ()
{
    qRegisterMetaType<QList<myFileInfo*>>("QList<myFileInfo*>");
    //connect(this,SIGNAL(notifyCompleted(QList<myFileInfo*>)),parent,SLOT(onSearchFinished(QList<myFileInfo*>)));
}


void mySearchThread::run(){
    /*QEventLoop eventloop;
    QTimer::singleShot(5000, &eventloop, SLOT(quit())); //wait 5s
    eventloop.exec();*/
    getFileListPath(m_path);
    emit notifyCompleted(m_plFileInfo);
}

QStringList mySearchThread::getFileListPath(const QString &dirPath){
    QStringList output = {};
    QDir dir(dirPath);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);
    foreach (auto fileInfo, fileInfoList){
        if (fileInfo.isDir()) {
            getFileListPath(fileInfo.absoluteFilePath());
        }
        else {
            QString fileName = fileInfo.fileName();
            float fileSize = fileInfo.size();
            QString filePath = fileInfo.absoluteFilePath();
            QString createdTime = fileInfo.created().toString();
            emit notifyNewFileName(fileName);
            myFileInfo* newInfo = new myFileInfo(fileName,fileSize,createdTime,filePath);
            m_plFileInfo.append(newInfo);
        }
    }
    fileInfoList.clear();
    return output;
}

void mySearchThread::proceedSearchRequest(const QString &dirPath){
    m_plFileInfo.clear();
    m_path = dirPath;
}
