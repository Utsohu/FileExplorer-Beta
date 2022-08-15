#ifndef MYFILEINFO_H
#define MYFILEINFO_H

#include <QAbstractItemModel>

class myFileInfo
{

public:
    explicit myFileInfo(const QString name, const float size, const QString createdTime, const QString path);
    explicit myFileInfo();
    QString m_fileName;
    float m_fileSize;
    QString m_fileCreatedTime;
    QString m_filePath;

private:
};

#endif // MYFILEINFO_H
