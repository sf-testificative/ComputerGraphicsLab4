#include "mainwindow.h"
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    QWidget* central = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_view = new GraphicsView(central);
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(m_view, 1);

    QFrame* panel = new QFrame(central);
    panel->setFrameShape(QFrame::StyledPanel);
    panel->setFrameShadow(QFrame::Raised);
    panel->setFixedWidth(240);

    QVBoxLayout* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(12, 12, 12, 12);
    panelLayout->setSpacing(12);

    QGroupBox* moveBox = new QGroupBox("Перенос (dx, dy)", panel);
    QFormLayout* moveForm = new QFormLayout(moveBox);

    m_dxSpin = new QDoubleSpinBox(moveBox);
    m_dxSpin->setRange(-1000, 1000);
    m_dxSpin->setValue(50);
    m_dxSpin->setDecimals(2);

    m_dySpin = new QDoubleSpinBox(moveBox);
    m_dySpin->setRange(-1000, 1000);
    m_dySpin->setValue(50);
    m_dySpin->setDecimals(2);

    moveForm->addRow("dx:", m_dxSpin);
    moveForm->addRow("dy:", m_dySpin);
    panelLayout->addWidget(moveBox);

    QGroupBox* rotBox = new QGroupBox("Поворот", panel);
    QFormLayout* rotForm = new QFormLayout(rotBox);

    m_angleSpin = new QDoubleSpinBox(rotBox);
    m_angleSpin->setRange(-360, 360);
    m_angleSpin->setValue(45);
    m_angleSpin->setSuffix(" °");
    m_angleSpin->setDecimals(2);

    rotForm->addRow("Угол:", m_angleSpin);
    panelLayout->addWidget(rotBox);

    QGroupBox* scaleBox = new QGroupBox("Масштаб (kx, ky)", panel);
    QFormLayout* scaleForm = new QFormLayout(scaleBox);

    m_kxSpin = new QDoubleSpinBox(scaleBox);
    m_kxSpin->setRange(0.01, 100);
    m_kxSpin->setValue(1.5);
    m_kxSpin->setSingleStep(0.1);
    m_kxSpin->setDecimals(3);

    m_kySpin = new QDoubleSpinBox(scaleBox);
    m_kySpin->setRange(0.01, 100);
    m_kySpin->setValue(1.5);
    m_kySpin->setSingleStep(0.1);
    m_kySpin->setDecimals(3);

    scaleForm->addRow("kx:", m_kxSpin);
    scaleForm->addRow("ky:", m_kySpin);
    panelLayout->addWidget(scaleBox);

    panelLayout->addStretch();

    mainLayout->addWidget(panel, 0);
    setCentralWidget(central);

    connect(m_dxSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onUpdateParams);
    connect(m_dySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onUpdateParams);
    connect(m_angleSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onUpdateParams);
    connect(m_kxSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onUpdateParams);
    connect(m_kySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onUpdateParams);

    onUpdateParams();

    QMenu* fileMenu = menuBar()->addMenu("Файл");

    QAction* clearAct = fileMenu->addAction("Очистить сцену");
    clearAct->setShortcut(QKeySequence("Ctrl+N"));
    connect(clearAct, &QAction::triggered, this, &MainWindow::onClearScene);

    fileMenu->addSeparator();

    QAction* exitAct = fileMenu->addAction("Выход");
    exitAct->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    QMenu* toolMenu = menuBar()->addMenu("Инструменты");
    QActionGroup* group = new QActionGroup(this);
    group->setExclusive(true);

    auto addTool = [&](const QString& name, Tool t, const QString& shortcut) {
        QAction* a = toolMenu->addAction(name);
        a->setCheckable(true);
        a->setShortcut(QKeySequence(shortcut));
        a->setData(static_cast<int>(t));
        group->addAction(a);
    };

    addTool("Создать полигон (ЛКМ — точка, ПКМ — завершить)", Tool::CreatePolygon, "1");
    addTool("Смещение", Tool::MovePolygon, "2");
    addTool("Поворот вокруг точки", Tool::RotatePolygonAroundPoint, "3");
    addTool("Поворот вокруг центра полигона", Tool::RotatePolygonAroundCenter, "4");
    addTool("Масштаб вокруг точки", Tool::ScalePolygonAroundPoint, "5");
    addTool("Масштаб вокруг центра полигона", Tool::ScalePolygonAroundCenter, "6");
    addTool("Пересечение двух рёбер", Tool::EdgeIntersection, "7");
    addTool("Принадлежность точки полигону", Tool::PointInPolygon, "8");
    addTool("Точка слева/справа от ребра", Tool::PointSideOfEdge, "9");

    connect(group, &QActionGroup::triggered, this, &MainWindow::onToolChanged);
    group->actions().first()->setChecked(true);
    m_view->setTool(Tool::CreatePolygon);

    m_statusLabel = new QLabel("Готово. ЛКМ — добавить точку, ПКМ — завершить полигон.", this);
    statusBar()->addWidget(m_statusLabel);

    connect(m_view, &GraphicsView::statusMessage, this, [this](const QString& msg) { m_statusLabel->setText(msg); });

    resize(1200, 800);
    setWindowTitle("Lab 4");
}

Tool MainWindow::toolFromAction(QAction* a) const {
    return static_cast<Tool>(a->data().toInt());
}

void MainWindow::onToolChanged(QAction* action) {
    m_view->setTool(toolFromAction(action));
    m_statusLabel->setText("Инструмент: " + action->text());
}

void MainWindow::onClearScene() {
    m_view->clearScene();
    m_statusLabel->setText("Сцена очищена");
}

void MainWindow::onUpdateParams() {
    m_view->setTranslation(m_dxSpin->value(), m_dySpin->value());
    m_view->setRotationAngle(m_angleSpin->value());
    m_view->setScaleFactors(m_kxSpin->value(), m_kySpin->value());
}