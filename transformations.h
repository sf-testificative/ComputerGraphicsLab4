#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include <QGenericMatrix>
#include "polygon.h"

using Mat3 = QGenericMatrix<3, 3, float>;

QGenericMatrix<3, 3, float> translationMatrix(double dx, double dy);
QGenericMatrix<3, 3, float> rotationMatrix(double phiRad);
QGenericMatrix<3, 3, float> scaleMatrix(double kx, double ky);

QGenericMatrix<3, 3, float> multiply(const QGenericMatrix<3, 3, float>& A, const QGenericMatrix<3, 3, float>& B);

QGenericMatrix<3, 3, float> rotationAroundPoint(double phiRad, double a, double b);

QGenericMatrix<3, 3, float> scaleAroundPoint(double kx, double ky, double a, double b);

void applyTransform(Polygon& poly, const QGenericMatrix<3, 3, float>& M);

#endif