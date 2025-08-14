#include "widget.h"
#include "NumberType.h"
#include <QApplication>
#include <QDebug>
#include <stdio.h>
#include <math.h>

// 定义一个三维向量结构体
typedef struct {
    double x;
    double y;
    double z;
} Vector3D;

// 计算两个向量的叉积
Vector3D crossProduct(const Vector3D u, const Vector3D v) {
    Vector3D result;
    result.x = u.y * v.z - u.z * v.y;
    result.y = u.z * v.x - u.x * v.z;
    result.z = u.x * v.y - u.y * v.x;
    return result;
}

// 计算两个向量的点积
double dotProduct(const Vector3D u, const Vector3D v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

// 计算向量在另一个向量方向上的投影
Vector3D projectVector(const Vector3D v, const Vector3D n) {
    double nDotN = dotProduct(n, n);
    if (nDotN == 0) {
        // 避免除以0
        return (Vector3D){0, 0, 0};
    }
    Vector3D projection = (Vector3D){v.x * dotProduct(n, v) / nDotN, v.y * dotProduct(n, v) / nDotN, v.z * dotProduct(n, v) / nDotN};
    return projection;
}

// 计算向量在由两个向量张成的平面上的投影
Vector3D projectToPlane(const Vector3D v, const Vector3D u, const Vector3D w) {
    // 计算法向量
    Vector3D n = crossProduct(u, w);

    // 计算向量v在法向量n上的投影
    Vector3D vProjN = projectVector(v, n);

    // 计算向量v在平面上的投影
    Vector3D vProjPlane = (Vector3D){v.x - vProjN.x, v.y - vProjN.y, v.z - vProjN.z};

    return vProjPlane;
}

int test()
{
    // 定义向量
    Vector3D u = {1, 0, 0};
    Vector3D w = {0, 1, 0};
    Vector3D v = {2, 3, 4};

    // 计算投影
    Vector3D vProjected = projectToPlane(v, u, w);

    // 输出结果
    printf("Projected vector to plane: (%.2f, %.2f, %.2f)\n", vProjected.x, vProjected.y, vProjected.z);

    // 计算并输出投影向量在向量u和w方向上的分量值
    Vector3D uComponent = (Vector3D){vProjected.x, 0, 0}; // 与u平行的分量
    Vector3D wComponent = (Vector3D){0, vProjected.y, 0}; // 与w平行的分量
    printf("Component along u: (%.2f, %.2f, %.2f)\n", uComponent.x, uComponent.y, uComponent.z);
    printf("Component along w: (%.2f, %.2f, %.2f)\n", wComponent.x, wComponent.y, wComponent.z);

    return 0;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("协议字节生成工具");
    w.show();
    test();
//    test_number_tytpe();
float xx[4];
qDebug()<<sizeof(xx);
    return a.exec();
}
