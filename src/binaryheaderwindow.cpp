#include "binaryheaderwindow.h"
#include "ui_binaryheaderwindow.h"

BinaryHeaderWindow::BinaryHeaderWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::BinaryHeaderWindow) {
  ui->setupUi(this);
  this->setAttribute(Qt::WA_QuitOnClose, false);
}

BinaryHeaderWindow::~BinaryHeaderWindow() { delete ui; }

void BinaryHeaderWindow::setBinaryHeader(QString binary_header) {
  ui->textEdit->setText(binary_header);
}
