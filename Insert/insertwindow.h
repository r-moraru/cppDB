#ifndef INSERTWINDOW_H
#define INSERTWINDOW_H

#include <QDialog>

namespace Ui {
class InsertWindow;
}

class InsertWindow : public QDialog
{
    Q_OBJECT

public:
    explicit InsertWindow(QWidget *parent = nullptr);
    ~InsertWindow();

signals:
    void insertRow(QString row_string);

private slots:
    void set_retry();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::InsertWindow *ui;
    bool retry;
};

#endif // INSERTWINDOW_H
