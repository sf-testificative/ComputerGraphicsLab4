#ifndef POINTSIDEOFEDGE_H
#define POINTSIDEOFEDGE_H

#include <QVector>
#include "polygon.h"

double pointSideRelativeToEdge(const Point2D& a, const Point2D& b, const Point2D& p);

bool pickNearestEdge(const QVector<Polygon>& polys, const Point2D& p, Point2D& a, Point2D& b, double maxDist = 30.0);

#endif // POINTSIDEOFEDGE_H