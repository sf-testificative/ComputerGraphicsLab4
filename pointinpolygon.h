#ifndef POINTINPOLYGON_H
#define POINTINPOLYGON_H

#include <QVector>
#include "polygon.h"

bool pointInPolygon(const Polygon& poly, const Point2D& p);

Polygon* pickPolygonAt(const QVector<Polygon>& polygons, const Point2D& p, double hitRadius = 10.0);

#endif // POINTINPOLYGON_H