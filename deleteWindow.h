#ifndef DELETEWINDOW_H
#define DELETEWINDOW_H

#include <QDialog>

namespace Ui {
class DeleteWindow;
}

class DeleteWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteWindow(QWidget *parent = nullptr);
    ~DeleteWindow();

signals:
    void deleteRow(QString, QString);

private slots:
    void set_retry();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::DeleteWindow *ui;
    bool retry;
};

#endif // DELETEWINDOW_H
