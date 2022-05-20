#ifndef TEXTHEADERWINDOW_H
#define TEXTHEADERWINDOW_H

#include <QWidget>


namespace Ui {
class TextHeaderWindow;
}

class TextHeaderWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TextHeaderWindow(QWidget *parent = nullptr);
    ~TextHeaderWindow();

    void setTextHeader(QString text_header);

signals:
    void changeFormat(QString formt);

private slots:
    void on_ebcdicF_btn_clicked();

    void on_asciiF_btn_clicked();

private:
    Ui::TextHeaderWindow *ui;

};

#endif // TEXTHEADERWINDOW_H
