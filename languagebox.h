#ifndef LANGUAGEBOX_H
#define LANGUAGEBOX_H

#include <QDialog>

namespace Ui {
class languageBox;
}

class languageBox : public QDialog
{
    Q_OBJECT

public:
    explicit languageBox(QWidget *parent = nullptr);
    ~languageBox();

private:
    Ui::languageBox *ui;
private slots:
    void onEnClicked(bool isClicked);
    void onCnClicked(bool isClicked);
    void onFrClicked(bool isClicked);
};

#endif // LANGUAGEBOX_H
