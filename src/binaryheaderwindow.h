#ifndef BINARYHEADERWINDOW_H
#define BINARYHEADERWINDOW_H

#include <QWidget>

namespace Ui {
class BinaryHeaderWindow;
}

class BinaryHeaderWindow : public QWidget {
  Q_OBJECT

 public:
  explicit BinaryHeaderWindow(QWidget *parent = nullptr);
  ~BinaryHeaderWindow();

  void setBinaryHeader(QString binary_header);

 private:
  Ui::BinaryHeaderWindow *ui;
};

#endif  // BINARYHEADERWINDOW_H
