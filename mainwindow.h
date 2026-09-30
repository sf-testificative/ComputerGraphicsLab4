#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QActionGroup>
#include <QDoubleSpinBox>
#include "graphicsview.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onToolChanged(QAction* action);
    void onClearScene();
    void onUpdateParams();

private:
    GraphicsView* m_view = nullptr;
    QLabel* m_statusLabel = nullptr;

    QDoubleSpinBox* m_dxSpin = nullptr;
    QDoubleSpinBox* m_dySpin = nullptr;
    QDoubleSpinBox* m_angleSpin = nullptr;
    QDoubleSpinBox* m_kxSpin = nullptr;
    QDoubleSpinBox* m_kySpin = nullptr;

    Tool toolFromAction(QAction* a) const;
};

#endif // MAINWINDOW_H