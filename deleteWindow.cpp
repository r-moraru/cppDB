#include "deletewindow.h"
#include "ui_deletewindow.h"

#include "mainwindow.h"

DeleteWindow::DeleteWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteWindow)
{
    ui->setupUi(this);

    connect(this, &DeleteWindow::deleteRow,
            qobject_cast<MainWindow *>(parent), &MainWindow::deleteRow);

    connect(qobject_cast<MainWindow *>(parent), &MainWindow::retry,
            this, &DeleteWindow::set_retry);
}

DeleteWindow::~DeleteWindow()
{
    delete ui;
}

void DeleteWindow::on_okButton_clicked()
{
    QString column_name = ui->columnNameLineEdit->text();
    QString value = ui->valueLineEdit->text();
    retry = false;

    emit deleteRow(column_name, value);

    if (!retry)
        close();
}


void DeleteWindow::on_cancelButton_clicked()
{
    close();
}


void DeleteWindow::set_retry() {
    retry = true;
}
