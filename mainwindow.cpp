#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "insertwindow.h"
#include "selectwindow.h"
#include "updatewindow.h"
#include "deletewindow.h"
#include "newdbwindow.h"
#include "newtablewindow.h"

#include "table.h"

#include <QLabel>
#include <QMessageBox>
#include <QFileSystemModel>
#include <QDir>

#include <sstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , fileSystemModel(new QFileSystemModel)
{
    ui->setupUi(this);

    if (!QDir(QDir::currentPath() + "/DBs").exists()) {
        QDir().mkdir(QDir::currentPath() + "/DBs");
    }

    fileSystemModel->setRootPath(QDir::currentPath() + "/DBs");

    ui->dbListView->setModel(fileSystemModel);
    ui->dbListView->setRootIndex(fileSystemModel->index(QDir::currentPath() + "/DBs"));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fileSystemModel;
}


void MainWindow::on_pushButton_11_clicked()
{
    if (selectedDB == "" || selectedTable == "")
        return;
    InsertWindow insert(this);
    insert.setModal(true);
    insert.exec();
}


void MainWindow::on_selectButton_clicked()
{
    if (selectedDB == "" || selectedTable == "")
        return;
    SelectWindow select(this);
    select.setModal(true);
    select.exec();
}


void MainWindow::on_updateButton_clicked()
{
    if (selectedDB == "" || selectedTable == "")
        return;
    UpdateWindow update(this);
    update.setModal(true);
    update.exec();
}


void MainWindow::on_deleteButton_clicked()
{
    if (selectedDB == "" || selectedTable == "")
        return;
    DeleteWindow del(this);
    del.setModal(true);
    del.exec();
}


void MainWindow::on_newDBButton_clicked()
{
    newDBWindow window(this);
    window.setModal(true);
    window.exec();
}


void MainWindow::on_deleteDBButton_clicked()
{
    auto index = ui->dbListView->currentIndex();
    QString selected_db_name = index.data(Qt::DisplayRole).toString();

    if (selected_db_name == "") return;

    QString db_path = QDir::currentPath() + "/DBs/" + selected_db_name;
    QDir().rmdir(db_path);
}


void MainWindow::on_selectDBButton_clicked()
{
    auto index = ui->dbListView->currentIndex();
    QString selected_db_name = index.data(Qt::DisplayRole).toString();

    if (selected_db_name == "") return;

    selectedDB = selected_db_name;
    selectedTable = "";

    QString db_path = QDir::currentPath() + "/DBs/" + selected_db_name;

    ui->dbView->setModel(fileSystemModel);
    ui->dbView->setRootIndex(fileSystemModel->index(db_path));

    // open DB view tab
    ui->tabWidget->setCurrentIndex(1);
}


void MainWindow::on_newTableButton_clicked()
{
    if (selectedDB == "") return;

    NewTableWindow newTableWindow(this);
    newTableWindow.setModal(true);
    newTableWindow.exec();
}

void MainWindow::mw_create_table(QVector<QString>& column_names,
                              QVector<QString>& column_types,
                              QVector<int>& column_sizes, int pk_pos,
                              QString table_name)
{
    std::stringstream communication_stream;
    communication_stream << (pk_pos != -1 ? 1 : 0) << " ";
    communication_stream << pk_pos;
    for (int i = 0; i < column_names.size(); i++) {
        communication_stream << column_names[i].toStdString() << " ";
        communication_stream << column_types[i].toStdString();
        if (column_types[i] == "char") {
            communication_stream << "(" << column_sizes[i] << ")";
        }

        if (i == column_names.size()-1)
            communication_stream << ";";
        communication_stream << " ";
    }

    create_table((selectedDB + "/" + table_name).toStdString(),
                 communication_stream);

    QString path = QDir::currentPath() + "/DBs/" + selectedDB + "/" +
                                                   selectedTable;
    QMessageBox ms;
        ms.setText(path);
        ms.exec();

    std::string table_path = path.toStdString();
    Table table(table_path);
    table.add_root_node();
}


void MainWindow::on_pushButton_2_clicked()
{
    auto index = ui->dbView->currentIndex();
    QString selected_table_name = index.data(Qt::DisplayRole).toString();

    if (selected_table_name == "") return;

    QString path = QDir::currentPath() + "/DBs/" + selectedDB + "/" +
                                                   selected_table_name;
    QFile(path).remove();
}


void MainWindow::on_selectTableButton_clicked()
{
    auto index = ui->dbView->currentIndex();
    QString selected_table_name = index.data(Qt::DisplayRole).toString();

    selectedTable = selected_table_name;

    ui->tabWidget->setCurrentIndex(2);
    ui->QueryResults->setRowCount(0);
    ui->QueryResults->setColumnCount(0);
}


void MainWindow::on_viewTableDataButton_clicked()
{
    if (selectedDB == "" || selectedTable == "")    return;
    QString path = QDir::currentPath() + "/DBs/" + selectedDB + "/" +
                                                   selectedTable;
    std::string table_path = path.toStdString();
    Table table(table_path);

    int primary_key_pos, row_count;
    primary_key_pos = table.get_primary_key_pos();
    row_count = table.get_column_count();

    QVector<QString> column_names, column_types, column_sizes;

    column_names = table.get_column_names();
    column_types = table.get_column_types();
    column_sizes = table.get_column_sizes();

    ui->QueryResults->setColumnCount(4);
    ui->QueryResults->setRowCount(row_count);

    QStringList labels = {"Is Primary Key", "Column Name", "Column Type", "Column Size"};
    ui->QueryResults->setHorizontalHeaderLabels(labels);

    ui->QueryResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int i = 0; i < row_count; i++) {
        ui->QueryResults->setCellWidget(i, 0, new QLabel);
        QLabel  *is_pk = qobject_cast<QLabel *>(ui->QueryResults->
                                                cellWidget(i, 0));
        is_pk->setAlignment(Qt::AlignHCenter);

        ui->QueryResults->setCellWidget(i, 1, new QLabel);
        QLabel *column_name_label = qobject_cast<QLabel *>(ui->QueryResults->
                                               cellWidget(i, 1));
        column_name_label->setAlignment(Qt::AlignHCenter);

        ui->QueryResults->setCellWidget(i, 2, new QLabel);
        QLabel *column_type_label = qobject_cast<QLabel *>(ui->QueryResults->
                                               cellWidget(i, 2));
        column_type_label->setAlignment(Qt::AlignHCenter);

        ui->QueryResults->setCellWidget(i, 3, new QLabel);
        QLabel *column_size_label = qobject_cast<QLabel *>(ui->QueryResults->
                                               cellWidget(i, 3));
        column_size_label->setAlignment(Qt::AlignHCenter);

        if (primary_key_pos == i)
            is_pk->setText("true");
        else
            is_pk->setText("false");

        QString column_name = column_names[i];
        column_name_label->setText(column_name);

        QString column_type = column_types[i];

        if (column_type == "i") column_type = "int";
        else if (column_type == "c") column_type = "char";
        else if (column_type == "f") column_type = "float";

        column_type_label->setText(column_type);

        QString column_size = column_sizes[i];
        column_size_label->setText(column_size);
    }
}


void MainWindow::insertRow(QString new_string) {
    std::stringstream ss(new_string.toStdString());

    QString path = QDir::currentPath() + "/DBs/" + selectedDB + "/" +
                                                   selectedTable;
    std::string table_path = path.toStdString();
    Table table(table_path);

    bool is_valid = true;

    QVector<QString> column_types = table.get_column_types();

    for (int i = 0; i < column_types.size(); i++) {
        std::string word;
        ss >> word;
        std::stringstream word_ss(word);

        if (column_types[i] == "i") {
            int v;
            if (!(word_ss >> v)) {
                is_valid = false;
                QMessageBox mb;
                mb.setWindowTitle("Error");
                mb.setText("Wrong value at pos " + QString::number(i));
                mb.exec();
                break;
            }
            else if (!word_ss.eof()) {
                is_valid = false;
                QMessageBox mb;
                mb.setWindowTitle("Error");
                mb.setText("Wrong value at pos " + QString::number(i));
                mb.exec();
                break;
            }
        }
        else if (column_types[i] == "f") {
            float val;
            if (!(word_ss >> val)) {
                is_valid = false;
                QMessageBox mb;
                mb.setWindowTitle("Error");
                mb.setText("Wrong value at pos " + QString::number(i));
                mb.exec();
                break;
            }
        }
        else if (column_types[i] == "c") {
            std::string val;
            word_ss >> val;
        }
    }

    if (is_valid && !ss.eof()) {
        is_valid = false;
        QMessageBox mb;
        mb.setWindowTitle("Error");
        mb.setText("Too many values");
        mb.exec();
    }

    if (!is_valid) {
        emit retry();
    }
    else {
        ss = std::stringstream(new_string.toStdString());
        table.insert_row(ss);
    }
}


void MainWindow::selectRows(QString column_name, QString value) {
    QString path = QDir::currentPath() + "/DBs/" + selectedDB + "/" +
                                                   selectedTable;
    std::string table_path = path.toStdString();
    Table table(table_path);

    bool valid = false;

    QVector<QString> column_names = table.get_column_names();
    QVector<QString> column_types = table.get_column_types();

    int i;
    for (i = 0; i < column_names.size(); i++) {
        if (column_names[i] == column_name) {
            valid = true;
            break;
        }
    }

    if (!valid) {
        QMessageBox mb;
        mb.setWindowTitle("Error");
        mb.setText("Requested column does not exist in table");
        mb.exec();

        emit retry();

        return;
    }

    std::stringstream test_stream(value.toStdString());
    if (column_types[i] == "i") {
        int val;
        if (!(test_stream >> val) || !(test_stream.eof()))
            valid = false;
    }
    else if (column_types[i] == "f") {
        float val;
        if (!(test_stream >> val) || !(test_stream.eof()))
            valid = false;
    }
    else if (column_types[i] == "c") {
        std::string val;
        if (!(test_stream >> val) || !(test_stream.eof()))
            valid = false;
    }

    if (!valid) {
        QMessageBox mb;
        mb.setWindowTitle("Error");
        mb.setText("Invalid value");
        mb.exec();

        emit retry();

        return;
    }

    std::stringstream answer;
    std::stringstream input;

    input << column_name.toStdString() << " " << value.toStdString();
    table.select_rows(answer, input);

    answer.seekg(0, std::ios::beg);

    QStringList columnNames;
    for (auto & col_name : column_names)
        columnNames.append(col_name);

    ui->QueryResults->setColumnCount(column_names.size());
    ui->QueryResults->setRowCount(0);
    ui->QueryResults->setHorizontalHeaderLabels(columnNames);
    ui->QueryResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int col_num = column_names.size();
    int row_num = -1;
    QString result;
    std::string str;

    while (answer >> str) {
        QMessageBox ms;
        ms.setText("Am ajus aici.");
        ms.exec();

        if (col_num == column_names.size()) {
            col_num = 0;
            row_num++;
            ui->QueryResults->insertRow(row_num);
        }

        result = QString::fromStdString(str);

        ui->QueryResults->setCellWidget(row_num, col_num, new QLabel);

        QLabel *cellLabel = qobject_cast<QLabel *>(ui->QueryResults->
                                                   cellWidget(row_num, col_num));

        cellLabel->setText(result);

        col_num++;
    }
}

void MainWindow::deleteRow(QString column_name, QString value) {

}


void MainWindow::updateRow(QString searched_name, QString searched_value,
                           QString updated_name, QString new_value) {

}
