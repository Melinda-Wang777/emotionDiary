#include "calendarview.h"
#include "mainwindow.h"
#include "qvectornd.h"
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QTextCharFormat>
#include <QDate>
#include <QMap>
#include <QTimer>
#include <QMessageBox>
#include <QSqlQuery>
#include <diarywindow.h>
#include <QTextDocument>
#include "diaryviewwindow.h"

CalendarWidget::CalendarWidget(MainWindow *mw, QWidget *parent)
    : BaseWindow(parent), m_mainWindow(mw)
{
    setGradientBackground(QColor(240, 245, 245), QColor(225, 235, 230));
    // 如果有背景图，取消注释下面这行
    // setBackgroundImage(":/images/bg_calendar.jpg");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);

    m_calendar = new QCalendarWidget(this);

    // 日历控件极简样式
    m_calendar->setStyleSheet(
        "QCalendarWidget {"
        "  background-color: rgba(255, 255, 255, 0.7);"
        "  border-radius: 16px;"
        "  padding: 10px;"
        "}"
        "QCalendarWidget QAbstractItemView {"
        "  selection-background-color: #A8C4B0;"
        "  selection-color: white;"
        "}"
        "QCalendarWidget QToolButton {"
        "  color: white;"
        "  background: transparent;"
        "  font-weight: normal;"
        "}"
        "QCalendarWidget QMenu {"
        "  background-color: white;"
        "}"
        );

    layout->addWidget(m_calendar);

    setWindowTitle("情绪日历");
    resize(500, 450);

    connect(m_calendar, &QCalendarWidget::currentPageChanged,
            this, &CalendarWidget::updateCalendarColors);

    connect(m_calendar, &QCalendarWidget::clicked,
            this, &CalendarWidget::onDateClicked);

    QTimer::singleShot(100, this, [this](){updateCalendarColors();});
  }

void CalendarWidget::updateCalendarColors()
{
    if (!m_mainWindow) return;

    int year = m_calendar->yearShown();
    int month = m_calendar->monthShown();

    auto data = m_mainWindow->getMonthData(year, month);

    QMap<QString, QVector3D> dateMap;
    for (auto &item : data) {
        dateMap[item.first] = item.second;
    }

    QDate firstDay(year, month, 1);
    int daysInMonth = firstDay.daysInMonth();

    for (int day = 1; day <= daysInMonth; day++) {
        QDate date(year, month, day);
        QString key = date.toString("yyyy-MM-dd");

        QTextCharFormat fmt;

        if (dateMap.contains(key)) {
            QVector3D pad = dateMap[key];
            int p = pad.x();
            int a = pad.y();
            int d = pad.z();
            fmt.setBackground(QColor(p, a, d));
            int gray = (p + a + d) / 3;
            fmt.setForeground(gray > 128 ? Qt::black : Qt::white);
        } else {
            fmt.setBackground(Qt::white);
            fmt.setForeground(Qt::black);
        }

        m_calendar->setDateTextFormat(date, fmt);
    }
}

void CalendarWidget::onDateClicked(const QDate &date)
{
    QString dateStr = date.toString("yyyy-MM-dd");

    QSqlDatabase &db = MainWindow::getDatabase();
    QString diaryContent;
    bool hasDiary = false;

    if (db.isOpen()) {
        QSqlQuery query(db);
        query.prepare("SELECT diary FROM daily_emotions WHERE date = ?");
        query.addBindValue(dateStr);
        if (query.exec() && query.next()) {
            diaryContent = query.value(0).toString();
            if (!diaryContent.isEmpty()) {
                // 用 QTextDocument 解析 HTML，检查是否真的为空
                QTextDocument doc;
                doc.setHtml(diaryContent);
                if (!doc.isEmpty()) {
                    hasDiary = true;
                }
            }
        }
    }

    if (hasDiary) {
        DiaryViewWindow *viewWin = new DiaryViewWindow(this);
        viewWin->setAttribute(Qt::WA_DeleteOnClose);
        viewWin->loadDiary(dateStr, diaryContent);
        viewWin->show();
    } else {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("没有记录");
        msgBox.setText(dateStr + " 还没有写日记哦~");
        msgBox.setInformativeText("要现在补充吗？");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        if (msgBox.exec() == QMessageBox::Yes) {
            DiaryWindow *diaryWin = new DiaryWindow(nullptr);
            diaryWin->setAttribute(Qt::WA_DeleteOnClose);
            diaryWin->loadDiary(dateStr);
            diaryWin->show();
        }
    }
}
