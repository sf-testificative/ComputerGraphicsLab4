#include "segmentintersection.h"
#include <QtMath>

bool segmentIntersection(const Point2D& a, const Point2D& b,
                         const Point2D& c, const Point2D& d,
                         Point2D& out, bool& insideSegments) {
    double nx = -(d.y - c.y);
    double ny =  (d.x - c.x);

    double denom = nx * (b.x - a.x) + ny * (b.y - a.y);
    if (qAbs(denom) < 1e-9) {
        insideSegments = false;
        return false;
    }

    double t = -(nx * (a.x - c.x) + ny * (a.y - c.y)) / denom;

    double ux = -(b.y - a.y);
    double uy =  (b.x - a.x);
    double denom2 = ux * (d.x - c.x) + uy * (d.y - c.y);

    double u = 0;
    if (qAbs(denom2) > 1e-9)
        u = -(ux * (c.x - a.x) + uy * (c.y - a.y)) / denom2;

    out.x = a.x + t * (b.x - a.x);
    out.y = a.y + t * (b.y - a.y);

    insideSegments = (t >= -1e-9 && t <= 1 + 1e-9 &&
                      u >= -1e-9 && u <= 1 + 1e-9);
    return true;
}