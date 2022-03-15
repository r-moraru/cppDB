#ifndef EXPORTWINDOW_H
#define EXPORTWINDOW_H

#include <QDialog>

namespace Ui {
class ExportWindow;
}

class ExportWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ExportWindow(QWidget *parent = nullptr);
    ~ExportWindow();

signals:
    void exportCSV(QString, bool);

private slots:


    void on_doneButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::ExportWindow *ui;
};

#endif // EXPORTWINDOW_H
