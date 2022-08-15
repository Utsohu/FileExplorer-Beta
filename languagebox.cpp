#include "languagebox.h"
#include "ui_languagebox.h"
#include "mainwindow.h"

languageBox::languageBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::languageBox)
{
    ui->setupUi(this);

    setModal(true);
    connect(ui->enBtn,SIGNAL(clicked(bool)),this,SLOT(onEnClicked(bool)));
    connect(ui->cnBtn,SIGNAL(clicked(bool)),this,SLOT(onCnClicked(bool)));
    connect(ui->frBtn,SIGNAL(clicked(bool)),this,SLOT(onFrClicked(bool)));
}

languageBox::~languageBox()
{
    delete ui;
}

void languageBox::onEnClicked(bool isClicked){
    done(3);
}

void languageBox::onCnClicked(bool isClicked){
    done(4);
}

void languageBox::onFrClicked(bool isClicked){
    done(5);
}
