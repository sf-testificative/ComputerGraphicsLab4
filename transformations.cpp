#include "transformations.h"
#include <QtMath>

QGenericMatrix<3, 3, float> translationMatrix(double dx, double dy) {
    float v[9] = { 1, 0, 0,
                  0, 1, 0,
                  (float)dx, (float)dy, 1 };
    return QGenericMatrix<3, 3, float>(v);
}

QGenericMatrix<3, 3, float> rotationMatrix(double phiRad) {
    float c = (float)qCos(phiRad);
    float s = (float)qSin(phiRad);
    float v[9] = {  c,  s, 0,
                  -s,  c, 0,
                  0,  0, 1 };
    return QGenericMatrix<3, 3, float>(v);
}

QGenericMatrix<3, 3, float> scaleMatrix(double x, double y) {
    float v[9] = { (float)x, 0, 0,
                  0, (float)y, 0,
                  0, 0, 1 };
    return QGenericMatrix<3, 3, float>(v);
}

QGenericMatrix<3, 3, float> multiply(const QGenericMatrix<3, 3, float>& A, const QGenericMatrix<3, 3, float>& B) {
    QGenericMatrix<3, 3, float> R;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            float sum = 0;
            for (int k = 0; k < 3; ++k)
                sum += A(i, k) * B(k, j);
            R(i, j) = sum;
        }
    return R;
}

QGenericMatrix<3, 3, float> rotationAroundPoint(double phiRad, double a, double b) {
    return multiply(
        multiply(translationMatrix(-a, -b), rotationMatrix(phiRad)),
        translationMatrix(a, b));
}

QGenericMatrix<3, 3, float> scaleAroundPoint(double kx, double ky, double a, double b) {
    return multiply(
        multiply(translationMatrix(-a, -b), scaleMatrix(kx, ky)),
        translationMatrix(a, b));
}

void applyTransform(Polygon& poly, const QGenericMatrix<3, 3, float>& M) {
    for (Point2D& p : poly.vertices) {
        float x = (float)p.x, y = (float)p.y, w = 1.0f;
        float nx = x * M(0,0) + y * M(1,0) + w * M(2,0);
        float ny = x * M(0,1) + y * M(1,1) + w * M(2,1);
        float nw = x * M(0,2) + y * M(1,2) + w * M(2,2);
        if (qAbs(nw) > 1e-9) { nx /= nw; ny /= nw; }
        p.x = nx; p.y = ny;
    }
}