#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include <QGenericMatrix>
#include "polygon.h"

using Mat3 = QGenericMatrix<3, 3, float>;

Mat3 translationMatrix(double dx, double dy);
Mat3 rotationMatrix(double phiRad);
Mat3 scaleMatrix(double kx, double ky);

Mat3 multiply(const Mat3& A, const Mat3& B);

Mat3 rotationAroundPoint(double phiRad, double a, double b);

Mat3 scaleAroundPoint(double kx, double ky, double a, double b);

void applyTransform(Polygon& poly, const Mat3& M);

#endif