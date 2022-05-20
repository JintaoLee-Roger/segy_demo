#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "textheaderwindow.h"
#include "binaryheaderwindow.h"
#include "traceheaderwindow.h"
#include "segy.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Segy segyF = Segy();

private slots:
    void change_text_header(QString formt);

    void change_trace_header(QString t_num);

    void on_openfile_btn_clicked();

    void on_textheader_wind_btn_clicked();

    void on_binheader_wind_btn_clicked();

    void on_traceheader_wind_btn_clicked();

    void on_scan_btn_clicked();

    void on_convrt_dat_btn_clicked();

    void on_savefile_btn_clicked();

    void on_trace_num_line_returnPressed();

private:
    bool is_open_segy_ = false;
//    Segy segyF = Segy();

    Ui::MainWindow *ui;
    TextHeaderWindow *text_header_window = new TextHeaderWindow;
    BinaryHeaderWindow *binary_header_window = new BinaryHeaderWindow;
    TraceHeaderWindow *trace_header_window = new TraceHeaderWindow;
};
#endif // MAINWINDOW_H
