#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QDateTime>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QMenu>
#include <QMenuBar>
#include <QUrl>
#include <QLabel>
#include <QDesktopServices>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTranslator>
#include "mysearchthread.h"

MainWindow::MainWindow(QString language, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QTranslator translator;

    if (language == "cn"){
        translator.load(":/languages/FileExplorer_cn.qm");
    }
    else if (language == "fr"){
        translator.load(":/languages/FileExplorer_fr.qm");
    }
    else{
        translator.load(":/languages/FileExplorer_en.qm");
    }
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);


    qRegisterMetaType<QList<myFileInfo*>>("QList<myFileInfo*>");
    TableViewInitial();
    m_model = new QFileSystemModel();
    m_model->setFilter(QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
    //m_model->setRootPath("D:/AndroidSDK");
    m_model->setRootPath("/");
    ui->fileView->setModel(m_model);
    QWidget::setWindowTitle("FileExplorer");
    QWidget::setWindowIcon(QIcon(":/images/smallIcon.png"));
    ui->searchBtn->setText(tr("search"));
    ui->fileBrowser->setSortingEnabled(true);
    ui->fileBrowser->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->fileBrowser->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->fileBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    //ui->fileView->setRootIndex(m_model->index("D:/AndroidSDK"));
    searchThread = new mySearchThread();
    connect(searchThread,SIGNAL(notifyCompleted(QList<myFileInfo*>)),this,SLOT(onSearchFinished(QList<myFileInfo*>)));
    connect(searchThread,SIGNAL(notifyNewFileName(QString)),this,SLOT(onSearchFileChanged(QString)));
    connect(ui->searchBtn,SIGNAL(clicked(bool)),this,SLOT(searchTargetFiles(bool)));
    m_progressdlg = 0;
    connect(ui->fileView,SIGNAL(clicked(QModelIndex)),this,SLOT(TreeClicked(QModelIndex)));
    //getFileListUnderDir("D:/AndroidSDK/emulator/bin64");

    QMenu* menuFile = new QMenu(this);
    menuFile->setTitle(tr("file "));
    QAction* excelAction = new QAction(tr("Export excel"));
    connect(excelAction,SIGNAL(triggered(bool)),this,SLOT(onExportActivated(bool)));
    menuFile->addAction(excelAction);
    QAction* quitAction = new QAction(tr("Exit"));
    connect(quitAction,SIGNAL(triggered(bool)),this,SLOT(onQuitTriggered(bool)));
    menuFile->addAction(quitAction);

    QMenu* menu = new QMenu(this);
    menu->setTitle(tr("help"));
    QAction* aboutAction = new QAction(tr("About"));
    menu->addAction(aboutAction);
    connect(aboutAction,SIGNAL(triggered(bool)),this,SLOT(aboutDeveloper(bool)));
    QMenuBar* menuBarHelp = new QMenuBar();
    menuBarHelp->addMenu(menuFile);
    menuBarHelp->addMenu(menu);
//    menuBarHelp->setWindowIconText(tr("help"));
    ui->mainToolBar->addWidget(menuBarHelp);

    connect(ui->fileBrowser,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onDoubleClicked(QModelIndex)));
    connect(ui->fileBrowser,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onBrowserRightClicked(QPoint)));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_model;
    delete m_saveModel;
}

void MainWindow::TreeClicked(const QModelIndex &index){
   QString path = m_model->filePath(index);
   m_FileModel->removeRows(0,m_FileModel->rowCount());
   getFileListPath(path);
}

void MainWindow::getFileListUnderDir(const QString &dirPath){
    QDir dir(dirPath);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);
    foreach (auto fileInfo, fileInfoList){
        if (fileInfo.isDir()) {
            getFileListUnderDir(fileInfo.absoluteFilePath());
        }
    }
}

QStringList MainWindow::getFileListPath(const QString &dirPath){
    QWidget::setWindowTitle(dirPath);
    QStringList output = {};
    searchThread->proceedSearchRequest(dirPath);
    searchThread->start();
    showSearchProceed();
    return output;
}

void MainWindow::TableViewInitial(){
    m_parentFileModel = new QStandardItemModel();
    m_parentFileModel->setHorizontalHeaderLabels(QStringList() << tr("file name") << tr("created time")
                                         << tr("file size") << tr("file path"));
    QStandardItem* parentItem = m_parentFileModel->invisibleRootItem();
    m_FileModel = parentItem;
    ui->fileBrowser->setModel(m_parentFileModel);
}

void MainWindow::onSearchFinished(const QList<myFileInfo*> info){
    haveReceived = true;
    m_plFileInfo = info;
    m_plShowInfo = m_plFileInfo;

    for (myFileInfo* exactInfo : info){
        QList<QStandardItem*> items_station;
        QStandardItem* item_station1 = new QStandardItem(exactInfo->m_fileName);
        items_station.append(item_station1);

        QStandardItem* item_station2 = new QStandardItem(exactInfo->m_fileCreatedTime);
        items_station.append(item_station2);

        QString back = " B";
        float fileSize = exactInfo->m_fileSize;
        if (fileSize >= 1024){
            fileSize /= 1024;
            back = " KB";
        }
        if (fileSize >= 1024){
            fileSize /= 1024;
            back = " MB";
        }
        if (fileSize >= 1024){
            fileSize /= 1024;
            back = " GB";
        }
        QStandardItem* item_station3 = new QStandardItem(QString::number(fileSize,'f',2) + back);
        items_station.append(item_station3);

        QStandardItem* item_station4 = new QStandardItem(exactInfo->m_filePath);
        items_station.append(item_station4);

        m_FileModel->appendRow(items_station);
    }
    if(m_progressdlg) {
        m_progressdlg->setValue(m_progressdlg->maximum());
        delete m_progressdlg;
        m_progressdlg = 0;
    }
    killTimer(m_hTimer);
    m_hTimer = 0;
}

void MainWindow::showSearchProceed(){
     haveReceived = false;
     if(m_progressdlg) delete m_progressdlg;
     m_progressdlg = new QProgressDialog(this);
     m_progressdlg->setWindowModality(Qt::WindowModal);
     m_progressdlg->setWindowTitle(tr("search progress"));
     m_progressdlg->setRange(0,101);
     m_progressdlg->setWindowIcon(QIcon(":/images/smallIcon.png"));
     m_progressdlg->show();  //显示对话框
     m_cancelBtn = new QPushButton();
     m_cancelBtn->setText("cancel");
     m_cancelBtn->hide();
     //cancelBtn->setGeometry(200,100,30,50);
     connect(m_cancelBtn,SIGNAL(clicked(bool)),this,SLOT(finishSearching(bool)));
     m_progressdlg->setCancelButton(m_cancelBtn);
     m_hTimer = startTimer(200);
}
void MainWindow::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == m_hTimer){
        if (haveReceived) return;
        if (m_progressdlg->value()<100) m_progressdlg->setValue(m_progressdlg->value()+1);
        m_progressdlg->setLabelText(m_currentSearchingFile);
    }
}

void MainWindow::onExportActivated(bool isClicked){
    m_savePath = "./files.csv";
    m_saveDialog = new QDialog();
    m_saveDialog->setWindowIcon(QIcon(":/images/smallIcon.png"));
    m_saveDialog->setFixedSize(1280,720);
    m_saveDialog->setWindowTitle(tr("choose save path"));
    m_saveModel = new QFileSystemModel();
    m_saveModel->setFilter(QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
    //m_model->setRootPath("D:/AndroidSDK");
    m_saveModel->setRootPath("/");
    QTreeView* saveTreeView = new QTreeView(m_saveDialog);
    saveTreeView->setModel(m_saveModel);
    saveTreeView->setFixedSize(1280,680);

    m_saveFilePathEdit = new QLineEdit(m_saveDialog);
    m_saveFilePathEdit->setText(tr("root path of save is:"));
    //saveLabel->setFixedSize(900,20);
    m_saveFilePathEdit->setGeometry(0,680,500,40);
    m_saveFilePathEdit->setEnabled(false);

    m_saveFileNameEdit = new QLineEdit(m_saveDialog);
    m_saveFileNameEdit->setText("");
    m_saveFileNameEdit->setGeometry(500,680,500,40);
    m_saveFileNameEdit->setToolTip(tr("please type file name"));

    QPushButton* saveBtn = new QPushButton(m_saveDialog);
    saveBtn->setText(tr("OK"));
    saveBtn->setGeometry(1000,680,280,40);

    m_saveDialog->show();
    connect(saveTreeView,SIGNAL(clicked(QModelIndex)),this,SLOT(onSavePathChanged(QModelIndex)));
    connect(saveBtn,SIGNAL(clicked(bool)),this,SLOT(onSaveConfirmed(bool)));
}

void MainWindow::searchTargetFiles(bool isClicked){
    QString targetFileName = ui->lineEdit->text();
    QList<myFileInfo*> infos = {};
    for (myFileInfo* info : m_plFileInfo){
        if (info->m_fileName.contains(targetFileName, Qt::CaseInsensitive)){
            infos.append(info);
        }
    }
    m_plShowInfo = infos;

    m_FileModel->removeRows(0,m_FileModel->rowCount());
    for (myFileInfo* exactInfo : m_plShowInfo){
        QList<QStandardItem*> items_station;
        QStandardItem* item_station1 = new QStandardItem(exactInfo->m_fileName);
        items_station.append(item_station1);

        QStandardItem* item_station2 = new QStandardItem(exactInfo->m_fileCreatedTime);
        items_station.append(item_station2);

        QString back = " B";
        float fileSize = exactInfo->m_fileSize;
        if (fileSize >= 1024){
            fileSize /= 1024;
            back = " KB";
        }
        if (fileSize >= 1024){
            fileSize /= 1024;
            back = " MB";
        }
        if (fileSize >= 1024){
            fileSize /= 1024;
            back = " GB";
        }
        QStandardItem* item_station3 = new QStandardItem(QString::number(fileSize,'f',2) + back);
        items_station.append(item_station3);

        QStandardItem* item_station4 = new QStandardItem(exactInfo->m_filePath);
        items_station.append(item_station4);

        m_FileModel->appendRow(items_station);
    }
}

void MainWindow::aboutDeveloper(bool isClicked){
    QMessageBox* notice = new QMessageBox();
    notice->setText(tr("this software is developed by XXX"));
    notice->exec();
}

void MainWindow::onDoubleClicked(const QModelIndex &index){
    QStandardItem* item = m_parentFileModel->item(index.row(), 3);
    QString path = item->text();
    QUrl url = QUrl::fromLocalFile(path);
    QDesktopServices::openUrl(url);
}

void MainWindow::onSavePathChanged(const QModelIndex &index){
    m_savePath = m_saveModel->filePath(index);
    m_saveFilePathEdit->setText(tr("root path of save is:") + m_savePath);
}

void MainWindow::onSaveConfirmed(bool isClicked){
    QString dtStr = m_savePath;
    QFile file;
    if (m_saveFileNameEdit->text().length() <= 0){
        file.setFileName(dtStr + "/searchResult.csv");
    }
    else{
        file.setFileName(dtStr + "/" + m_saveFileNameEdit->text() + ".csv");
    }

    if (file.open(QIODevice::WriteOnly)){
        QTextStream out(&file);
        out << tr("file name") << ",";
        out << tr("create time") << ",";
        out << tr("size") << ",";
        out << tr("file path") << ",";
        out << "\n";
        for (myFileInfo* info : m_plShowInfo){

            out << info->m_fileName << ",";
            out << info->m_fileCreatedTime << ",";

            QString back = " B";
            float fileSize = info->m_fileSize;
            if (fileSize >= 1024){
                fileSize /= 1024;
                back = " KB";
            }
            if (fileSize >= 1024){
                fileSize /= 1024;
                back = " MB";
            }
            if (fileSize >= 1024){
                fileSize /= 1024;
                back = " GB";
            }
            out << QString::number(fileSize,'f',2) + back << ",";
            out << info->m_filePath << ",";
            out << "\n";
        }
        file.close();

        m_saveDialog->close();
        QMessageBox* notice = new QMessageBox();
        notice->setWindowIcon(QIcon(":/images/smallIcon.png"));
        notice->setWindowTitle(tr("notice"));
        notice->setText(tr("excel export complete"));
        notice->exec();
    }
}

void MainWindow::onBrowserRightClicked(QPoint pos){
    auto index = ui->fileBrowser->indexAt(pos);
    if (index.isValid()){
        index = (QModelIndex) index;
        clickedPos = index;

        QStandardItem* item = m_parentFileModel->item(index.row(), 3);
        m_openFilePath = item->text();
        /*QUrl url = QUrl::fromLocalFile(path);
        QDesktopServices::openUrl(url);*/

        QMenu* menu = new QMenu(ui->fileBrowser);
        QAction* deleteAction = new QAction(tr("delete file"));
        connect(deleteAction,SIGNAL(triggered(bool)),this,SLOT(onBrowserDeleted(bool)));
        QAction* openFileAction = new QAction(tr("open file in dir"));
        connect(openFileAction,SIGNAL(triggered(bool)),this,SLOT(onBrowserOpen(bool)));
        menu->addAction(deleteAction);
        menu->addAction(openFileAction);
        menu->exec(QCursor::pos());
    }
}

void MainWindow::onBrowserDeleted(bool isClicked){
    QMessageBox message(QMessageBox::NoIcon,tr("warning"),tr("Are you sure to delete this file? File will be unable to restore after being deleted."),
                        QMessageBox::Yes | QMessageBox::No, NULL);
    message.setWindowIcon(QIcon(":/images/smallIcon.png"));
    if (message.exec() == QMessageBox::Yes){
        for (myFileInfo* info : m_plFileInfo){
            if (info->m_filePath == m_openFilePath){
                m_plFileInfo.removeAll(info);
                m_plShowInfo.removeAll(info);
                m_parentFileModel->removeRow(clickedPos.row());
                break;
            }
        }
        QFile::remove(m_openFilePath);
    }
}

void MainWindow::onBrowserOpen(bool isClicked){
    QString path = m_openFilePath.section('/',0,-2);
    QUrl url = QUrl::fromLocalFile(path);
    QDesktopServices::openUrl(url);
}

void MainWindow::onQuitTriggered(bool isClicked){
    QApplication::quit();
}

void MainWindow::onSearchFileChanged(QString fileName){
    m_currentSearchingFile = fileName;
}

void MainWindow::finishSearching(bool isClicked){
    killTimer(m_hTimer);
    m_progressdlg = 0;
    searchThread->terminate();
}
