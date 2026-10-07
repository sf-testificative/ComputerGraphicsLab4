#include "polygon.h"
#include <QtMath>
#include <cmath>

QPolygonF Polygon::toQPolygonF() const {
    QPolygonF poly;
    for (const Point2D& p : vertices)
        poly << p.toQPointF();
    return poly;
}

bool Polygon::isConvex() const {
    int n = vertices.size();
    if (n < 3) return true;

    int sign = 0;
    for (int i = 0; i < n; ++i) {
        const Point2D& a = vertices[i];
        const Point2D& b = vertices[(i + 1) % n];
        const Point2D& c = vertices[(i + 2) % n];

        double cross = (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
        if (qAbs(cross) < 1e-9) continue;

        int s = (cross > 0) ? 1 : -1;
        if (sign == 0) sign = s;
        else if (sign != s) return false;
    }
    return true;
}

double distToSegment(const Point2D& p, const Point2D& a, const Point2D& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double len2 = dx * dx + dy * dy;
    if (len2 < 1e-12) {
        double ddx = p.x - a.x;
        double ddy = p.y - a.y;
        return std::sqrt(ddx * ddx + ddy * ddy);
    }
    double t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / len2;
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    double px = a.x + t * dx;
    double py = a.y + t * dy;
    double ddx = p.x - px;
    double ddy = p.y - py;
    return std::sqrt(ddx * ddx + ddy * ddy);
}

void drawPolygon(QPainter& p, const Polygon& poly, const QColor& color, bool close) {
    if (poly.isEmpty()) return;
    p.setPen(QPen(color, 2));
    p.setBrush(Qt::NoBrush);

    if (poly.size() == 1) {
        p.setBrush(color);
        p.drawEllipse(poly.vertices[0].toQPointF(), 4, 4);
        return;
    }

    QPolygonF q = poly.toQPolygonF();
    if (poly.size() >= 3 && close) p.drawPolygon(q);
    else p.drawPolyline(q);

    p.setBrush(Qt::red);
    p.setPen(QPen(Qt::red, 1));
    for (const Point2D& v : poly.vertices)
        p.drawEllipse(v.toQPointF(), 3, 3);
}