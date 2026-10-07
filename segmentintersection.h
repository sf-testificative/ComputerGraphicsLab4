#ifndef SEGMENTINTERSECTION_H
#define SEGMENTINTERSECTION_H

#include <QVector>
#include <QPainter>
#include "polygon.h"

bool segmentIntersection(const Point2D& a, const Point2D& b, const Point2D& c, const Point2D& d, Point2D& out, bool& insideSegments);

void drawEdgeIntersection(QPainter& p, bool hasFirstEdge, const Point2D& firstA, const Point2D& firstB, const QVector<Point2D>& secondEdge,
                          const Point2D& mousePos, bool hasIntersection, const Point2D& intersectionPoint);

#endif // SEGMENTINTERSECTION_H