#ifndef POLYGON_H
#define POLYGON_H

#include <QVector>
#include <QPointF>
#include <QPolygonF>

struct Point2D {
    double x, y;
    Point2D() : x(0), y(0) {}
    Point2D(double _x, double _y) : x(_x), y(_y) {}

    QPointF toQPointF() const { return QPointF(x, y); }
};

class Polygon {
public:
    QVector<Point2D> vertices;

    Polygon() {}

    void addVertex(const Point2D& p) { vertices.append(p); }
    void clear() { vertices.clear(); }
    bool isEmpty() const { return vertices.isEmpty(); }
    int size() const { return vertices.size(); }

    bool isConvex() const;
    QPolygonF toQPolygonF() const;
};

#endif // POLYGON_H