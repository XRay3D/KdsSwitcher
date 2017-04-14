#include "pointedit.h"
#include "ui_pointedit.h"

#include <QDebug>

PointEdit::PointEdit(Point* point, QLineEdit* lineEdit, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::PointEdit)
    , lineEdit(lineEdit)
    , point(point)

{
    ui->setupUi(this);
    ui->spinBox_Parcel->setValue(point->Parcel.toInt());
    ui->lineEdit_Description->setText(point->Description);
    connect(ui->pushButton_Yes, &QPushButton::clicked, this, &PointEdit::Yes);
}

PointEdit::~PointEdit()
{
    delete ui;
}

void PointEdit::Yes()
{
    if (ui->lineEdit_Description->text().isEmpty()) {
        ui->lineEdit_Description->setText(QString().setNum(ui->spinBox_Parcel->value()));
    }
    lineEdit->setText(ui->lineEdit_Description->text());
    point->Description = ui->lineEdit_Description->text();
    point->Parcel = QString().setNum(ui->spinBox_Parcel->value());
    hide();
}

void PointEdit::on_spinBox_Parcel_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    //    if (ui->lineEdit_Description->text().isEmpty()
    //        || ui->lineEdit_Description->text().toInt() == (arg1 - 1)
    //        || ui->lineEdit_Description->text().toInt() == (arg1 + 1)) {
    //        ui->lineEdit_Description->setText(QString().setNum(arg1));
    //    }
}
