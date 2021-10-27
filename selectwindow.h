#ifndef SELECTWINDOW_H
#define SELECTWINDOW_H

#include <QDialog>

namespace Ui {
class SelectWindow;
}

class SelectWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SelectWindow(QWidget *parent = nullptr);
    ~SelectWindow();

signals:
    void selectRows(QString, QString);

private slots:
    void set_retry();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::SelectWindow *ui;
    bool retry;
};

#endif // SELECTWINDOW_H
