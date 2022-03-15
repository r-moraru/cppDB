#ifndef EXPORT_H
#define EXPORT_H

#include <QDialog>

namespace Ui {
class export;
}

class export : public QDialog
{
    Q_OBJECT

public:
    explicit export(QWidget *parent = nullptr);
    ~export();

private:
    Ui::export *ui;
};

#endif // EXPORT_H
