#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QButtonGroup>
#include <QToolButton>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include "graphicsview.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onToolButtonClicked(int id);
    void onClearScene();
    void onUpdateParams();

private:
    GraphicsView* m_view = nullptr;
    QLabel* m_statusLabel = nullptr;

    QButtonGroup* m_toolGroup = nullptr;

    QDoubleSpinBox* m_dxSpin = nullptr;
    QDoubleSpinBox* m_dySpin = nullptr;
    QDoubleSpinBox* m_angleSpin = nullptr;
    QDoubleSpinBox* m_kxSpin = nullptr;
    QDoubleSpinBox* m_kySpin = nullptr;

    void addToolButton(QVBoxLayout* layout, int id, const QString& text);
};

#endif // MAINWINDOW_H