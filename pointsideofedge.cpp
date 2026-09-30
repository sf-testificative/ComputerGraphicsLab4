#include "pointsideofedge.h"

double pointSideRelativeToEdge(const Point2D& a, const Point2D& b, const Point2D& p) {
    double ax = b.x - a.x;
    double ay = b.y - a.y;
    double bx = p.x - a.x;
    double by = p.y - a.y;
    return ax * by - bx * ay;
}