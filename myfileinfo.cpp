#include "myfileinfo.h"
#include <QMetaType>

myFileInfo::myFileInfo(const QString name, const float size, const QString createdTime, const QString path){
    m_fileName = name;
    m_fileSize = size;
    m_fileCreatedTime = createdTime;
    m_filePath = path;
}

myFileInfo::myFileInfo(){

}
