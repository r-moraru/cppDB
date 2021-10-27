#include "updatewindow.h"
#include "ui_updatewindow.h"

#include "mainwindow.h"

UpdateWindow::UpdateWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateWindow)
{
    ui->setupUi(this);

    connect(this, &UpdateWindow::updateRow,
            qobject_cast<MainWindow *>(parent), &MainWindow::updateRow);

    connect(qobject_cast<MainWindow *>(parent), &MainWindow::retry,
            this, &UpdateWindow::set_retry);
}

UpdateWindow::~UpdateWindow()
{
    delete ui;
}

void UpdateWindow::on_okButton_clicked()
{
    QString search_column = ui->searchColumnLineEdit->text();
    QString search_value = ui->searchValueLineEdit->text();
    QString updated_column = ui->updatedColumnLineEdit->text();
    QString new_value = ui->newValueLineEdit->text();
    retry = false;

    emit updateRow(search_column, search_value,
                    updated_column, new_value);

    if (!retry)
        close();
}


void UpdateWindow::on_cancelButton_clicked()
{
    close();
}


void UpdateWindow::set_retry() {
    retry = true;
}
