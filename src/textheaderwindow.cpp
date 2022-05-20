#include "textheaderwindow.h"
#include "ui_textheaderwindow.h"

TextHeaderWindow::TextHeaderWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextHeaderWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose, false);
}

TextHeaderWindow::~TextHeaderWindow()
{
    delete ui;
}

void TextHeaderWindow::setTextHeader(QString text_header){
    ui->textheader_out_textline->setText(text_header);
}

void TextHeaderWindow::on_ebcdicF_btn_clicked()
{
    emit changeFormat("EBCDIC");
}


void TextHeaderWindow::on_asciiF_btn_clicked()
{
    emit changeFormat("ASCII");
}

