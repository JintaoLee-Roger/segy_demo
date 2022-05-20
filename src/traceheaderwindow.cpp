#include "traceheaderwindow.h"
#include "ui_traceheaderwindow.h"

TraceHeaderWindow::TraceHeaderWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TraceHeaderWindow)
{
    ui->setupUi(this);
}

TraceHeaderWindow::~TraceHeaderWindow()
{
    delete ui;
}

void TraceHeaderWindow::set_trace_num(QString num) {
    ui->trace_num_line->setText(num);
}

void TraceHeaderWindow::on_trace_num_line_returnPressed()
{
    emit changeText(ui->trace_num_line->text());
}

void TraceHeaderWindow::setTraceHeader(QString trace_header) {
    ui->textEdit->setText(trace_header);
}



