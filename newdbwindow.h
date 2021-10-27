#ifndef NEWDBWINDOW_H
#define NEWDBWINDOW_H

#include <QDialog>

namespace Ui {
class newDBWindow;
}

class newDBWindow : public QDialog
{
    Q_OBJECT

public:
    explicit newDBWindow(QWidget *parent = nullptr);
    ~newDBWindow();

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::newDBWindow *ui;
};

#endif // NEWDBWINDOW_H
