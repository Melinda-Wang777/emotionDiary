#include "basewindow.h"
#include <QResizeEvent>
#include <QRandomGenerator>

BaseWindow::BaseWindow(QWidget *parent)
    : QWidget(parent)  // 调用 QWidget 的构造函数
{
    setAttribute(Qt::WA_StyledBackground, true);
    // 默认极简水彩渐变
    m_gradColor1 = QColor(248, 245, 240);
    m_gradColor2 = QColor(235, 225, 215);
}

void BaseWindow::setBackgroundImage(const QString& imagePath)
{
    m_backgroundPixmap.load(imagePath);
    if (!m_backgroundPixmap.isNull()) {
        m_useImage = true;
        m_useGradient = false;
        update();
    }
}

void BaseWindow::setGradientBackground(const QColor& color1, const QColor& color2)
{
    m_gradColor1 = color1;
    m_gradColor2 = color2;
    m_useImage = false;
    m_useGradient = true;
    update();
}

void BaseWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_useImage && !m_backgroundPixmap.isNull()) {
        QPixmap scaled = m_backgroundPixmap.scaled(
            size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        painter.drawPixmap(rect(), scaled);
    } else if (m_useGradient) {
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, m_gradColor1);
        gradient.setColorAt(1, m_gradColor2);
        painter.fillRect(rect(), gradient);
    }
}

void BaseWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    update();
}