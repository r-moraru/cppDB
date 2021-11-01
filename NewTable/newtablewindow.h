#ifndef NEWTABLEWINDOW_H
#define NEWTABLEWINDOW_H

#include <QDialog>

namespace Ui {
class NewTableWindow;
}

class NewTableWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewTableWindow(QWidget *parent = nullptr);
    ~NewTableWindow();

signals:
    void readyToCreate(QVector<QString> cn, QVector<QString> ct,
                       QVector<int> cs, int pkp,
                       QString tn);

private slots:

    void on_addColumnButton_clicked();

    void on_doneButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::NewTableWindow *ui;
    QVector<QString> column_names;
    QVector<QString> column_types;
    QVector<int> column_sizes;
    QString table_name;
    int pk_pos;
};

#endif // NEWTABLEWINDOW_H
