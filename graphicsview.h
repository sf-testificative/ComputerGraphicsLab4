#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QVector>
#include "polygon.h"
#include "transformations.h"

enum class Tool {
    CreatePolygon,
    MovePolygon,
    RotatePolygonAroundPoint,
    RotatePolygonAroundCenter,
    ScalePolygonAroundPoint,
    ScalePolygonAroundCenter,
    EdgeIntersection,
    PointInPolygon,
    PointSideOfEdge
};

class GraphicsView : public QWidget {
    Q_OBJECT
public:
    explicit GraphicsView(QWidget* parent = nullptr);

    void setTool(Tool t);
    void clearScene();

    void setTranslation(double dx, double dy) { m_dx = dx; m_dy = dy; }
    void setRotationAngle(double deg) { m_angleDeg = deg; }
    void setScaleFactors(double kx, double ky) { m_kx = kx; m_ky = ky; }

signals:
    void statusMessage(const QString& msg);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private:
    Tool m_tool = Tool::CreatePolygon;

    QVector<Polygon> m_polygons;
    Polygon m_currentPolygon;

    Point2D m_testPoint;
    bool m_hasTestPoint = false;

    bool m_hasFirstEdge = false;
    Point2D m_firstEdgeA;
    Point2D m_firstEdgeB;
    QVector<Point2D> m_secondEdge;
    bool m_hasIntersection = false;
    Point2D m_intersectionPoint;

    bool m_hasSideEdge = false;
    Point2D m_sideEdgeA;
    Point2D m_sideEdgeB;

    Point2D m_centerPoint;
    bool m_hasCenterPoint = false;

    Point2D m_mousePos;
    bool m_hasMousePos = false;

    double m_dx = 50, m_dy = 50;
    double m_angleDeg = 45;
    double m_kx = 1.5, m_ky = 1.5;

    QPointF m_offset{0, 0};

    bool m_panning = false;
    QPoint m_panStart;

    void drawPolygon(QPainter& p, const Polygon& poly,
                     const QColor& color, bool close);
    Polygon* pickPolygonAt(const Point2D& p);
    void finishCurrentPolygon();

    Point2D toWorld(const QPoint& screenPos) const;

    bool pickEdgeAt(const Point2D& p, Point2D& a, Point2D& b) const;
};

#endif // GRAPHICSVIEW_H