#ifndef TRACEHEADERWINDOW_H
#define TRACEHEADERWINDOW_H

#include <QWidget>

namespace Ui {
class TraceHeaderWindow;
}

class TraceHeaderWindow : public QWidget {
  Q_OBJECT

 public:
  explicit TraceHeaderWindow(QWidget *parent = nullptr);
  ~TraceHeaderWindow();
  void set_trace_num(QString);
  void setTraceHeader(QString trace_header);

 signals:
  void changeText(QString t_num);

 private slots:
  void on_trace_num_line_returnPressed();

 private:
  Ui::TraceHeaderWindow *ui;
};

#endif  // TRACEHEADERWINDOW_H
