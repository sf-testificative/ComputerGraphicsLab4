#include "mainwindow.h"
#include <QWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>

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
    panel->setFixedWidth(300);

    QVBoxLayout* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(10, 10, 10, 10);
    panelLayout->setSpacing(10);

    QGroupBox* toolBox = new QGroupBox("Инструменты", panel);
    QVBoxLayout* toolLayout = new QVBoxLayout(toolBox);
    toolLayout->setSpacing(4);

    m_toolGroup = new QButtonGroup(this);
    m_toolGroup->setExclusive(true);

    addToolButton(toolLayout, static_cast<int>(Tool::CreatePolygon), "Создать полигон");
    addToolButton(toolLayout, static_cast<int>(Tool::MovePolygon), "Смещение");
    addToolButton(toolLayout, static_cast<int>(Tool::RotatePolygonAroundPoint), "Поворот вокруг точки");
    addToolButton(toolLayout, static_cast<int>(Tool::RotatePolygonAroundCenter), "Поворот вокруг центра полигона");
    addToolButton(toolLayout, static_cast<int>(Tool::ScalePolygonAroundPoint), "Масштаб вокруг точки");
    addToolButton(toolLayout, static_cast<int>(Tool::ScalePolygonAroundCenter), "Масштаб вокруг центра полигона");
    addToolButton(toolLayout, static_cast<int>(Tool::EdgeIntersection), "Пересечение двух рёбер");
    addToolButton(toolLayout, static_cast<int>(Tool::PointInPolygon), "Принадлежность точки полигону");
    addToolButton(toolLayout, static_cast<int>(Tool::PointSideOfEdge), "Точка слева/справа от ребра");

    connect(m_toolGroup, &QButtonGroup::idClicked, this, &MainWindow::onToolButtonClicked);

    panelLayout->addWidget(toolBox);

    QGroupBox* moveBox = new QGroupBox("Перенос", panel);
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

    QGroupBox* scaleBox = new QGroupBox("Масштаб", panel);
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

    QPushButton* clearBtn = new QPushButton("Очистить сцену", panel);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearScene);
    panelLayout->addWidget(clearBtn);

    panelLayout->addStretch();

    mainLayout->addWidget(panel, 0);
    setCentralWidget(central);

    connect(m_dxSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onUpdateParams);
    connect(m_dySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onUpdateParams);
    connect(m_angleSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onUpdateParams);
    connect(m_kxSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onUpdateParams);
    connect(m_kySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onUpdateParams);

    onUpdateParams();

    m_toolGroup->button(static_cast<int>(Tool::CreatePolygon))->setChecked(true);
    m_view->setTool(Tool::CreatePolygon);

    m_statusLabel = new QLabel("ЛКМ — добавить точку, ПКМ — завершить полигон.", this);
    statusBar()->addWidget(m_statusLabel);

    connect(m_view, &GraphicsView::statusMessage, this, [this](const QString& msg) { m_statusLabel->setText(msg); });

    resize(1280, 800);
    setWindowTitle("Lab 4");
}

void MainWindow::addToolButton(QVBoxLayout* layout, int id, const QString& text) {
    QToolButton* btn = new QToolButton(this);
    btn->setText(text);
    btn->setCheckable(true);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btn->setMinimumHeight(32);
    btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    btn->setAutoRaise(false);

    m_toolGroup->addButton(btn, id);
    layout->addWidget(btn);
}

void MainWindow::onToolButtonClicked(int id) {
    m_view->setTool(static_cast<Tool>(id));
    QToolButton* btn = qobject_cast<QToolButton*>(m_toolGroup->button(id));
    if (btn)
        m_statusLabel->setText("Инструмент: " + btn->text());
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