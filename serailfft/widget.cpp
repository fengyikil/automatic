#include "widget.h"
#include "ui_widget.h"
#include <QSerialPortInfo>
#include <QDebug>
#include<QPen>
#include "math.h"
#include "filter.h"

float g_buf[PER_SIZE] = {0.0};
float g_out[PER_SIZE] = {0.0};
//LPF_2OrdType lpf_2nd_c_p;		//速度环P值滤

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        ui->comboBox->addItem(info.portName());
    }
    customPlot = ui->widget;
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(QColor(0, 0, 204)));
    customPlot->graph(0)->setVisible(false);

    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(QColor(153, 102, 0)));
    customPlot->graph(1)->setVisible(false);

    customPlot->addGraph();
    customPlot->graph(2)->setPen(QPen(QColor(204, 0, 0)));
    customPlot->graph(2)->setVisible(false);

    customPlot->addGraph();
    customPlot->graph(3)->setPen(QPen(QColor(0, 153, 0)));
    customPlot->graph(3)->setVisible(false);

    customPlot->addGraph();
    customPlot->graph(4)->setPen(QPen(QColor(204, 0, 255)));
    customPlot->graph(4)->setVisible(false);

    customPlot->addGraph();
    customPlot->graph(5)->setPen(QPen(QColor(102, 255, 0)));
    customPlot->graph(5)->setVisible(false);

    customPlot->addGraph();
    customPlot->graph(6)->setPen(QPen(QColor(0, 153, 51)));
    customPlot->graph(6)->setVisible(false);

    customPlot->addGraph();
    customPlot->graph(7)->setPen(QPen(QColor(0, 102, 153)));
    customPlot->graph(7)->setVisible(false);

    customPlot->xAxis->setRange(AXIS_MIN_X,AXIS_MAX_X);
    customPlot->yAxis->setRange(AXIS_MIN_Y,AXIS_MAX_Y);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
                                QCP::iSelectLegend | QCP::iSelectPlottables);


    tracer = new QCPItemTracer(customPlot);
    //下面的代码就是设置游标的外观
    this->setMouseTracking(true);
    tracer->setInterpolating(false);//禁用插值
    tracer->setPen(QPen(Qt::DashLine));//虚线游标
    tracer->setStyle(QCPItemTracer::tsNone);//游标样式：十字星、圆圈、方框

    ui->label_cursor->setText(QString("x = %1\ny = %2").arg( 0).arg(0));
    ui->label_pos1->setText(QString("x1 = %1\ny1 = %2").arg( 0).arg(0));
    ui->label_pos2->setText(QString("x2 = %1\ny2 = %2").arg( 0).arg(0));
    ui->label_pos_diff->setText(QString("dx = %1\ndy = %2").arg(0).arg(0));



    connect(customPlot, &QCustomPlot::mouseMove,this,[=](QMouseEvent *event){
        double x = customPlot->xAxis->pixelToCoord(event->pos().x());
        //下面的代码很关键
        //        tracer->setGraph(customPlot->graph(0)); //设置游标吸附于指定曲线
        //        tracer->setGraph(NULL);
        tracer->setGraphKey(x); //将游标横坐标（key）设置成刚获得的横坐标数据x
        tracer->setInterpolating(true); //游标的纵坐标可以通过曲线数据线性插值自动获得
        tracer->updatePosition(); //使得刚设置游标的横纵坐标位置生效
        //以下代码用于更新游标说明的内容
        vx = tracer->position->key();
        vy = ((int)(tracer->position->value()*1000))/1000.0; //y轴值保留三位有效数字
        //        tracerLabel->setText(QString("x = %1\n y = %2").arg(QDateTime::fromMSecsSinceEpoch(xValue*1000.0).toString("yy-MM-dd hh:mm:ss.zzz")).arg(yValue));//(*1000) ms 转 s
        //        tracerLabel->setText(QString("x = %1\n y = %2").arg( xValue).arg(yValue));//

        ui->label_cursor->setText(QString("x = %1\ny = %2").arg( vx).arg(vy));
        customPlot->replot(); //刷新图标，不能省略
    });

    connect(customPlot, &QCustomPlot::mouseDoubleClick,this,[=](QMouseEvent *event){
        static bool sw;
        if(!sw)
        {
            vx1 = vx;
            vy1 = vy;
            vx2 = 0;
            vy2 = 0;
            sw = true;
            ui->label_pos_diff->setText(QString("dx = %1\ndy = %2").arg(0).arg(0));
        }
        else
        {
            vx2 = vx;
            vy2 = vy;
            sw = false;
            ui->label_pos_diff->setText(QString("dx = %1\ndy = %2").arg( fabs(vx2-vx1)).arg(fabs(vy2-vy1)));

        }
        ui->label_pos1->setText(QString("x1 = %1\ny1 = %2").arg( vx1).arg(vy1));
        ui->label_pos2->setText(QString("x2 = %1\ny2 = %2").arg( vx2).arg(vy2));
    });


    real_fft_plan = fft_init(PER_SIZE, FFT_REAL, FFT_FORWARD, g_buf, g_out);
    QObject::connect(&timer,SIGNAL(timeout()),this,SLOT(update_data()));
    QObject::connect(&timer1,SIGNAL(timeout()),this,SLOT(update_plot()));

    //    customPlot->legend->setVisible(true);
    //    QObject::connect(
    //        customPlot,
    //        &QCustomPlot::legendClick,
    //        [](QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event)
    //        {
    //            auto plItem = qobject_cast<QCPPlottableLegendItem*>(item) ;
    //            plItem->plottable()->setVisible(!plItem->plottable()->visible());
    //            // how to get to the relevant graph from the item variable?
    //        } );

//    LPF_2Ord_Init(2000.0f, 100.0f , 0.7071f, &lpf_2nd_c_p);
    timer1.start(60);

}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_toolButton_clicked(bool checked)
{
    if(checked) {
        qDebug()<<"hh";
        qstd.OpenSerialPort(ui->comboBox->currentText());
        ui->toolButton->setText("关闭");
    }
    else {
        qstd.CloseSerialPort();
        ui->toolButton->setText("打开");

    }
}

//void Widget::update_data()
//{
//    static float mvalue;
//    float tmp[PER_SIZE]={0};
//    float max = 0;
//    float lpf;
//    int count = pointCount;
//    if(fifo_get(qstd.ffd,g_buf,PER_SIZE*sizeof(float))==PER_SIZE*sizeof(float)){
//        fft_execute(real_fft_plan);
//        for(int i = 0;i<PER_SIZE;i++){
//            customPlot->graph(0)->addData(count,g_buf[i]);
//            lpf = LPF_2Ord_Cal(g_buf[i], &lpf_2nd_c_p);	 //二阶低通滤波
//            g_buf[i] = lpf;
////            customPlot->graph(2)->addData(pointCount,lpf);
//            if(i>1 && i<PER_SIZE/2)
//            {
//                tmp[i] = sqrt(pow(real_fft_plan->output[2 * i], 2) + pow(real_fft_plan->output[2 * i + 1], 2))/ (PER_SIZE/2);
//                customPlot->graph(1)->addData(count,tmp[i]);
//            }
//            else
//            {
//                customPlot->graph(1)->addData(count,-10);
//            }
//            count++;
//        }
//        fft_execute(real_fft_plan);
//        for(int i = 0;i<PER_SIZE;i++){
//            customPlot->graph(2)->addData(pointCount,g_buf[i]);

//            if(i>1 && i<PER_SIZE/2)
//            {
//                tmp[i] = sqrt(pow(real_fft_plan->output[2 * i], 2) + pow(real_fft_plan->output[2 * i + 1], 2))/ (PER_SIZE/2);
//                customPlot->graph(3)->addData(pointCount,tmp[i]);
//            }
//            else
//            {
//                customPlot->graph(3)->addData(pointCount,-10);
//            }
//            pointCount++;
//        }

//    }
//    max = tmp[10];
//    int count1 = 15;
//    for(int i = 11; i<38;i++ )
//    {
//        if(max<tmp[i])
//        {
//            max = tmp[i];
//            count1 = i;
//        }
//    }
//    mvalue = mvalue *0.9 + max*0.1;
//    ui->label_3->setText(QString::number(mvalue));
//    ui->label_count->setText(QString::number(count));

//    QCPRange curRange = customPlot->xAxis->range();
//    float scope = (curRange.upper - curRange.lower);
//    scope_x = scope;
//    if(pointCount>scope_x)
//        customPlot->xAxis->setRange(pointCount - scope_x,pointCount);
//}

void Widget::update_data()
{
    dpt.DataHandle(qstd.ff);
    float buf[PER_SIZE]={0};
    int count = pointCount;
    for(int i = 0;i<DATA_GUI_CHANNEL_NUM;i++)
    {
        if(fifo_get(dpt.fc[i],buf,PER_SIZE*sizeof(float)) == PER_SIZE*sizeof(float))
        {
            count = pointCount;
            for(int j = 0;j<PER_SIZE;j++)
            {
                customPlot->graph(i)->addData(count,buf[j]);
                count++;
            }
        }
    }
    pointCount = count;

    QCPRange curRange = customPlot->xAxis->range();
    float scope = (curRange.upper - curRange.lower);
    scope_x = scope;
    if(pointCount>scope_x)
    {
        customPlot->xAxis->setRange(pointCount - scope_x,pointCount);
    }
}


void Widget::update_plot()
{
    customPlot->replot();
}


void Widget::on_toolButton_2_clicked(bool checked)
{
    if(checked) {
        timer.start(PER_SIZE/4);
    }
    else {
//        timer1.stop();
        timer.stop();
    }
}

void Widget::on_clean_clicked()
{

    customPlot->graph(0)->data().data()->clear();
    customPlot->graph(1)->data().data()->clear();
    customPlot->graph(2)->data().data()->clear();
    customPlot->graph(3)->data().data()->clear();
    customPlot->graph(4)->data().data()->clear();
    customPlot->graph(5)->data().data()->clear();
    customPlot->graph(6)->data().data()->clear();
    customPlot->graph(7)->data().data()->clear();

    customPlot->xAxis->setRange(0, scope_x/2);

    pointCount = 0;
}





void Widget::on_pushButton_x_add_clicked()
{
    QCPRange curRange = customPlot->xAxis->range();
    float scope = (curRange.upper - curRange.lower)/2;
    float mid = curRange.center();
    customPlot->xAxis->setRange(mid - scope/2, mid+ scope/2);
    scope_x = scope;
}

void Widget::on_pushButton_x_sub_clicked()
{
    QCPRange curRange = customPlot->xAxis->range();
    float scope = (curRange.upper - curRange.lower)*2;
    float mid = curRange.center();
    customPlot->xAxis->setRange(mid - scope/2, mid+ scope/2);
    scope_x = scope;
}

void Widget::on_pushButton_y_add_clicked()
{
    QCPRange curRange = customPlot->yAxis->range();
    float scope = (curRange.upper - curRange.lower)/2;
    float mid = curRange.center();
    customPlot->yAxis->setRange(mid - scope/2, mid+ scope/2);
}

void Widget::on_pushButton_y_sub_clicked()
{
    QCPRange curRange = customPlot->yAxis->range();
    float scope = (curRange.upper - curRange.lower)*2;
    float mid = curRange.center();
    customPlot->yAxis->setRange(mid - scope/2, mid+ scope/2);

}

void Widget::on_comboBox_2_currentIndexChanged(int index)
{
    if(index == 0)
    {
        tracer->setGraph(NULL);
        tracer->setStyle(QCPItemTracer::tsNone);//游标样式：十字星、圆圈、方框
        tracer->setVisible(false);
    }
    else
    {
        tracer->setVisible(true);
        tracer->setStyle(QCPItemTracer::tsCrosshair);//游标样式：十字星、圆圈、方框
        tracer->setGraph(customPlot->graph(index - 1)); //设置游标吸附于指定曲线
    }
}


void Widget::on_checkBox_ref_stateChanged(int arg1)
{
    if(arg1)
    {
        customPlot->graph(0)->setVisible(true);
    }
    else
    {
        customPlot->graph(0)->setVisible(false);
    }
}

void Widget::on_checkBox_ref_fft_stateChanged(int arg1)
{
    if(arg1)
    {
        customPlot->graph(1)->setVisible(true);
    }
    else
    {
        customPlot->graph(1)->setVisible(false);
    }
}

void Widget::on_checkBox_feedback_stateChanged(int arg1)
{
    if(arg1)
    {
        customPlot->graph(2)->setVisible(true);
    }
    else
    {
        customPlot->graph(2)->setVisible(false);
    }
}

void Widget::on_checkBox_feedback_fft_stateChanged(int arg1)
{
    if(arg1)
    {
        customPlot->graph(3)->setVisible(true);
    }
    else
    {
        customPlot->graph(3)->setVisible(false);
    }
}

void Widget::on_checkBox_err_stateChanged(int arg1)
{
    if(arg1)
    {
        customPlot->graph(4)->setVisible(true);
    }
    else
    {
        customPlot->graph(4)->setVisible(false);
    }
}

void Widget::on_checkBox_err_fft_stateChanged(int arg1)
{
    if(arg1)
    {
        customPlot->graph(5)->setVisible(true);
    }
    else
    {
        customPlot->graph(5)->setVisible(false);
    }
}

void Widget::on_checkBox_out_stateChanged(int arg1)
{
    if(arg1)
    {
        customPlot->graph(6)->setVisible(true);
    }
    else
    {
        customPlot->graph(6)->setVisible(false);
    }
}

void Widget::on_checkBox_out_fft_stateChanged(int arg1)
{
    if(arg1)
    {
        customPlot->graph(7)->setVisible(true);
    }
    else
    {
        customPlot->graph(7)->setVisible(false);
    }
}
