#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QWidget>  // 基类是 QWidget
#include <QPixmap>
#include <QPainter>
#include <QLinearGradient>

class BaseWindow : public QWidget  // 继承 QWidget
{
    Q_OBJECT

public:
    explicit BaseWindow(QWidget *parent = nullptr);

    void setBackgroundImage(const QString& imagePath);
    void setGradientBackground(const QColor& color1, const QColor& color2);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QPixmap m_backgroundPixmap;
    bool m_useImage = false;
    bool m_useGradient = true;
    QColor m_gradColor1;
    QColor m_gradColor2;
};

#endif