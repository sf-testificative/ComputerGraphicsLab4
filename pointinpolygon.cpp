#include "pointinpolygon.h"
#include <QtMath>
#include "pointsideofedge.h"

bool pointInPolygon(const Polygon& poly, const Point2D& p) {
    int n = poly.size();

    if (n == 1) {
        return qAbs(poly.vertices[0].x - p.x) < 1e-6 && qAbs(poly.vertices[0].y - p.y) < 1e-6;
    }

    if (n == 2) {
        double s = pointSideRelativeToEdge(poly.vertices[0], poly.vertices[1], p);
        if (qAbs(s) > 1e-6) return false;
        double minX = qMin(poly.vertices[0].x, poly.vertices[1].x);
        double maxX = qMax(poly.vertices[0].x, poly.vertices[1].x);
        double minY = qMin(poly.vertices[0].y, poly.vertices[1].y);
        double maxY = qMax(poly.vertices[0].y, poly.vertices[1].y);
        return p.x >= minX - 1e-6 && p.x <= maxX + 1e-6 && p.y >= minY - 1e-6 && p.y <= maxY + 1e-6;
    }

    if (poly.isConvex()) {
        int sign = 0;
        for (int i = 0; i < n; ++i) {
            double s = pointSideRelativeToEdge(
                poly.vertices[i], poly.vertices[(i + 1) % n], p);
            if (qAbs(s) < 1e-9) continue;
            int sg = (s > 0) ? 1 : -1;
            if (sign == 0) sign = sg;
            else if (sign != sg) return false;
        }
        return true;
    }

    bool inside = false;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        const Point2D& vi = poly.vertices[i];
        const Point2D& vj = poly.vertices[j];
        if (((vi.y > p.y) != (vj.y > p.y)) &&
            (p.x < (vj.x - vi.x) * (p.y - vi.y) / (vj.y - vi.y + 1e-12) + vi.x)) {
            inside = !inside;
        }
    }
    return inside;
}

Polygon* pickPolygonAt(const QVector<Polygon>& polygons, const Point2D& p, double hitRadius) {
    for (int i = polygons.size() - 1; i >= 0; --i) {
        const Polygon& poly = polygons[i];

        if (poly.size() == 0) continue;

        if (poly.size() == 1) {
            double dx = p.x - poly.vertices[0].x;
            double dy = p.y - poly.vertices[0].y;
            if (std::sqrt(dx * dx + dy * dy) <= hitRadius)
                return const_cast<Polygon*>(&poly);
            continue;
        }

        if (poly.size() == 2) {
            if (distToSegment(p, poly.vertices[0], poly.vertices[1]) <= hitRadius)
                return const_cast<Polygon*>(&poly);
            continue;
        }

        if (pointInPolygon(poly, p))
            return const_cast<Polygon*>(&poly);

        for (int j = 0; j < poly.size(); ++j) {
            const Point2D& a = poly.vertices[j];
            const Point2D& b = poly.vertices[(j + 1) % poly.size()];
            if (distToSegment(p, a, b) <= hitRadius)
                return const_cast<Polygon*>(&poly);
        }
    }
    return nullptr;
}