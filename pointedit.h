#ifndef POINTEDIT_H
#define POINTEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QDialog>

namespace Ui {
class PointEdit;
}

class PointEdit : public QDialog {
    Q_OBJECT

public:
    typedef struct {
        QString Parcel;
        QString Description;
    } Point;

    explicit PointEdit(Point* point, QLineEdit* lineEdit, QWidget* parent = 0);
    ~PointEdit();

private slots:
    void on_spinBox_Parcel_valueChanged(int arg1);

private:
    Ui::PointEdit* ui;
    QLineEdit* lineEdit;
    Point* point;
    void Yes();
};

#endif // POINTEDIT_H
