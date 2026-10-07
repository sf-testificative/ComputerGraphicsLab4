#ifndef POINTSIDEOFEDGE_H
#define POINTSIDEOFEDGE_H

#include <QVector>
#include <QPainter>
#include <QColor>
#include "polygon.h"

double pointSideRelativeToEdge(const Point2D& a, const Point2D& b, const Point2D& p);

bool pickNearestEdge(const QVector<Polygon>& polys, const Point2D& p, Point2D& a, Point2D& b, double maxDist = 30.0);

void drawEdgeWithArrow(QPainter& p, const Point2D& a, const Point2D& b, const QColor& color);

#endif // POINTSIDEOFEDGE_H