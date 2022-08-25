#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QProgressDialog>
#include <QLineEdit>
#include "myfileinfo.h"
#include "mysearchthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString language, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QFileSystemModel* m_model;
    QFileSystemModel* m_saveModel;
    QStandardItem* m_FileModel;
    QStandardItemModel* m_parentFileModel;
    void getFileListUnderDir(const QString& dirPath);
    QStringList getFileListPath(const QString& dirPath);
    void TableViewInitial();
    void showSearchProceed();
    QList<myFileInfo*> m_plFileInfo;
    QList<myFileInfo*> m_plShowInfo;
    mySearchThread* searchThread;
    QProgressDialog* m_progressdlg;
    QPushButton* m_cancelBtn;
    QString m_savePath;
    QString m_openFilePath;
    QString m_currentSearchingFile = "";
    QLineEdit* m_saveFileNameEdit;
    QLineEdit* m_saveFilePathEdit;
    QDialog* m_saveDialog;
    QModelIndex clickedPos;
    bool haveReceived = false;
    int m_hTimer;

public:

protected:
    virtual void timerEvent(QTimerEvent *e);
private slots:
    void TreeClicked(const QModelIndex &index);
    void onSearchFinished(const QList<myFileInfo*> info);
    void onExportActivated(bool isClicked);
    void searchTargetFiles(bool isClicked);
    void aboutDeveloper(bool isClicked);
    void onDoubleClicked(const QModelIndex &index);
    void onSavePathChanged(const QModelIndex &index);
    void onSaveConfirmed(bool isClicked);
    void onBrowserRightClicked(QPoint pos);
    void onBrowserDeleted(bool isClicked);
    void onBrowserOpen(bool isClicked);
    void onQuitTriggered(bool isClicked);
    void onSearchFileChanged(QString fileName);
    void finishSearching(bool isClicked);


signals:
    void requestSearch(const QString &dirPath);
};

#endif // MAINWINDOW_H
