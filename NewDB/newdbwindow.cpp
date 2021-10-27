#include "newdbwindow.h"
#include "../Main/mainwindow.h"
#include "ui_newdbwindow.h"

#include <QMessageBox>
#include <QDir>
#include <QFileSystemModel>

newDBWindow::newDBWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newDBWindow)
{
    ui->setupUi(this);
}

newDBWindow::~newDBWindow()
{
    delete ui;
}

void newDBWindow::on_okButton_clicked()
{
    QString new_db_name = ui->dbNameLineEdit->text();

    QString dirName = QDir::currentPath() + "/DBs/" + new_db_name;
    if (!QDir(dirName).exists()) {
        QDir().mkdir(dirName);
        close();
    }
}


void newDBWindow::on_cancelButton_clicked()
{
    close();
}

