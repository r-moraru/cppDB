#ifndef UPDATEWINDOW_H
#define UPDATEWINDOW_H

#include <QDialog>

namespace Ui {
class UpdateWindow;
}

class UpdateWindow : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateWindow(QWidget *parent = nullptr);
    ~UpdateWindow();

signals:
    void updateRow(QString, QString, QString, QString);

private slots:
    void set_retry();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::UpdateWindow *ui;
    bool retry;
};

#endif // UPDATEWINDOW_H
