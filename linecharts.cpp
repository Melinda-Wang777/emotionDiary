#include "linecharts.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QDate>
#include <QtMath>
#include "basewindow.h"

LineChartsWindow::LineChartsWindow(MainWindow *mw, QWidget *parent)
    : BaseWindow(parent), m_mainwindow(mw)
{
    setWindowTitle("情绪趋势分析");
    resize(900, 550);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *controlLayout = new QHBoxLayout();

    QLabel *dimensionLabel = new QLabel("选择维度：", this);

    m_dimensionCombo = new QComboBox(this);
    m_dimensionCombo->addItem("愉悦度（P） 难过 ← → 开心");
    m_dimensionCombo->addItem("唤醒度（A） 平静 ← → 兴奋");
    m_dimensionCombo->addItem("支配度（D） 无力 ← → 掌控");
    m_dimensionCombo->addItem("全部维度（P+A+D） 对比显示");

    QLabel *yearLabel = new QLabel("年份:", this);

    m_yearSpin = new QSpinBox(this);
    m_yearSpin->setRange(2000, 2100);
    m_yearSpin->setValue(QDate::currentDate().year());
    m_yearSpin->setFixedWidth(100);

    QLabel *monthLabel = new QLabel("月份:", this);

    m_monthCombo = new QComboBox(this);
    for (int m = 1; m <= 12; ++m) {
        m_monthCombo->addItem(QString("%1月").arg(m), m);
    }
    m_monthCombo->setCurrentIndex(QDate::currentDate().month() - 1);
    m_monthCombo->setFixedWidth(100);

    QPushButton *refreshBtn = new QPushButton("刷 新", this);
    refreshBtn->setFixedWidth(65);
    refreshBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: black;"
        "  color: #FFFFFF;"
        "  border: 2px solid #2C2C2C;"
        "  font-family: 'SimSun';"
        "  font-size: 16px;"
        "  font-weight: normal;"
        "  padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(44, 44, 44, 0.9);"
        "}"
        );

    dimensionLabel->setStyleSheet("font-family: 'SimSun'; font-size: 14px; color: #2C2C2C; font-weight: bold; background: transparent;");
    yearLabel->setStyleSheet("font-family: 'SimSun'; font-size: 14px; color: #2C2C2C; font-weight: bold; background: transparent;");
    monthLabel->setStyleSheet("font-family: 'SimSun'; font-size: 14px; color: #2C2C2C; font-weight: bold; background: transparent;");

    m_dimensionCombo->setStyleSheet(
        "QComboBox {"
        "  border: 1px solid #2C2C2C;"
        "  padding: 4px 8px;"
        "  background-color: white;"
        "  color: #333;"
        "  font-size: 12px;"
        "}"
        );

    m_yearSpin->setStyleSheet(
        "QSpinBox {"
        "  border: 1px solid #2C2C2C;"
        "  padding: 4px 8px;"
        "  background-color: white;"
        "  color: #333;"
        "  font-size: 12px;"
        "}"
        );

    m_monthCombo->setStyleSheet(
        "QComboBox {"
        "  border: 1px solid #2C2C2C;"
        "  padding: 4px 8px;"
        "  background-color: white;"
        "  color: #333;"
        "  font-size: 12px;"
        "}"
        );

    controlLayout->addWidget(dimensionLabel);
    controlLayout->addWidget(m_dimensionCombo);
    controlLayout->addWidget(yearLabel);
    controlLayout->addWidget(m_yearSpin);
    controlLayout->addWidget(monthLabel);
    controlLayout->addWidget(m_monthCombo);
    controlLayout->addWidget(refreshBtn);
    controlLayout->addStretch();

    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(
        "font-family: 'SimSun';"
        "font-size: 16px;"
        "font-weight: bold;"
        "color: #2C2C2C;"
        "background-color: rgba(255, 255, 255, 0);"
        "padding: 6px;"
        );

    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addStretch();

    connect(m_dimensionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LineChartsWindow::onDimensionChanged);
    connect(m_yearSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &LineChartsWindow::refreshData);
    connect(m_monthCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LineChartsWindow::refreshData);
    connect(refreshBtn, &QPushButton::clicked, this, &LineChartsWindow::refreshData);

    setLayout(mainLayout);
    refreshData();
}

LineChartsWindow::~LineChartsWindow() {}

void LineChartsWindow::refreshData()
{
    if(!m_mainwindow) return;

    int year = m_yearSpin->value();
    int month = m_monthCombo->currentData().toInt();
    m_data = m_mainwindow->getMonthData(year, month);

    updateTitle();
    update();
}

void LineChartsWindow::updateTitle()
{
    QString dimName[] = {"愉悦度(P)", "唤醒度(A)", "支配度(D)", "全部维度（P+A+D）"};
    QString monthText = QString("%1年%2月").arg(m_yearSpin->value())
                            .arg(m_monthCombo->currentData().toInt());

    if (m_data.isEmpty()) {
        m_titleLabel->setText(QString("%1 %2趋势图（暂无数据）")
                                  .arg(monthText)
                                  .arg(dimName[m_currentDimension]));
    } else {
        m_titleLabel->setText(QString("%1 %2趋势图")
                                  .arg(monthText)
                                  .arg(dimName[m_currentDimension]));
    }
}

void LineChartsWindow::onDimensionChanged(int index)
{
    m_currentDimension = index;
    updateTitle();
    update();
}

void LineChartsWindow::calculateLayout()
{
    m_chartWidth = width() - m_leftMargin - m_rightMargin;
    m_chartHeight = height() - m_topMargin - m_bottomMargin;
}

QPointF LineChartsWindow::getPoint(int index, int dimension) const
{
    if(index < 0 || index >= m_data.size() || m_data.size() < 1)
    {
        return QPointF(-1, -1);
    }

    double val;
    switch(dimension)
    {
    case 0:
        val = m_data[index].second.x();
        break;
    case 1:
        val = m_data[index].second.y();
        break;
    case 2:
        val = m_data[index].second.z();
        break;
    default:
        val = 128;
    }

    double x;
    if (m_data.size() == 1) {
        x = m_leftMargin + m_chartWidth / 2.0;
    } else {
        x = m_leftMargin + (double(index) / (m_data.size() - 1)) * m_chartWidth;
    }
    double y = m_topMargin + m_chartHeight * (1.0 - val / 255.0);

    return QPointF(x, y);
}

void LineChartsWindow::drawLegend(QPainter &painter, int yOffset)
{
    struct LegendItem {
        QString name;
        QColor color;
        int dimension;
    };

    QList<LegendItem> legends;
    if (m_currentDimension == 3) {
        legends = {
            {"愉悦度 (P)", QColor(255, 100, 100), 0},
            {"唤醒度 (A)", QColor(100, 200, 100), 1},
            {"支配度 (D)", QColor(100, 100, 255), 2}
        };
    } else {
        QColor colors[] = {QColor(255, 100, 100), QColor(100, 200, 100), QColor(100, 100, 255)};
        QString names[] = {"愉悦度(P)", "唤醒度(A)", "支配度(D)"};
        legends = {{names[m_currentDimension], colors[m_currentDimension], m_currentDimension}};
    }

    int legendX = width() - 130;
    int legendY = yOffset;
    int itemHeight = 25;

    painter.save();
    painter.setOpacity(0.85);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::NoPen);

    int legendHeight = legends.size() * itemHeight + 10;
    painter.drawRect(legendX - 15, legendY - 5, 120, legendHeight);

    QFont legendFont("SimSun", 10);
    painter.setFont(legendFont);

    for (int i = 0; i < legends.size(); i++) {
        int y = legendY + i * itemHeight;

        painter.setPen(QPen(legends[i].color, 2));
        painter.drawLine(legendX, y + 10, legendX + 20, y + 10);

        painter.setBrush(QBrush(legends[i].color));
        painter.drawEllipse(legendX + 8, y + 6, 8, 8);

        painter.setPen(QPen(QColor(44, 44, 44), 1));
        painter.drawText(legendX + 25, y, 80, 20, Qt::AlignLeft, legends[i].name);
    }

    painter.restore();
}

void LineChartsWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QPixmap bg(":/images/bg_chart.jpg");
    if (!bg.isNull()) {
        QPixmap scaled = bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, scaled);
    } else {
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(248, 245, 240));
        gradient.setColorAt(1, QColor(235, 225, 215));
        painter.fillRect(rect(), gradient);
    }
    painter.setRenderHint(QPainter::Antialiasing, true);

    calculateLayout();

    if (m_data.isEmpty()) {
        painter.setPen(QPen(QColor(150, 150, 150), 1));
        painter.drawText(rect(), Qt::AlignCenter,
                         "该月份暂无情绪记录数据\n请先在\"记录情绪\"中记录心情");
        return;
    }

    if (m_titleLabel->text().isEmpty() || m_titleLabel->text().contains("(暂无数据)")) {
        updateTitle();
    }

    painter.setPen(QPen(QColor(44, 44, 44), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(m_leftMargin, m_topMargin, m_chartWidth, m_chartHeight);

    painter.setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));
    QFont font = painter.font();
    font.setPointSize(9);
    painter.setFont(font);

    for (int i = 0; i <= 5; i++) {
        int yValue = 255 - i * 51;
        double y = m_topMargin + m_chartHeight * i / 5.0;
        painter.drawLine(m_leftMargin - 5, y, m_leftMargin + m_chartWidth, y);
        painter.setPen(QPen(QColor(44, 44, 44), 1));
        painter.drawText(m_leftMargin - 50, y - 5, 45, 20, Qt::AlignRight, QString::number(yValue));
        painter.setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));
    }

    int labelInterval = 1;
    int minLabelWidth = 35;

    if (m_data.size() > 1) {
        double pointWidth = m_chartWidth / (m_data.size() - 1);
        if (pointWidth < minLabelWidth) {
            labelInterval = qCeil(minLabelWidth / pointWidth);
        }
    }
    labelInterval = qMin(labelInterval, qMax(1, m_data.size() / 6));

    for (int i = 0; i < m_data.size(); i++) {
        double x;
        if (m_data.size() == 1) {
            x = m_leftMargin + m_chartWidth / 2.0;
        } else {
            x = m_leftMargin + (double(i) / (m_data.size() - 1)) * m_chartWidth;
        }

        bool shouldDrawLabel = (i == 0) || (i == m_data.size() - 1) || (i % labelInterval == 0);

        if (shouldDrawLabel) {
            painter.setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));
            painter.drawLine(x, m_topMargin, x, m_topMargin + m_chartHeight);

            QString dateStr = m_data[i].first;
            QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
            if (date.isValid()) {
                dateStr = QString::number(date.day());
            } else {
                QStringList parts = dateStr.split('-');
                if (parts.size() >= 3) {
                    dateStr = parts[2];
                }
            }

            painter.setPen(QPen(QColor(44, 44, 44), 1));
            if (labelInterval > 1) {
                font.setPointSize(8);
            } else {
                font.setPointSize(9);
            }
            painter.setFont(font);

            int textWidth = 35;
            painter.drawText(x - textWidth/2, m_topMargin + m_chartHeight + 5,
                             textWidth, 20, Qt::AlignCenter, dateStr);
        }
    }

    painter.setPen(QPen(QColor(44, 44, 44), 1));
    painter.drawText(m_leftMargin + m_chartWidth / 2 - 30, height() - 35, 60, 20,
                     Qt::AlignCenter, "日期");
    painter.save();
    painter.translate(20, height() / 2);
    painter.rotate(-90);
    painter.drawText(-30, 0, 60, 20, Qt::AlignCenter, "情绪值");
    painter.restore();

    QColor colors[] = {QColor(255, 100, 100), QColor(100, 200, 100), QColor(100, 100, 255)};

    if (m_currentDimension == 3) {
        for (int dim = 0; dim < 3; dim++) {
            QPen linePen(colors[dim], 2);
            linePen.setCapStyle(Qt::RoundCap);
            linePen.setJoinStyle(Qt::RoundJoin);
            painter.setPen(linePen);

            for (int i = 0; i < m_data.size() - 1; i++) {
                QPointF p1 = getPoint(i, dim);
                QPointF p2 = getPoint(i + 1, dim);
                if (p1.x() >= 0 && p2.x() >= 0) {
                    painter.drawLine(p1, p2);
                }
            }
            for (int i = 0; i < m_data.size(); i++) {
                QPointF point = getPoint(i, dim);
                if (point.x() < 0) continue;

                QColor pointColor = colors[dim];
                pointColor.setAlpha(180);
                painter.setPen(QPen(colors[dim].darker(150), 1.5));
                painter.setBrush(QBrush(pointColor));
                painter.drawEllipse(point, 5, 5);
            }
        }
    }

    else {
        QPen linePen(colors[m_currentDimension], 3);
        linePen.setCapStyle(Qt::RoundCap);
        linePen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(linePen);

        for (int i = 0; i < m_data.size() - 1; i++) {
            QPointF p1 = getPoint(i, m_currentDimension);
            QPointF p2 = getPoint(i + 1, m_currentDimension);
            if (p1.x() >= 0 && p2.x() >= 0) {
                painter.drawLine(p1, p2);
            }
        }

        for (int i = 0; i < m_data.size(); i++) {
            QPointF point = getPoint(i, m_currentDimension);
            if (point.x() < 0) continue;

            QColor pointColor = colors[m_currentDimension];
            pointColor.setAlpha(180);
            painter.setPen(QPen(colors[m_currentDimension].darker(150), 2));
            painter.setBrush(QBrush(pointColor));
            painter.drawEllipse(point, 5, 5);
        }
    }
    drawLegend(painter, m_topMargin + 5);
}