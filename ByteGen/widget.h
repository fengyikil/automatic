#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    bool parser_tokens(QStringList tokens);

    QByteArray send_data;
    QString bit_send;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_parser_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
