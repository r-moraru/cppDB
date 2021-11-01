#ifndef COLUMNNAMESWINDOW_H
#define COLUMNNAMESWINDOW_H

#include <QDialog>

namespace Ui {
class ColumnNamesWindow;
}

class ColumnNamesWindow : public QDialog
{
    Q_OBJECT

public:
    ColumnNamesWindow(QVector<QString> column_names, QWidget *parent = nullptr);
    ~ColumnNamesWindow();

signals:
    void createTable(QVector<QString>, QVector<QString>,
                     QVector<int>,
                     int, QString);
private:
    Ui::ColumnNamesWindow *ui;

private slots:
    void changedColumnType(int i);
    void on_doneButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // COLUMNNAMESWINDOW_H
