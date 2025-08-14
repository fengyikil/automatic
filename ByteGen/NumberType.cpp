#include <QString>
#include <QRegularExpression>
#include <QDebug>
#include "NumberType.h"

NumberType judgeNumberType(const QString &str)
{
    // 正则表达式匹配十进制整数
    QRegularExpression decimalIntegerRegex("^[-+]?[0-9]+$");
    // 正则表达式匹配十六进制数（带或不带0x前缀）
    QRegularExpression hexadecimalRegex("^(?:0x)?[0-9A-Fa-f]+$");
    // 正则表达式匹配浮点十进制数
    QRegularExpression decimalFloatRegex("^[-+]?[0-9]+(\\.[0-9]+)?([eE][-+]?[0-9]+)?$");

    if (decimalIntegerRegex.match(str).hasMatch()) {
        return DecimalInteger;
    } else if (hexadecimalRegex.match(str).hasMatch()) {
        return Hexadecimal;
    } else if (decimalFloatRegex.match(str).hasMatch()) {
        return DecimalFloat;
    } else {
        return InvalidNumber;
    }
}

int test_number_tytpe()
{
    QString str1 = "12345"; // 十进制整数
    QString str2 = "0xA3F"; // 十六进制数
    QString str3 = "123.45"; // 浮点十进制数
    QString str4 = "abc123"; // 非数字字符串

    NumberType type1 = judgeNumberType(str1);
    NumberType type2 = judgeNumberType(str2);
    NumberType type3 = judgeNumberType(str3);
    NumberType type4 = judgeNumberType(str4);

    qDebug() << "str1 is" << (type1 == DecimalInteger ? "a decimal integer" :
                               type1 == Hexadecimal ? "a hexadecimal number" :
                               type1 == DecimalFloat ? "a decimal float" : "an invalid number");
    qDebug() << "str2 is" << (type2 == DecimalInteger ? "a decimal integer" :
                               type2 == Hexadecimal ? "a hexadecimal number" :
                               type2 == DecimalFloat ? "a decimal float" : "an invalid number");
    qDebug() << "str3 is" << (type3 == DecimalInteger ? "a decimal integer" :
                               type3 == Hexadecimal ? "a hexadecimal number" :
                               type3 == DecimalFloat ? "a decimal float" : "an invalid number");
    qDebug() << "str4 is" << (type4 == DecimalInteger ? "a decimal integer" :
                               type4 == Hexadecimal ? "a hexadecimal number" :
                               type4 == DecimalFloat ? "a decimal float" : "an invalid number");

    return 0;
}
