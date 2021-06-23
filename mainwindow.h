#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <graphram.h>
#include <rammodel.h>
#include <QPainter>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_configureBtn_clicked();//确认配置按钮按下时

    void on_nextStepBtn_clicked();//默认情况的按钮被按下时

    void on_resetBtn_clicked();//重置和停止按钮被按下时

    void on_configureBtn_apply_clicked();

    void on_configureBtn_release_clicked();

private:
    Ui::MainWindow *ui;

    bool is_configured;//是否确认配置，如果没有确认的话，点击后面的按钮无效

    void log_inf(request_inf inf);//显示日志信息



protected:

    void paintEvent(QPaintEvent *);//绘制内存的图形

//    void paint_ram(QPaintEvent *event);

    //内存的模型
    RAMModel model;
};

#endif // MAINWINDOW_H
