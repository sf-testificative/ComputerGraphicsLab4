#include "pointsideofedge.h"
#include <QPolygonF>
#include <QtMath>
#include <cmath>

double pointSideRelativeToEdge(const Point2D& a, const Point2D& b, const Point2D& p) {
    double ax = b.x - a.x;
    double ay = b.y - a.y;
    double bx = p.x - a.x;
    double by = p.y - a.y;
    return bx * ay - ax * by;
}

bool pickNearestEdge(const QVector<Polygon>& polys, const Point2D& p, Point2D& a, Point2D& b, double maxDist) {
    double bestDist = 1e18;
    Point2D bestA, bestB;
    bool found = false;

    for (const Polygon& poly : polys) {
        int n = poly.size();
        if (n < 2) continue;

        for (int i = 0; i < n; ++i) {
            const Point2D& v1 = poly.vertices[i];
            const Point2D& v2 = poly.vertices[(i + 1) % n];

            double dx = v2.x - v1.x;
            double dy = v2.y - v1.y;
            double len2 = dx * dx + dy * dy;
            if (len2 < 1e-12) continue;

            double t = ((p.x - v1.x) * dx + (p.y - v1.y) * dy) / len2;
            if (t < 0) t = 0;
            if (t > 1) t = 1;
            double px = v1.x + t * dx;
            double py = v1.y + t * dy;
            double dist = (p.x - px) * (p.x - px) + (p.y - py) * (p.y - py);
                if (dist < bestDist && dist <= maxDist * maxDist) {
                bestDist = dist;
                bestA = v1;
                bestB = v2;
                found = true;
            }
        }
    }
    if (!found) return false;
    a = bestA;
    b = bestB;
    return true;
}

void drawEdgeWithArrow(QPainter& p, const Point2D& a, const Point2D& b, const QColor& color) {
    p.setPen(QPen(color, 3));
    p.drawLine(a.toQPointF(), b.toQPointF());

    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double len = std::sqrt(dx * dx + dy * dy);
    if (len < 1e-6) return;

    double ux = dx / len;
    double uy = dy / len;
    double px = -uy;
    double py = ux;

    double midX = (a.x + b.x) / 2.0;
    double midY = (a.y + b.y) / 2.0;

    double arrowLen   = 16.0;
    double arrowWidth = 7.0;

    QPointF tip(midX + ux * arrowLen, midY + uy * arrowLen);
    QPointF left(midX - ux * arrowLen + px * arrowWidth, midY - uy * arrowLen + py * arrowWidth);
    QPointF right(midX - ux * arrowLen - px * arrowWidth, midY - uy * arrowLen - py * arrowWidth);

    QPolygonF arrow;
    arrow << tip << left << right;
    p.setBrush(color);
    p.setPen(QPen(color, 2));
    p.drawPolygon(arrow);
}