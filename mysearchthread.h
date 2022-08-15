#ifndef MYSEARCHTHREAD_H
#define MYSEARCHTHREAD_H

#include <QAbstractItemModel>
#include <QThread>
#include "myfileinfo.h"

class mySearchThread : public QThread
{
    Q_OBJECT

public:
    explicit mySearchThread();

private:
    void run();
    QStringList getFileListPath(const QString &dirPath);
    QList<myFileInfo*> m_plFileInfo;
    QString m_path;

signals:
    void notifyCompleted(QList<myFileInfo*> infos);
    void notifyNewFileName(QString fileName);

public:
    void proceedSearchRequest(const QString &dirPath);
};

#endif // MYSEARCHTHREAD_H
