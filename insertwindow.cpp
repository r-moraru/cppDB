#include "insertwindow.h"
#include "ui_insertwindow.h"

#include "mainwindow.h"

InsertWindow::InsertWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertWindow),
    retry(false)
{
    ui->setupUi(this);

    connect(this, &InsertWindow::insertRow,
            qobject_cast<MainWindow *>(parent), &MainWindow::insertRow);

    connect(qobject_cast<MainWindow *>(parent), &MainWindow::retry,
            this, &InsertWindow::set_retry);
}

InsertWindow::~InsertWindow()
{
    delete ui;
}

void InsertWindow::on_okButton_clicked()
{
    QString new_row = ui->textEdit->toPlainText();
    retry = false;

    emit insertRow(new_row);

    if (!retry)
        close();
}


void InsertWindow::on_cancelButton_clicked()
{
    close();
}


void InsertWindow::set_retry() {
    retry = true;
}
