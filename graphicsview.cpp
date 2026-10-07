#include "graphicsview.h"
#include <QPainter>
#include <QStringList>
#include <cmath>

#include "pointinpolygon.h"
#include "pointsideofedge.h"
#include "polygon.h"
#include "segmentintersection.h"
#include "transformations.h"

GraphicsView::GraphicsView(QWidget* parent): QWidget(parent)
{
    setMouseTracking(true);
}

void GraphicsView::setTool(Tool t)
{
    m_tool = t;
    m_secondEdge.clear();
    m_hasIntersection = false;
    m_hasTestPoint = false;
    m_hasCenterPoint = false;
    m_hasMousePos = false;
    m_hasFirstEdge = false;
    m_hasSideEdge = false;
    update();
}

void GraphicsView::clearScene()
{
    m_polygons.clear();
    m_currentPolygon.clear();
    m_secondEdge.clear();
    m_hasIntersection = false;
    m_hasTestPoint = false;
    m_hasCenterPoint = false;
    m_hasFirstEdge = false;
    m_hasSideEdge = false;
    update();
}

void GraphicsView::finishCurrentPolygon()
{
    if (m_currentPolygon.size() >= 1)
        m_polygons.append(m_currentPolygon);
    m_currentPolygon.clear();
    update();
}

Point2D GraphicsView::toWorld(const QPoint& screenPos) const
{
    return Point2D(screenPos.x(), screenPos.y());
}

void GraphicsView::mousePressEvent(QMouseEvent* e)
{
    Point2D p = toWorld(e->pos());

    switch (m_tool) {
    case Tool::CreatePolygon:
        if (e->button() == Qt::LeftButton) {
            m_currentPolygon.addVertex(p);
            update();
        } else if (e->button() == Qt::RightButton) {
            finishCurrentPolygon();
        }
        break;

    case Tool::MovePolygon: {
        Polygon* poly = pickPolygonAt(m_polygons, p);
        if (poly) {
            applyTransform(*poly, translationMatrix(m_dx, m_dy));
            update();
        }
        break;
    }

    case Tool::RotatePolygonAroundPoint:
        if (!m_hasCenterPoint) {
            m_centerPoint = p;
            m_hasCenterPoint = true;
        } else {
            Polygon* poly = pickPolygonAt(m_polygons, p);
            if (poly) {
                applyTransform(*poly, rotationAroundPoint(qDegreesToRadians(m_angleDeg), m_centerPoint.x, m_centerPoint.y));
                update();
            }
            m_hasCenterPoint = false;
        }
        break;

    case Tool::RotatePolygonAroundCenter: {
        Polygon* poly = pickPolygonAt(m_polygons, p);
        if (poly && poly->size() > 0) {
            double cx = 0, cy = 0;
            for (const Point2D& v : poly->vertices) {
                cx += v.x;
                cy += v.y;
            }
            cx /= poly->size();
            cy /= poly->size();
            applyTransform(*poly, rotationAroundPoint(qDegreesToRadians(m_angleDeg), cx, cy));
            update();
        }
        break;
    }

    case Tool::ScalePolygonAroundPoint:
        if (!m_hasCenterPoint) {
            m_centerPoint = p;
            m_hasCenterPoint = true;
        } else {
            Polygon* poly = pickPolygonAt(m_polygons, p);
            if (poly) {
                applyTransform(*poly, scaleAroundPoint(m_kx, m_ky, m_centerPoint.x, m_centerPoint.y));
                update();
            }
            m_hasCenterPoint = false;
        }
        break;

    case Tool::ScalePolygonAroundCenter: {
        Polygon* poly = pickPolygonAt(m_polygons, p);
        if (poly && poly->size() > 0) {
            double cx = 0, cy = 0;
            for (const Point2D& v : poly->vertices) {
                cx += v.x;
                cy += v.y;
            }
            cx /= poly->size();
            cy /= poly->size();
            applyTransform(*poly, scaleAroundPoint(m_kx, m_ky, cx, cy));
            update();
        }
        break;
    }

    case Tool::EdgeIntersection: {
        if (e->button() == Qt::RightButton) {
            m_hasFirstEdge = false;
            m_secondEdge.clear();
            m_hasIntersection = false;
            update();
            break;
        }

        if (!m_hasFirstEdge) {
            Point2D a, b;
            if (pickNearestEdge(m_polygons, p, a, b)) {
                m_firstEdgeA = a;
                m_firstEdgeB = b;
                m_hasFirstEdge = true;
                emit statusMessage("Ребро выбрано. Введите второе ребро.");
            }
            update();
            break;
        }

        m_secondEdge.append(p);

        if (m_secondEdge.size() == 2) {
            bool inside = false;
            m_hasIntersection = segmentIntersection(
                m_firstEdgeA, m_firstEdgeB,
                m_secondEdge[0], m_secondEdge[1],
                m_intersectionPoint, inside);
            if (m_hasIntersection) {
                emit statusMessage(QString("Пересечение: (%1, %2)").arg(m_intersectionPoint.x, 0, 'f', 2).arg(m_intersectionPoint.y, 0, 'f', 2));
            } else {
                emit statusMessage("Прямые параллельны или совпадают");
            }
        } else if (m_secondEdge.size() > 2) {
            m_secondEdge.clear();
            m_secondEdge.append(p);
            m_hasIntersection = false;
        }
        update();
        break;
    }

    case Tool::PointInPolygon: {
        QStringList results;
        for (int i = 0; i < m_polygons.size(); ++i)
            if (pointInPolygon(m_polygons[i], p))
                results << QString("Полигон #%1 (%2)").arg(i + 1).arg(m_polygons[i].isConvex() ? "выпуклый" : "невыпуклый");
        emit statusMessage(results.isEmpty() ? "Точка не принадлежит ни одному полигону" : "Точка внутри: " + results.join(", "));
        m_testPoint = p;
        m_hasTestPoint = true;
        update();
        break;
    }

    case Tool::PointSideOfEdge: {
        if (e->button() == Qt::RightButton) {
            Point2D a, b;
            if (pickNearestEdge(m_polygons, p, a, b)) {
                m_sideEdgeA = a;
                m_sideEdgeB = b;
                m_hasSideEdge = true;
                emit statusMessage("Ребро выбрано. Кликните точку.");
            } else {
                emit statusMessage("Ребро не найдено. Кликните ближе к ребру полигона.");
            }
            update();
            break;
        }

        if (!m_hasSideEdge) {
            emit statusMessage("Сначала выберите ребро правой кнопкой мыши.");
            break;
        }

        double s = pointSideRelativeToEdge(m_sideEdgeA, m_sideEdgeB, p);
        QString side = (s > 0) ? "СЛЕВА" : (s < 0 ? "СПРАВА" : "НА ПРЯМОЙ");
        emit statusMessage(QString("Точка %1 относительно ребра (s = %2)").arg(side).arg(s, 0, 'f', 2));
        update();
        break;
    }
    }
}

void GraphicsView::mouseMoveEvent(QMouseEvent* e)
{
    m_mousePos = toWorld(e->pos());
    m_hasMousePos = true;
    update();
}

void GraphicsView::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), Qt::white);

    for (const Polygon& poly : m_polygons) {
        drawPolygon(p, poly, QColor(0, 100, 200), true);
    }

    if (!m_currentPolygon.isEmpty()) {
        drawPolygon(p, m_currentPolygon, QColor(0, 180, 0), false);
        if (m_hasMousePos && !m_currentPolygon.vertices.isEmpty()) {
            p.setPen(QPen(QColor(0, 180, 0, 120), 1, Qt::DashLine));
            p.drawLine(m_currentPolygon.vertices.last().toQPointF(), m_mousePos.toQPointF());
        }
    }

    if (m_tool == Tool::EdgeIntersection) {
        drawEdgeIntersection(p, m_hasFirstEdge, m_firstEdgeA, m_firstEdgeB, m_secondEdge, m_mousePos, m_hasIntersection, m_intersectionPoint);
    }

    if (m_tool == Tool::PointSideOfEdge && m_hasSideEdge) {
        drawEdgeWithArrow(p, m_sideEdgeA, m_sideEdgeB, Qt::darkGreen);
    }
}