#include "selectwindow.h"
#include "ui_selectwindow.h"

#include "mainwindow.h"

SelectWindow::SelectWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectWindow)
{
    ui->setupUi(this);

    connect(this, &SelectWindow::selectRows,
            qobject_cast<MainWindow *>(parent), &MainWindow::selectRows);

    connect(qobject_cast<MainWindow *>(parent), &MainWindow::retry,
            this, &SelectWindow::set_retry);
}


SelectWindow::~SelectWindow()
{
    delete ui;
}


void SelectWindow::on_okButton_clicked()
{
    QString column_name = ui->columnNameLineEdit->text();
    QString value = ui->valueLineEdit->text();
    retry = false;

    emit selectRows(column_name, value);

    if (!retry)
        close();
}


void SelectWindow::on_cancelButton_clicked()
{
    close();
}


void SelectWindow::set_retry() {
    retry = true;
}
