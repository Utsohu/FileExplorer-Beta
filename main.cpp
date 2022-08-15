#include "mainwindow.h"
#include "languagebox.h"
#include <QApplication>
#include <QTextCodec>
#include <QPushButton>
#include <QSettings>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings s(  "./fileexplore.ini" , QSettings::IniFormat);
    s.beginGroup("Lan");
    QString language = s.value("zh", "").toString();


    if (language == ""){
        languageBox checkBox;
        int ret = checkBox.exec();
        if( ret == 3){//QDialog::Accepted) {
            language = "en";
            s.setValue("zh","en");
        }
        else if(ret == 4){//QDialog::Rejected){
            language = "cn";
            s.setValue("zh","cn");
        }
        else if(ret == 5){
            language = "fr";
            s.setValue("zh","fr");
        }
        else{
            exit(0);
        }
    }
    s.endGroup();
    MainWindow w(language);
    w.showMaximized();
    return a.exec();
}
