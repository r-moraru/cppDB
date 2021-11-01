#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileSystemModel>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    friend class DeleteWindow;
    friend class InsertWindow;
    friend class SelectWindow;
    friend class UpdateWindow;

    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getSelectedDB() { return selectedDB; }
    void setSelectedDB(QString str) { selectedDB = str; }

    QString getSelectedTable() { return selectedTable; }
    void setSelectedTable(QString str) { selectedTable = str; }

signals:
    void retry();

public slots:
    void mw_create_table(QVector<QString>, QVector<QString>, QVector<int>,
                         int, QString);

    void insertRow(QString row);

    void selectRows(QString column_name, QString value);

    void deleteRow(QString column_name, QString value);

    void updateRow(QString, QString, QString, QString);

private slots:
    void on_pushButton_11_clicked();

    void on_selectButton_clicked();

    void on_updateButton_clicked();

    void on_deleteButton_clicked();

    void on_newDBButton_clicked();

    void on_deleteDBButton_clicked();

    void on_selectDBButton_clicked();

    void on_newTableButton_clicked();

    void on_pushButton_2_clicked();

    void on_selectTableButton_clicked();

    void on_viewTableDataButton_clicked();

    void on_createFromFile_clicked();

    void on_insertFromFile_clicked();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *fileSystemModel;
    QString selectedDB;
    QString selectedTable;
};
#endif // MAINWINDOW_H
