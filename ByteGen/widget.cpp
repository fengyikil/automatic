#include "widget.h"
#include "ui_widget.h"
#include "QDebug"
#include "stdio.h"
#include "NumberType.h"
#include <QRegularExpression>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

static const uint16_t kCcittTab[] = { // CRC 余式表
                                      0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
                                      0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
                                      0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
                                      0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
                                      0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
                                      0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
                                      0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
                                      0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
                                      0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
                                      0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
                                      0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
                                      0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
                                      0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
                                      0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
                                      0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
                                      0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
                                      0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
                                      0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
                                      0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
                                      0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
                                      0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
                                      0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
                                      0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
                                      0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
                                      0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
                                      0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
                                      0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
                                      0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
                                      0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
                                      0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
                                      0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
                                      0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
                                    };


//**********************************************************
// brief      crc校验，对传入数组从头到尾全部校验
// param[in]  ary: 要校验的数组
// param[in]  len:   数组长度
// param[out] none
// retval     数组校验值
//**********************************************************
uint16_t CrcCalcul(uint8_t *ary, uint32_t len) {
    uint8_t  dat;
    uint16_t crc = 0;
    while (len-- != 0) {
        dat = crc >> 8;
        crc <<= 8;
        crc ^= kCcittTab[dat ^ *ary];
        ary++;
    }
    return crc;
}

bool Widget::parser_tokens(QStringList tokens)
{
    qDebug() << "count is " << tokens.count();

    if (tokens.count() != 0 && tokens.count() != 2)
    {
        qDebug() << "err";
        return false;
    }
    if (tokens.count() == 0)
    {
        return true;
    }

    QString type = tokens[0];
    QString value = tokens[1];
    QByteArray tmp;

    char* p;
    value = value.trimmed();
    //判断是否是单个字符
    if (value[0] == "'"&&value[2] == "'"&&value.length() == 3)
    {
        //        res = QString::number(value[1].digitValue());
        if (type == "char") {
            //			QString tc = value[1];
            tmp.append(value[1].toLatin1());
            send_data.append(tmp);
            return true;
        }
        else
        {
            qDebug() << "err";
            return false;
        }
    }
    //判断是否是字符串
    else if (value[0] == "\""&&value[value.length() - 1] == "\"")
    {
        if (type == "str")
        {
            value = value.remove(0, 1);
            value = value.remove(value.length() - 1, 1);
            send_data.append(value);
            return true;

        }
        else
        {
            qDebug() << "err";
            return false;
        }
    }
    //判断是否是数字
    else
    {
        NumberType nt =  judgeNumberType(value);
        bool ok;
        double num;
        if(nt == DecimalInteger || nt == DecimalFloat ) {
            num = value.toDouble(&ok);
        } else if(nt == Hexadecimal) {
            num = (double)value.toInt(&ok, 16);

        } else {
            return 0 ;
        }

        if (ok)
        {
            if (type == "char")
            {
                if (num >= -128 && num <= 255)
                {
                    char c = num;
                    send_data.append(1, c);
                    return true;
                }
                else
                    return false;
            }
            else if (type == "short")
            {
                if (num >= -3278 && num <= 65535)
                {
                    short s = num;
                    p = (char*)&s;
                    tmp = QByteArray(p, 2);
                    send_data.append(tmp);
                    return true;
                }
                else
                    return false;

            }
            else if (type == "int")
            {
                int i = num;
                if (num >= 0x0 && num <= 0xffffffff)
                {

                    p = (char*)&i;
                    tmp = QByteArray(p, 4);
                    send_data.append(tmp);
                    return true;
                }
                else
                    return false;
            }
            else if (type == "float")
            {
                float f = num;
                p = (char*)&f;
                tmp = QByteArray(p, 4);
                send_data.append(tmp);
                return true;
            }
            else if (type == "double")
            {
                p = (char*)&num;
                tmp = QByteArray(p, 8);
                send_data.append(tmp);
                return true;
            }
            else
            {
                qDebug() << "err";
                return false;
            }

        }
        else
        {
            qDebug() << "err";
            return false;
        }
    }
}

QString add_crc(QString data)
{
    uint8_t buf[5120];
    uint16_t obuf;
    QString Stmp;
    uint8_t* p = (uint8_t*)&obuf;
    QStringList strList = data.split(" ");
    int len = strList.length();
    for(int i = 0;i<len;i++)
    {
        QString str = strList[i];
        buf[i] = (char)str.toInt(NULL,16);
        QString Stmp;
        qDebug() << Stmp.sprintf("0x%02x", buf[i]);
    }
    obuf =  CrcCalcul(buf+2,len-2);
    buf[len] = p[0];
    buf[len+1] = p[1];
    qDebug() << Stmp.sprintf("0x%02x", buf[len]);
    qDebug() << Stmp.sprintf("0x%02x", buf[len+1]);


    strList.clear();

    uint8_t xbuf[5120]={0};
    for(int i=0;i<len+2;i++)
    {
        sprintf((char*)(xbuf+3*i),"%02x ",buf[i]);
    }
    QString ostr((char*)xbuf);
    ostr.chop(1);
    return ostr;
}

void Widget::on_pushButton_clicked()
{
    uint8_t buf[5120];
    uint16_t obuf;
    uint8_t* p = (uint8_t*)&obuf;
    QString content = ui->lineEdit->text();
    content = content.trimmed();
    QString Stmp;
    QStringList strList = content.split(" ");
    int len = strList.length();
    for(int i = 0;i<len;i++)
    {
        QString str = strList[i];
        buf[i] = (char)str.toInt(NULL,16);
        QString Stmp;
        qDebug() << Stmp.sprintf("0x%02x", buf[i]);
    }
    obuf =  CrcCalcul(buf,len);
    buf[len] = p[0];
    buf[len+1] = p[1];
    qDebug() << Stmp.sprintf("0x%02x", buf[len]);
    qDebug() << Stmp.sprintf("0x%02x", buf[len+1]);


    strList.clear();

    uint8_t xbuf[5120]={0};
    for(int i=0;i<len+2;i++)
    {
        sprintf((char*)(xbuf+3*i),"%02x ",buf[i]);
    }
    QString ostr((char*)xbuf);
    ostr.chop(1);
    ui->lineEdit_2->setText(ostr);

}

void Widget::on_pushButton_parser_clicked()
{
    QString programs = ui->plainTextEdit_program->toPlainText();
    send_data.clear();
    bit_send.clear();

    QString prgm = ui->plainTextEdit_program->toPlainText();
    prgm.replace(QRegularExpression("//[^\n]*"), "");
    qDebug()<<prgm;

    QStringList lines = prgm.split('\n');
    qDebug() << lines;

    QStringList tokens;
    QString token;

    foreach(QString line, lines) {
        tokens.clear();
        for (int i = 0; i < line.length(); i++)
        {
            if (line[i] == ' ' || line[i] == '\t')
            {
                if (token.contains('"'))
                {

                    token.append(line[i]);
                }
                else
                {
                    if (!token.isEmpty())
                    {
                        tokens.append(token);
                        token.clear();
                    }
                }
            }
            else
            {
                token.append(line[i]);
            }
        }
        if (!token.isEmpty())
        {
            tokens.append(token);
            token.clear();
        }
        if (!parser_tokens(tokens))
        {
            bit_send.append("parse err!");
            ui->plainTextEdit_send->clear();
            ui->plainTextEdit_send->appendPlainText(bit_send);
            return;
        }

        qDebug() << tokens;
    }

    char zh[10];
    char* p = send_data.data();
    bit_send.append(QString("length is %1 :\n\n").arg(QString::number(send_data.length())));
    QString data;
    QString data1;
    for (int i = 0; i < send_data.length(); i++)
    {
        unsigned char s = p[i];
        sprintf(zh, "%02x ", s);
        data.append(QString(zh));
        //        printf("%s",zh);

        //        printf("%2s\t",zh);
    }
    data.chop(1);
    data1 = add_crc(data);
    qDebug() << "length is" << send_data.length();
    qDebug() << bit_send + data + "add crc:\n\n";
    //    bit_send.chop(1);
    ui->plainTextEdit_send->clear();
    ui->plainTextEdit_send->appendPlainText(bit_send + data + "\n\nadd crc:\n\n" + data1);
}
