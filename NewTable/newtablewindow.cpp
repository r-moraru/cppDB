#include "newtablewindow.h"
#include "ui_newtablewindow.h"
#include "../Table/table.h"

#include "../Main/mainwindow.h"
#include <QMessageBox>
#include <sstream>

NewTableWindow::NewTableWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTableWindow),
    pk_pos(-1)
{
    ui->setupUi(this);
    connect(ui->charRadioButton, &QAbstractButton::toggled,
            ui->stringSizeLineEdit, &QWidget::setVisible);

    ui->stringSizeLabel->setVisible(false);
    ui->stringSizeLineEdit->setVisible(false);

    connect(ui->charRadioButton, &QAbstractButton::toggled,
            ui->stringSizeLabel, &QWidget::setVisible);

    connect(this, &NewTableWindow::readyToCreate,
            qobject_cast<MainWindow *>(parent), &MainWindow::mw_create_table);
}

NewTableWindow::~NewTableWindow()
{
    delete ui;
}

void NewTableWindow::on_addColumnButton_clicked()
{
    QString new_column = ui->columnNameLineEdit->text();
    for (auto & column_name : column_names) {
        if (column_name == new_column) {
            QMessageBox ms;
            ms.setText("Column with name " + new_column + " already exists");
            ms.exec();
            return;
        }
    }

    // TODO: add dialog box
    if (pk_pos != -1 && ui->isPKCheckBox->isChecked()) {
        QMessageBox ms;
        ms.setText("Primary key has already been assigned to column " +
                   QString::number(pk_pos));
        ms.exec();
    }

    column_names.push_back(new_column);
    if (ui->intRadioButton->isChecked()) {
        column_types.push_back("Integer");
        column_sizes.push_back(sizeof(int));
    }
    else if (ui->FloatRadioButton->isChecked()){
        column_types.push_back("Float");
        column_sizes.push_back(sizeof(float));
    }
    else if (ui->charRadioButton->isChecked()) {
        column_types.push_back("String");
        // TODO: check if line edits contain valid values
        column_sizes.push_back(ui->stringSizeLineEdit->text().toInt());
    }
    else {
        QMessageBox ms;
        ms.setText("No data type seleceted");
        ms.exec();
        return;
    }

    if (ui->isPKCheckBox->isChecked())
        pk_pos = column_names.size()-1;

    ui->charRadioButton->setChecked(false);
    ui->intRadioButton->setChecked(false);
    ui->FloatRadioButton->setChecked(false);

    ui->stringSizeLineEdit->setText("");
    ui->columnNameLineEdit->setText("");
}


void NewTableWindow::on_doneButton_clicked()
{
    if (!column_names.empty() && ui->tableNameLineEdit->text() != "") {
        emit readyToCreate(column_names, column_types, column_sizes, pk_pos,
                           ui->tableNameLineEdit->text());
        close();
    }
}


void NewTableWindow::on_cancelButton_clicked()
{
    close();
}

