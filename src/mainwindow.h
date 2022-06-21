#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>

#include "binaryheaderwindow.h"
#include "segy.h"
#include "textheaderwindow.h"
#include "traceheaderwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  Segy segyF = Segy();  // segy file

 private slots:
  // change text of textheader window to EBCDIC or ASCII format
  void change_text_header(QString formt);

  // change the text by a trace index
  void change_trace_header(QString t_num);

  // show the process of scan
  void show_scan_process(int proc);

  // open file diagm
  void on_openfile_btn_clicked();

  // open textheader window
  void on_textheader_wind_btn_clicked();

  // open binaryheader window
  void on_binheader_wind_btn_clicked();

  // open traceheader window, show the specail trace header
  void on_traceheader_wind_btn_clicked();

  // scan segy file
  void on_scan_btn_clicked();

  // convert segy file to binary file (.dat)
  void on_convrt_dat_btn_clicked();

  // select a dir and input a specail .dat file name
  void on_savefile_btn_clicked();

  // press enter to change the text of a trace header
  void on_trace_num_line_returnPressed();

 private:
  bool is_open_segy_ = false;
  //    Segy segyF = Segy();

  Ui::MainWindow *ui;
  TextHeaderWindow *text_header_window = new TextHeaderWindow;
  BinaryHeaderWindow *binary_header_window = new BinaryHeaderWindow;
  TraceHeaderWindow *trace_header_window = new TraceHeaderWindow;

  void init_connect();
};
#endif  // MAINWINDOW_H
