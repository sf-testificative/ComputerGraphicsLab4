#include "graphicsview.h"
#include <QPainter>
#include <QtMath>
#include <QStringList>

#include "segmentintersection.h"
#include "pointinpolygon.h"
#include "pointsideofedge.h"

GraphicsView::GraphicsView(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);
}

void GraphicsView::setTool(Tool t) {
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

void GraphicsView::clearScene() {
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

void GraphicsView::finishCurrentPolygon() {
    if (m_currentPolygon.size() >= 1)
        m_polygons.append(m_currentPolygon);
    m_currentPolygon.clear();
    update();
}

Point2D GraphicsView::toWorld(const QPoint& screenPos) const {
    return Point2D(screenPos.x() - m_offset.x(),
                   screenPos.y() - m_offset.y());
}

bool GraphicsView::pickEdgeAt(const Point2D& p, Point2D& a, Point2D& b) const {
    Polygon* poly = nullptr;
    for (int i = m_polygons.size() - 1; i >= 0; --i) {
        if (pointInPolygon(m_polygons[i], p)) {
            poly = const_cast<Polygon*>(&m_polygons[i]);
            break;
        }
    }
    if (!poly || poly->size() < 2) return false;

    int n = poly->size();
    double bestDist = 1e18;
    int bestIdx = -1;
    for (int i = 0; i < n; ++i) {
        const Point2D& v1 = poly->vertices[i];
        const Point2D& v2 = poly->vertices[(i + 1) % n];

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
        if (dist < bestDist) {
            bestDist = dist;
            bestIdx = i;
        }
    }
    if (bestIdx < 0) return false;
    a = poly->vertices[bestIdx];
    b = poly->vertices[(bestIdx + 1) % n];
    return true;
}

Polygon* GraphicsView::pickPolygonAt(const Point2D& p) {
    for (int i = m_polygons.size() - 1; i >= 0; --i)
        if (pointInPolygon(m_polygons[i], p))
            return &m_polygons[i];
    return nullptr;
}

void GraphicsView::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::MiddleButton) {
        m_panning = true;
        m_panStart = e->pos();
        setCursor(Qt::ClosedHandCursor);
        return;
    }

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
        Polygon* poly = pickPolygonAt(p);
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
            Polygon* poly = pickPolygonAt(p);
            if (poly) {
                applyTransform(*poly,
                    rotationAroundPoint(qDegreesToRadians(m_angleDeg),
                                        m_centerPoint.x, m_centerPoint.y));
                update();
            }
            m_hasCenterPoint = false;
        }
        break;

    case Tool::RotatePolygonAroundCenter: {
        Polygon* poly = pickPolygonAt(p);
        if (poly && poly->size() > 0) {
            double cx = 0, cy = 0;
            for (const Point2D& v : poly->vertices) { cx += v.x; cy += v.y; }
            cx /= poly->size(); cy /= poly->size();
            applyTransform(*poly,
                rotationAroundPoint(qDegreesToRadians(m_angleDeg), cx, cy));
            update();
        }
        break;
    }

    case Tool::ScalePolygonAroundPoint:
        if (!m_hasCenterPoint) {
            m_centerPoint = p;
            m_hasCenterPoint = true;
        } else {
            Polygon* poly = pickPolygonAt(p);
            if (poly) {
                applyTransform(*poly,
                    scaleAroundPoint(m_kx, m_ky, m_centerPoint.x, m_centerPoint.y));
                update();
            }
            m_hasCenterPoint = false;
        }
        break;

    case Tool::ScalePolygonAroundCenter: {
        Polygon* poly = pickPolygonAt(p);
        if (poly && poly->size() > 0) {
            double cx = 0, cy = 0;
            for (const Point2D& v : poly->vertices) { cx += v.x; cy += v.y; }
            cx /= poly->size(); cy /= poly->size();
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
            if (pickEdgeAt(p, a, b)) {
                m_firstEdgeA = a;
                m_firstEdgeB = b;
                m_hasFirstEdge = true;
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
            if (m_hasIntersection)
                emit statusMessage(QString("Пересечение: (%1, %2)")
                    .arg(m_intersectionPoint.x, 0, 'f', 2)
                    .arg(m_intersectionPoint.y, 0, 'f', 2));
            else
                emit statusMessage("Прямые параллельны или совпадают");
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
                results << QString("Полигон #%1 (%2)").arg(i + 1)
                               .arg(m_polygons[i].isConvex() ? "выпуклый" : "невыпуклый");
        emit statusMessage(results.isEmpty()
            ? "Точка не принадлежит ни одному полигону"
            : "Точка внутри: " + results.join(", "));
        m_testPoint = p;
        m_hasTestPoint = true;
        update();
        break;
    }

    case Tool::PointSideOfEdge: {
        if (e->button() == Qt::RightButton) {
            Point2D a, b;
            if (pickEdgeAt(p, a, b)) {
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
        QString side = (s > 0) ? "СЛЕВА"
                      : (s < 0 ? "СПРАВА" : "НА ПРЯМОЙ");
        emit statusMessage(QString("Точка %1 относительно ребра (s = %2)")
                           .arg(side).arg(s, 0, 'f', 2));
        update();
        break;
    }
    }
}

void GraphicsView::mouseMoveEvent(QMouseEvent* e) {
    if (m_panning) {
        QPoint delta = e->pos() - m_panStart;
        m_panStart = e->pos();
        m_offset += QPointF(delta.x(), delta.y());
        update();
        return;
    }

    m_mousePos = toWorld(e->pos());
    m_hasMousePos = true;

    update();
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button() == Qt::MiddleButton) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
    }
}

void GraphicsView::drawPolygon(QPainter& p, const Polygon& poly,
                               const QColor& color, bool close) {
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

void GraphicsView::paintEvent(QPaintEvent* e) {
    Q_UNUSED(e);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), Qt::white);
    p.translate(m_offset);

    QRectF visibleWorld(-m_offset.x(), -m_offset.y(), width(), height());

    p.setPen(QPen(QColor(230, 230, 230), 1));
    int step = 50;
    int left   = (int)std::floor(visibleWorld.left()   / step) * step;
    int right  = (int)std::ceil (visibleWorld.right()  / step) * step;
    int top    = (int)std::floor(visibleWorld.top()    / step) * step;
    int bottom = (int)std::ceil (visibleWorld.bottom() / step) * step;
    for (int x = left; x <= right; x += step) p.drawLine(x, top, x, bottom);
    for (int y = top; y <= bottom; y += step) p.drawLine(left, y, right, y);

    p.setPen(QPen(Qt::black, 1));
    p.drawLine(left, 0, right, 0);
    p.drawLine(0, top, 0, bottom);

    for (const Polygon& poly : m_polygons) {
        QColor c = poly.isConvex() ? QColor(0, 100, 200)
                                   : QColor(200, 100, 0);
        drawPolygon(p, poly, c, true);
    }

    if (!m_currentPolygon.isEmpty()) {
        drawPolygon(p, m_currentPolygon, QColor(0, 180, 0), false);
        if (m_hasMousePos && !m_currentPolygon.vertices.isEmpty()) {
            p.setPen(QPen(QColor(0, 180, 0, 120), 1, Qt::DashLine));
            p.drawLine(m_currentPolygon.vertices.last().toQPointF(),
                       m_mousePos.toQPointF());
        }
    }

    if (m_tool == Tool::EdgeIntersection) {
        if (m_hasFirstEdge) {
            p.setPen(QPen(Qt::magenta, 3));
            p.drawLine(m_firstEdgeA.toQPointF(), m_firstEdgeB.toQPointF());

            p.setPen(QPen(Qt::blue, 2));
            if (m_secondEdge.size() == 1) {
                p.drawLine(m_secondEdge[0].toQPointF(), m_mousePos.toQPointF());
                p.setBrush(Qt::blue);
                p.drawEllipse(m_secondEdge[0].toQPointF(), 5, 5);
            } else if (m_secondEdge.size() == 2) {
                p.drawLine(m_secondEdge[0].toQPointF(),
                           m_secondEdge[1].toQPointF());
                p.setBrush(Qt::blue);
                p.drawEllipse(m_secondEdge[0].toQPointF(), 5, 5);
                p.drawEllipse(m_secondEdge[1].toQPointF(), 5, 5);
            }

            if (m_hasIntersection) {
                p.setPen(QPen(Qt::red, 3));
                p.setBrush(Qt::yellow);
                p.drawEllipse(m_intersectionPoint.toQPointF(), 7, 7);
            }
        }
    }

    if (m_tool == Tool::PointSideOfEdge && m_hasSideEdge) {
        p.setPen(QPen(Qt::darkGreen, 3));
        p.drawLine(m_sideEdgeA.toQPointF(), m_sideEdgeB.toQPointF());
    }
}