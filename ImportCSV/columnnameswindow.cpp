#include "columnnameswindow.h"
#include "ui_columnnameswindow.h"

#include "../Main/mainwindow.h"
#include <QMessageBox>
#include <QComboBox>
#include <QSpinBox>

ColumnNamesWindow::ColumnNamesWindow(QVector<QString> column_names,
                                     QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColumnNamesWindow)
{
    ui->setupUi(this);

    connect(this, &ColumnNamesWindow::createTable,
            qobject_cast<MainWindow *>(parent), &MainWindow::mw_create_table);

    ui->columnData->setColumnCount(column_names.size());
    ui->columnData->setHorizontalHeaderLabels(column_names);
    ui->columnData->insertRow(0);
    ui->columnData->insertRow(1);

    ui->columnData->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int i = 0; i < column_names.size(); i++) {
        ui->columnData->setCellWidget(0, i, new QComboBox);
        QStringList items = { "Integer", "Float", "String" };
        QComboBox *qcb = qobject_cast<QComboBox *>(ui->columnData->cellWidget(0, i));

        qcb->addItems(items);

        ui->columnData->setCellWidget(1, i, new QSpinBox);

        connect(qcb, &QComboBox::currentIndexChanged,
                this, &ColumnNamesWindow::changedColumnType);
    }

    for (int i = 0; i < column_names.size(); i++)
        ui->columnData->cellWidget(1, i)->hide();

    ui->primaryKeySpinBox->setMinimum(0);
    ui->primaryKeySpinBox->setMaximum(column_names.size()-1);
}

void ColumnNamesWindow::changedColumnType(int i) {
    QSpinBox *columnSize =
            qobject_cast<QSpinBox *>(ui->
                                     columnData->
                                     cellWidget(1, ui->columnData->
                                                   currentColumn()));
    if (i == 2)
        columnSize->show();
    else
        columnSize->hide();
}

ColumnNamesWindow::~ColumnNamesWindow()
{
    delete ui;
}

void ColumnNamesWindow::on_doneButton_clicked()
{
    QString table_name = ui->tableNameLineEdit->text();
    // TODO: check table_name - not empty, no whitespace
    int primary_key_pos = -1;
    if (ui->primaryKeyCheckBox->isChecked())
        primary_key_pos = ui->primaryKeySpinBox->value();

    QVector<QString> column_names;
    QVector<QString> column_types;
    QVector<int> column_sizes;
    for (int i = 0; i < ui->columnData->columnCount(); i++) {
        column_names.push_back(ui->columnData->
                               horizontalHeaderItem(i)->text());

        QComboBox *type = qobject_cast<QComboBox *>(ui->columnData->
                                                    cellWidget(0, i));
        QSpinBox *size = qobject_cast<QSpinBox *>(ui->columnData->
                                                  cellWidget(1, i));

        column_types.push_back(type->currentText());
        column_sizes.push_back(size->value());
    }

    emit createTable(column_names, column_types, column_sizes,
                     primary_key_pos, table_name);

    accept();
}


void ColumnNamesWindow::on_pushButton_2_clicked()
{
    reject();
}

