#ifndef SEGMENTINTERSECTION_H
#define SEGMENTINTERSECTION_H

#include "polygon.h"

bool segmentIntersection(const Point2D& a, const Point2D& b, const Point2D& c, const Point2D& d, Point2D& out, bool& insideSegments);

#endif // SEGMENTINTERSECTION_H