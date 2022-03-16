#include "exportwindow.h"
#include "../Main/mainwindow.h"
#include "ui_exportwindow.h"
#include <QRegularExpression>

ExportWindow::ExportWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportWindow)
{
    ui->setupUi(this);

    connect(this, &ExportWindow::exportCSV,
            qobject_cast<MainWindow *>(parent), &MainWindow::exportCSV);
}

ExportWindow::~ExportWindow()
{
    delete ui;
}

void ExportWindow::on_doneButton_clicked()
{
    QString filename = ui->fileNameLineEdit->text();
    bool includeColName = ui->includeColNames->isChecked();

    QRegularExpression regexp("^([a-z]|[A-Z]|_|-|[0-9])+$");
    QRegularExpressionMatch regMatch = regexp.match(filename);

    if (regMatch.hasMatch()) {
        emit exportCSV(filename, includeColName);
        close();
    }
}


void ExportWindow::on_cancelButton_clicked()
{
    close();
}

