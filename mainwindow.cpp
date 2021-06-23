#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboBoxPattern->addItem("默认数据");
    ui->comboBoxPattern->addItem("自定义数据");

    ui->comboBoxAlgorithm->addItem("首次适应算法");
    ui->comboBoxAlgorithm->addItem("最佳适应算法");

    ui->textEdit->setReadOnly(true);//文本框只读

    model = RAMModel();//初始化model

    is_configured = false;//初始是未配置状态

    ui->RAMtextEdit->hide();

    this->setFixedSize(1326, 615);

}



void MainWindow::on_configureBtn_clicked()
{
    is_configured = true;//已配置状态

    data_source source;
    algorithm al;
    int capacity;

    //禁止在运行中调整
    ui->comboBoxPattern->setDisabled(true);
    ui->comboBoxAlgorithm->setDisabled(true);
    ui->spinBox_ramCapacity->setDisabled(true);
    ui->configureBtn->setDisabled(true);

    if(ui->comboBoxPattern->currentIndex() == 0)
    {
        ui->spinBox_apply->setDisabled(true);
        ui->spinBox_release->setDisabled(true);
        ui->configureBtn_apply->setDisabled(true);
        ui->configureBtn_release->setDisabled(true);

        source = DEFAULT;//默认数据
    }
    else {
        ui->nextStepBtn->setDisabled(true);
        source = CUSTOMIZE;//自定义数据
    }

    //记录设置的算法
    if(ui->comboBoxAlgorithm->currentIndex() == 0)
    {
        al = FIRST_FIT;
    }
    else {
        al = BEST_FIT;
    }
    
    capacity = ui->spinBox_ramCapacity->value();//记录设置的容量
    
    model.setting(source, al, capacity);//设置model
}

void MainWindow::on_resetBtn_clicked()
{
    is_configured = false;//未配置状态
    model = RAMModel();//重置model

    //全部设置为可以使用
    ui->comboBoxPattern->setDisabled(false);
    ui->comboBoxAlgorithm->setDisabled(false);
    ui->spinBox_ramCapacity->setDisabled(false);
    ui->configureBtn->setDisabled(false);
    ui->spinBox_apply->setDisabled(false);
    ui->spinBox_release->setDisabled(false);
    ui->configureBtn_apply->setDisabled(false);
    ui->configureBtn_release->setDisabled(false);
    ui->nextStepBtn->setDisabled(false);
    //各种值也回到原来的设置
    ui->comboBoxPattern->setCurrentIndex(0);
    ui->comboBoxAlgorithm->setCurrentIndex(0);
    ui->spinBox_ramCapacity->setValue(640);
    ui->spinBox_apply->setValue(0);
    ui->spinBox_release->setValue(0);



    //绘制区域重置

    repaint();



    //日志信息清除
    ui->textEdit->clear();

}

void MainWindow::on_nextStepBtn_clicked()
{
    //未配置状态时，直接返回
    if(is_configured == false)
    {
        ui->textEdit->append("<font color=\"#FF0000\">请点击确认配置按钮后再运行！！</font>");
        return;
    }

    //如果还有剩余数据
    if(model.has_left_default_data() == true)
    {
        auto inf = model.next_step();//按默认数据的下一步进行

        //显示日志
        log_inf(inf);//调用显示日志函数

        //绘制图形

        repaint();//必须调用repaint函数，才能使绘制的图形显现

    }
    else
    {
        //在日志信息中显示没有剩余数据
        ui->textEdit->append("<font color=\"#FF0000\">默认数据已使用完!!<br>请重置或退出程序!!</font>");

    }
}


void MainWindow::log_inf(request_inf inf)
{
//    qDebug() << inf.job_num << ": " << inf.apply_or_release << " " << inf.lenth;

    QString log_line;
    log_line += QString("作业") + QString::number(inf.job_num);
    log_line += inf.apply_or_release == -1 ? QString("申请") : QString("释放");
    if(inf.apply_or_release == -1)
        log_line += QString::number(inf.lenth) + QString("K");
    log_line += inf.request_state == true ? QString("成功") : QString("失败");

    //如果没有成功则设置为红色
    if(inf.request_state == false)
        log_line = QString("<font color=\"#FF0000\">") + log_line + QString("</font> ");

    ui->textEdit->append(log_line);
}

//void MainWindow::paint_ram(QPaintEvent *event)
//{
//    QPainter painter(this);
//    painter.drawLine(QPointF(0, 0), QPointF(900, 800));


//}

void MainWindow::paintEvent(QPaintEvent *event)
{
    //绘制区域的坐标
    auto origin_x = ui->RAMtextEdit->geometry().x();
    auto origin_y = ui->RAMtextEdit->geometry().y();
    auto width = ui->RAMtextEdit->geometry().width();
    auto height = ui->RAMtextEdit->geometry().height();

    QPainter painter(this);
    QPen pen; //画笔
    pen.setColor(QColor(0, 0, 0));//黑色画笔画外框
    QBrush brush(QColor(165, 165, 165, 255)); //画刷
    painter.setPen(pen); //添加画笔
    painter.setBrush(brush); //添加画刷
    painter.drawRect(origin_x, origin_y, width, height); //绘制总体外框

    qDebug() << "x:" << origin_x << "y: " << origin_y << "width:" << width << "height:" << height;

    //各种颜色
    QColor color_collection[16] = {
        QColor("#FFB7DD"),
        QColor("#FF44AA"),
        QColor("#C10066"),
        QColor("#FFDDAA"),
        QColor("#FFAA33"),
        QColor("#EE7700"),
        QColor("#EEFFBB"),
        QColor("#CCFF33"),
        QColor("#99DD00"),
        QColor("#BBFFEE"),
        QColor("#33FFAA"),
        QColor("#00DD77"),
        QColor("#CCDDFF"),
        QColor("#5599FF"),
        QColor("#0044BB"),
        QColor("#9955FF"),
    };

//    std::vector<std::vector<int>> pos;

    for(unsigned int i = 0; i < model.jobs_in_ram.size(); i++)
    {
        QBrush brushTemp(color_collection[model.jobs_in_ram[i].job_num % 15]);
        painter.setBrush(brushTemp);//设置颜色

        //位置设置
        auto block_x = (width * (model.jobs_in_ram[i].block_in_ram.start_addr + 1) / model.get_capacity()) + origin_x;
        auto block_y = origin_y;
        auto block_width = width * model.jobs_in_ram[i].block_in_ram.lenth / model.get_capacity();
        auto block_height = height;

        qDebug() << block_x << block_y << block_width << block_height << color_collection[model.jobs_in_ram[i].job_num % 15];
        painter.drawRect(block_x, block_y, block_width, block_height);

        QString job_num("作业");
        job_num += QString::number(model.jobs_in_ram[i].job_num);
        painter.drawText(block_x + block_width / 2 - width * block_width / 10000, block_y + block_height / 2, job_num);

        QString job_lenth;
        job_lenth += QString::number(model.jobs_in_ram[i].block_in_ram.lenth) + "K";
        painter.drawText(block_x + block_width / 2 - width * block_width / 10000, block_y + block_height / 2 + height / 9, job_lenth);

    }
}

void MainWindow::on_configureBtn_apply_clicked()
{
    if(is_configured == false)
    {
        ui->textEdit->append("<font color=\"#FF0000\">请点击确认配置按钮后再运行！！</font>");
        return;
    }


    if(ui->spinBox_apply->value() == 0)
    {
        ui->textEdit->append("<font color=\"#FF0000\">不能申请0K的空间！！</font>");
        return;
    }
    else
    {
        request_inf inf;
        inf.job_num = model.current_job_num;
        inf.apply_or_release = -1;
        inf.request_state = model.apply_ram(model.current_job_num, ui->spinBox_apply->value());
        inf.lenth = ui->spinBox_apply->value();
        if(inf.request_state == true)
        {
            model.current_job_num++;
        }

        log_inf(inf);

        repaint();

    }


}

void MainWindow::on_configureBtn_release_clicked()
{
    if(is_configured == false)
    {
        ui->textEdit->append("<font color=\"#FF0000\">请点击确认配置按钮后再运行！！</font>");
        return;
    }

    request_inf inf;
    inf.job_num = ui->spinBox_release->value();
    inf.apply_or_release = 1;
    inf.request_state = model.release_ram(ui->spinBox_release->value());
    //不用设置inf.lenth,因为没有用到

    log_inf(inf);

    repaint();


}
MainWindow::~MainWindow()
{

    delete ui;
}








