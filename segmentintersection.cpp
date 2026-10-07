#include "segmentintersection.h"
#include <QtMath>

bool segmentIntersection(const Point2D& a, const Point2D& b, const Point2D& c, const Point2D& d, Point2D& out, bool& insideSegments) {
    double nx = -(d.y - c.y);
    double ny =  (d.x - c.x);

    double denom = nx * (b.x - a.x) + ny * (b.y - a.y);
    if (qAbs(denom) < 1e-9) {
        insideSegments = false;
        return false;
    }

    double t = -(nx * (a.x - c.x) + ny * (a.y - c.y)) / denom;

    out.x = a.x + t * (b.x - a.x);
    out.y = a.y + t * (b.y - a.y);
    return true;
}