#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../Insert/insertwindow.h"
#include "../Select/selectwindow.h"
#include "../Update/updatewindow.h"
#include "../Delete/deleteWindow.h"
#include "../NewDB/newdbwindow.h"
#include "../NewTable/newtablewindow.h"
#include "../ImportCSV/columnnameswindow.h"
#include "../ExportCSV/exportwindow.h"

#include "../Table/table.h"

#include <QLabel>
#include <QMessageBox>
#include <QFileSystemModel>
#include <QFileDialog>
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
    if (!QDir().rmdir(db_path)) {
        QMessageBox ms;
        ms.setWindowTitle("Error");
        ms.setText("Database must be empty in order to delete it.");
        ms.exec();
    }

    if (selectedDB == selected_db_name) {
        selectedDB = "";
    }
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

void MainWindow::mw_create_table(QVector<QString> column_names,
                              QVector<QString> column_types,
                              QVector<int> column_sizes, int pk_pos,
                              QString table_name)
{
    std::stringstream communication_stream;
    communication_stream << (pk_pos != -1 ? 1 : 0) << " ";
    if (pk_pos != -1)
        communication_stream << pk_pos << " ";
    for (int i = 0; i < column_names.size(); i++) {
        communication_stream << column_names[i].toStdString() << " ";
        if (column_types[i] == "Integer")
            communication_stream << "int";
        else if (column_types[i] == "Float")
            communication_stream << "float";
        else if (column_types[i] == "String") {
            communication_stream << "char(" << column_sizes[i] << ")";
        }

        if (i == column_names.size()-1)
            communication_stream << ";";
        communication_stream << " ";
    }

    create_table((selectedDB + "/" + table_name).toStdString(),
                 communication_stream);

    selectedTable = table_name + ".tab";
    QString path = QDir::currentPath() + "/DBs/" + selectedDB + "/" +
                                                   selectedTable;
    if (pk_pos == -1) return;
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

    if (selectedTable == selected_table_name)
        selectedTable = "";
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


bool checkValue(QString value, QString column_type) {
    bool valid = true;
    std::stringstream test_stream(value.toStdString());

    if (column_type == "i") {
        int val;
        if (!(test_stream >> val) || !(test_stream.eof()))
            valid = false;
    }
    else if (column_type == "f") {
        float val;
        if (!(test_stream >> val) || !(test_stream.eof()))
            valid = false;
    }
    else if (column_type == "c") {
        std::string val;
        if (!(test_stream >> val) || !(test_stream.eof()))
            valid = false;
    }

    return valid;
}


void MainWindow::insertRow(QString new_string) {
    std::stringstream ss(new_string.toStdString());

    QString path = QDir::currentPath() + "/DBs/" + selectedDB + "/" +
                                                   selectedTable;
    std::clog << "Inserting in file " << path.toStdString() << std::endl;
    std::string table_path = path.toStdString();
    Table table(table_path);

    bool is_valid = true;

    QVector<QString> column_types = table.get_column_types();

    for (int i = 0; i < column_types.size(); i++) {
        std::string word;
        ss >> word;

        if (!checkValue(QString::fromStdString(word), column_types[i])) {
            is_valid = false;
            QMessageBox mb;
            mb.setWindowTitle("Error");
            mb.setText("Wrong value at pos " + QString::number(i));
            mb.exec();
            break;
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


int getColumnPos(QVector<QString> column_names, QString column_name) {
    int i;
    for (i = 0; i < column_names.size(); i++) {
        if (column_names[i] == column_name) {
            return i;
        }
    }

    return -1;
}


bool checkColumnValuePair(QString column_name, QString value,
                          QVector<QString> column_names,
                          QVector<QString> column_types) {
    int i = getColumnPos(column_names, column_name);

    if (i == -1) {
        QMessageBox mb;
        mb.setWindowTitle("Error");
        mb.setText("Requested column does not exist in table");
        mb.exec();

        return false;
    }

    if (!checkValue(value, column_types[i])) {
        QMessageBox mb;
        mb.setWindowTitle("Error");
        mb.setText("Invalid value");
        mb.exec();

        return false;
    }

    return true;
}


void MainWindow::selectRows(QString column_name, QString value) {
    QString path = QDir::currentPath() + "/DBs/" + selectedDB + "/" +
                                                   selectedTable;
    std::string table_path = path.toStdString();
    Table table(table_path);

    QVector<QString> column_names = table.get_column_names();
    QVector<QString> column_types = table.get_column_types();

    bool valid = checkColumnValuePair(column_name, value, column_names, column_types);

    if (!valid) {
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
        cellLabel->setAlignment(Qt::AlignHCenter);

        col_num++;
    }
}

void MainWindow::deleteRow(QString column_name, QString value) {
    QString path = QDir::currentPath() + "/DBs/" + selectedDB + "/" +
                                                   selectedTable;
    std::string table_path = path.toStdString();
    Table table(table_path);

    QVector<QString> column_names = table.get_column_names();
    QVector<QString> column_types = table.get_column_types();

    bool valid = checkColumnValuePair(column_name, value, column_names, column_types);

    if (!valid) {
        emit retry();
        return;
    }

    std::stringstream input;

    input << column_name.toStdString() << " " << value.toStdString();

    table.delete_rows(input);
}


void MainWindow::updateRow(QString searched_name, QString searched_value,
                           QString updated_name, QString new_value) {
    QString path = QDir::currentPath() + "/DBs/" + selectedDB + "/" +
                                                   selectedTable;
    std::string table_path = path.toStdString();
    Table table(table_path);

    QVector<QString> column_names = table.get_column_names();
    QVector<QString> column_types = table.get_column_types();

    bool valid = checkColumnValuePair(searched_name, searched_value,
                                      column_names, column_types);

    valid &= checkColumnValuePair(updated_name, new_value,
                                  column_names, column_types);

    if (!valid) {
        emit retry();
        return;
    }

    std::stringstream input;

    input << searched_name.toStdString() <<
             " " << searched_value.toStdString() <<
             " ";

    input << updated_name.toStdString() <<
             " " << new_value.toStdString();

    table.update_rows(input);
}


void MainWindow::on_createFromFile_clicked()
{
    if (selectedDB == "")
        return;
    QString file_name = QFileDialog::getOpenFileName(this, "Open File", QString(),
                                                    "csv(*.csv)");

    QFile file(file_name);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    if (file.size() == 0) {
        QMessageBox ms;
        ms.setWindowTitle("Error");
        ms.setText("Empty file.");
        ms.exec();
    }
    else {
        QString line;
        QVector<QString> column_names;
        bool try_again = true;
        while (!file.atEnd() && try_again) {
            try_again = false;
            line = file.readLine().trimmed();
            column_names = line.split(",");
            if (column_names.size() == 0)   try_again = true;
        }

        if (column_names.size() == 0) {
            // TODO: invalid file
            return;
        }

        ColumnNamesWindow cnWindow(column_names, this);
        cnWindow.setModal(true);
        if (cnWindow.exec()) {
            while (!file.atEnd()) {
                line = file.readLine().trimmed();
                line.replace(',', ' ');
                insertRow(line);
            }
        }
    }
}


void MainWindow::on_insertFromFile_clicked()
{
    if (selectedDB == "" || selectedTable == "")
        return;
    QString file_name = QFileDialog::getOpenFileName(this, "Open File", QString(),
                                                    "csv(*.csv)");

    QFile file(file_name);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QString line;
    while (!file.atEnd()) {
        line = file.readLine().trimmed();
        line.replace(',', ' ');
        insertRow(line);
    }
}


void MainWindow::on_pushButton_clicked()
{
    if (selectedDB == "" || selectedTable == "")
        return;
    ExportWindow exportWindow(this);
    exportWindow.setModal(true);
    exportWindow.exec();
}

void MainWindow::exportCSV(QString filename, bool includeColName) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select a directory"),
                                                    "/",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    QString fullname = dir + "/" + filename + ".csv";
    QFile file(fullname);
    file.open(QIODevice::WriteOnly);

    QString path = QDir::currentPath() + "/DBs/" + selectedDB + "/" +
                                                   selectedTable;
    std::string table_path = path.toStdString();
    Table table(table_path);

    QVector<QString> column_names = table.get_column_names();

    if (includeColName) {
        for (qsizetype i = 0; i < column_names.size(); i++) {
            QString column = column_names[i];
            file.write(column.toLocal8Bit().data());
            if (i < column_names.size()-1) file.write(",");
        }
        file.write("\n");
    }

    std::stringstream rows;
    table.select_all_rows(rows);
    rows.seekg(0, std::ios::beg);

    std::string line;
    while (rows >> line) {
        file.write(line.c_str());
        file.write("\n");
    }
}

