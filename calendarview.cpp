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
    : QWidget(parent), m_mainWindow(mw)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_calendar = new QCalendarWidget(this);
    layout->addWidget(m_calendar);

    setWindowTitle("情绪日历");
    resize(450, 400);

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
