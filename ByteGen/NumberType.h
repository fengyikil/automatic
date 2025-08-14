#ifndef NUMBERTYPE_H
#define NUMBERTYPE_H

enum NumberType {
    DecimalInteger,
    Hexadecimal,
    DecimalFloat,
    InvalidNumber
};

extern int test_number_tytpe(void);
NumberType judgeNumberType(const QString &str);

#endif // NUMBERTYPE_H
