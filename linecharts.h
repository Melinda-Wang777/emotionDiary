#ifndef LINECHARTS_H
#define LINECHARTS_H

#include <QWidget>
#include <QVector3D>
#include <QComboBox>
#include <QLabel>
#include <QVector>
#include <QPair>
#include <QString>
#include <QSpinBox>
#include "basewindow.h"

class MainWindow;

class LineChartsWindow : public BaseWindow
{
    Q_OBJECT
public:
    explicit LineChartsWindow(MainWindow *mw, QWidget *parent = nullptr);
    ~LineChartsWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onDimensionChanged(int index);
    void refreshData();

private:
    void calculateLayout();
    QPointF getPoint(int index, int dimension) const;
    void updateTitle();
    void drawLegend(QPainter &painter, int yOffset);  // 新增：绘制图例

    MainWindow *m_mainwindow;
    QVector<QPair<QString, QVector3D>> m_data;
    QComboBox *m_dimensionCombo;
    QSpinBox *m_yearSpin;
    QComboBox *m_monthCombo;
    QLabel *m_titleLabel;

    // 绘图边距
    int m_leftMargin = 60;
    int m_rightMargin = 40;
    int m_topMargin = 80;
    int m_bottomMargin = 60;

    int m_currentDimension = 0;  // 0:P, 1:A, 2:D, 3:ALL

    int m_chartWidth = 0;
    int m_chartHeight = 0;
};

#endif // LINECHARTS_H