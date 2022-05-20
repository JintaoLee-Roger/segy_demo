#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose, true);
    connect(this->text_header_window,
                     SIGNAL(changeFormat(QString)),
                     this,
                     SLOT(change_text_header(QString)));

    connect(this->trace_header_window,
            SIGNAL(changeText(QString)),
            this,
            SLOT(change_trace_header(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openfile_btn_clicked()
{
    QString segy_name = QFileDialog::getOpenFileName(
                            this,
                            "Open Segy File",
                            "../segy_demo/",
                            "Segy File (*.segy *.sgy *.Segy *.Sgy)");

    if (! segy_name.isEmpty()) {
        ui->segyname_line->setText(segy_name);
        segyF.open_file(segy_name.toLatin1().data());
        is_open_segy_ = true;

        ui->inline_loc_comb->setCurrentText(segyF.get_default_inline_loc());
        ui->xline_loc_comb->setCurrentText(segyF.get_default_xline_loc());
    }
}


void MainWindow::on_textheader_wind_btn_clicked()
{
    if (is_open_segy_) {
        text_header_window->setTextHeader(segyF.textHeader_default());
    }
    if (! text_header_window->isVisible()){
        text_header_window->show();
    }
}

void MainWindow::change_text_header(QString formt) {
    if (is_open_segy_) {
        if (formt == "EBCDIC") {
            text_header_window->setTextHeader(segyF.textHeader_EBCDIC());
        }
        else {
            text_header_window->setTextHeader(segyF.textHeader_ASCII());
        }
    }
}

void MainWindow::on_binheader_wind_btn_clicked()
{
    if (is_open_segy_) {
        binary_header_window->setBinaryHeader(segyF.binaryHeader_QString());
    }
    if (! binary_header_window->isVisible()){
        binary_header_window->show();
    }
}


void MainWindow::on_traceheader_wind_btn_clicked()
{
    trace_header_window->set_trace_num(ui->trace_num_line->text());
    if (is_open_segy_) {
        int64_t t_num = ui->trace_num_line->text().toLong();
        trace_header_window->setTraceHeader(segyF.traceHeader_QString(t_num));
    }
    if (! trace_header_window->isVisible()){
        trace_header_window->show();
    }
}

void MainWindow::change_trace_header(QString t_num) {
    trace_header_window->setTraceHeader(segyF.traceHeader_QString(t_num.toLong()));
}

void MainWindow::on_scan_btn_clicked()
{
    if (is_open_segy_) {
        int in_loc = ui->inline_loc_comb->currentText().toInt();
        int x_loc = ui->xline_loc_comb->currentText().toInt();
        int xloc = ui->x_loc_comb->currentText().toInt();
        int yloc = ui->y_loc_comb->currentText().toInt();
        segyF.set_location(in_loc, x_loc, xloc, yloc);
        ui->scan_out_textline->setText(segyF.scan());
    }
}

void MainWindow::on_convrt_dat_btn_clicked()
{
    if (is_open_segy_) {
        if(! ui->outname_line->text().isEmpty()){
            bool is_complete = segyF.toDat(ui->outname_line->text());

            if (is_complete) {
                QMessageBox message(QMessageBox::NoIcon, "Finished", "Success!");
                message.exec();
            }

        }
        else {
            QMessageBox message(QMessageBox::NoIcon, "Name Error",
                                "Out file name is Empty!");
            message.exec();
        }
    }
}


void MainWindow::on_savefile_btn_clicked()
{
    QString save_name = QFileDialog::getSaveFileName(
                                this,
                                "Save Dat File",
                                ".",
                                "Binary File(*.dat)");

    if (! save_name.isEmpty()) {
        ui->outname_line->setText(save_name);
    }
}


void MainWindow::on_trace_num_line_returnPressed()
{
    trace_header_window->set_trace_num(ui->trace_num_line->text());
    if (is_open_segy_) {
        int64_t t_num = ui->trace_num_line->text().toLong();
        trace_header_window->setTraceHeader(segyF.traceHeader_QString(t_num));
    }
    if (! trace_header_window->isVisible()){
        trace_header_window->show();
    }
}

