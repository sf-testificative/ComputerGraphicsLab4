#include "polygon.h"
#include <QtMath>

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