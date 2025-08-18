#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <qcustomplot.h>
#include "dataproduct.h"

#include "fft.h"

/* 光添加头文件QChartView还不够,还需要引入QChart的命名空间 */
#include "qserailthread.h"

QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QserailThread qstd;
    DataProduct dpt;
    QTimer timer;
    QTimer timer1;
private slots:
    void on_toolButton_clicked(bool checked);
    void update_data();
    void update_plot();
    void on_toolButton_2_clicked(bool checked);
    void on_clean_clicked();

    void on_pushButton_x_add_clicked();

    void on_pushButton_x_sub_clicked();

    void on_pushButton_y_add_clicked();

    void on_pushButton_y_sub_clicked();

    void on_comboBox_2_currentIndexChanged(int index);
    void on_checkBox_ref_stateChanged(int arg1);

    void on_checkBox_ref_fft_stateChanged(int arg1);

    void on_checkBox_feedback_stateChanged(int arg1);

    void on_checkBox_feedback_fft_stateChanged(int arg1);

    void on_checkBox_err_stateChanged(int arg1);

    void on_checkBox_err_fft_stateChanged(int arg1);

    void on_checkBox_out_stateChanged(int arg1);

    void on_checkBox_out_fft_stateChanged(int arg1);

private:
    Ui::Widget *ui;
    fft_config_t* real_fft_plan;
    /* 图表对象 */
    QCustomPlot* customPlot;
    QCPItemTracer* tracer;

    /* 横纵坐标初始显示范围 */
    const int AXIS_MIN_X = 0,AXIS_MAX_X = 200000,AXIS_MIN_Y = -35000, AXIS_MAX_Y = 35000;

    /* 用来记录数据点数 */
    int pointCount = 0;
    float scope_y = 70000;
    float scope_x = 200000;
    /* 保持点值 */
    double vx,vy,vx1,vy1,vx2,vy2;

};
#endif // WIDGET_H
