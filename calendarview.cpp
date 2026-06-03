#include "calendarview.h"
#include "mainwindow.h"
#include "qvectornd.h"
#include <QCalendarWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextCharFormat>
#include <QDate>
#include <QMap>
#include <QTimer>
#include <QMessageBox>
#include <QSqlQuery>
#include <diarywindow.h>
#include <QTextDocument>
#include "diaryviewwindow.h"
#include <QShowEvent>

CalendarWidget::CalendarWidget(MainWindow *mw, QWidget *parent)
    : BaseWindow(parent), m_mainWindow(mw)
{
    setGradientBackground(QColor(253, 249, 245), QColor(240, 235, 230));
    // 如果有背景图，取消注释下面这行
    // setBackgroundImage(":/images/bg_calendar.jpg");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);

    m_calendar = new QCalendarWidget(this);
    m_calendar->setFirstDayOfWeek(Qt::Sunday);

    // 日历控件极简样式
    m_calendar->setStyleSheet(
        "QCalendarWidget {"
        "  background-color: rgba(255, 255, 255, 0.65);"
        "  border-radius: 16px;"
        "  padding: 12px;"
        "  border: 1px solid #E8E0D8;"
        "}"

        "QCalendarWidget QTableView {"
        "  background-color: transparent;"
        "  selection-background-color: #C4A882;"
        "  selection-color: white;"
        "  alternate-background-color: transparent;"
        "  gridline-color: transparent;"
        "  font-size: 13px;"
        "}"

        "QCalendarWidget QHeaderView::section {"
        "  background-color: transparent;"
        "  color: #8A7A6A;"
        "  font-weight: normal;"
        "  padding-bottom: 6px;"
        "}"

        "QCalendarWidget QWidget#qt_calendar_navigationbar {"
        "  background-color: transparent;"
        "  border: 1px solid #3A3A3A;"
        "  border-radius: 10px;"
        "  padding: 8px 12px;"
        "}"

        "QCalendarWidget QToolButton {"
        "  background-color: transparent;"
        "  border: none;"
        "  color: #5A4A3A;"
        "  font-size: 15px;"
        "  font-weight: normal;"
        "  padding: 0 4px;"
        "}"

        "QCalendarWidget QToolButton::left-arrow,"
        "QCalendarWidget QToolButton::right-arrow {"
        "  image: none;"
        "}"

        "QCalendarWidget QToolButton[text=\"‹\"] {"
        "  font-family: \"Segoe UI Symbol\", \"PingFang SC\";"
        "  font-size: 18px;"
        "  color: #000000;"
        "}"
        "QCalendarWidget QToolButton[text=\"›\"] {"
        "  font-family: \"Segoe UI Symbol\", \"PingFang SC\";"
        "  font-size: 18px;"
        "  color: #000000;"
        "}"

        "QCalendarWidget QToolButton:hover {"
        "  color: #000000;"
        "}"

        "QCalendarWidget QMenu {"
        "  background-color: white;"
        "  border-radius: 10px;"
        "  padding: 4px;"
        "}"
        "QCalendarWidget QMenu::item {"
        "  color: #5A4A3A;"
        "  padding: 6px 16px;"
        "  border-radius: 6px;"
        "}"
        "QCalendarWidget QMenu::item:selected {"
        "  background-color: #C4A882;"
        "  color: white;"
        "}"

        "QCalendarWidget QSpinBox {"
        "  color: #5A4A3A;"
        "  background-color: white;"
        "  border: 1px solid #E0D8D0;"
        "  border-radius: 6px;"
        "}"
        );

    layout->addWidget(m_calendar);

    setWindowTitle("情绪日历");
    resize(500, 450);

    connect(m_calendar, &QCalendarWidget::currentPageChanged,
            this, &CalendarWidget::updateCalendarColors);

    connect(m_calendar, &QCalendarWidget::clicked,
            this, &CalendarWidget::onDateClicked);
}

void CalendarWidget::updateCalendarColors()
{
    if (!m_mainWindow) return;

    int year = m_calendar->yearShown();
    int month = m_calendar->monthShown();

    QDate firstOfMonth(year, month, 1);

    QCalendar cal;
    int weekdayIndex = cal.dayOfWeek(firstOfMonth);

    QDate startDate;
    if (weekdayIndex == 7) {
        startDate = firstOfMonth.addDays(-7);
    } else {
        Qt::DayOfWeek firstDayOfWeek = m_calendar->firstDayOfWeek();
        int offset = (weekdayIndex - static_cast<int>(firstDayOfWeek) + 7) % 7;
        startDate = firstOfMonth.addDays(-offset);
    }

    //遍历所有可见日期
    QTextCharFormat defaultFmt;
    defaultFmt.setBackground(Qt::white);
    defaultFmt.setForeground(Qt::black);

    for (int i = 0; i < 42; ++i) {
        QDate date = startDate.addDays(i);
        m_calendar->setDateTextFormat(date, defaultFmt);
    }

    auto data = m_mainWindow->getMonthData(year, month);

    QMap<QString, QVector3D> dateMap;
    for (auto &item : data) {
        dateMap[item.first] = item.second;
    }

    int daysInMonth = firstOfMonth.daysInMonth();
    for (int day = 1; day <= daysInMonth; ++day) {
        QDate date(year, month, day);
        QString key = date.toString("yyyy-MM-dd");

        if (dateMap.contains(key)) {
            QVector3D pad = dateMap[key];
            int p = static_cast<int>(pad.x());
            int a = static_cast<int>(pad.y());
            int d = static_cast<int>(pad.z());
            QTextCharFormat fmt;
            fmt.setBackground(QColor(p, a, d));
            int gray = (p + a + d) / 3;
            fmt.setForeground(gray > 128 ? Qt::black : Qt::white);
            m_calendar->setDateTextFormat(date, fmt);
        }
    }
}

void CalendarWidget::showEvent(QShowEvent *event)
{
    BaseWindow::showEvent(event);
    updateCalendarColors();
    m_calendar->update();
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
        // 使用自定义 QDialog 替代 QMessageBox
        QDialog dialog(this);
        dialog.setWindowTitle("没有记录");
        dialog.setModal(true);
        dialog.setFixedSize(320, 150); // 可根据文字调整

        // 设置布局
        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        layout->setSpacing(12);
        layout->setContentsMargins(20, 20, 20, 20);

        QLabel *textLabel = new QLabel(dateStr + " 还没有写日记哦~", &dialog);
        textLabel->setAlignment(Qt::AlignCenter);
        textLabel->setWordWrap(true);
        layout->addWidget(textLabel);

        QLabel *infoLabel = new QLabel("要现在补充吗？", &dialog);
        infoLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(infoLabel);

        QHBoxLayout *btnLayout = new QHBoxLayout;
        btnLayout->setSpacing(20);
        QPushButton *yesBtn = new QPushButton("是", &dialog);
        QPushButton *noBtn = new QPushButton("否", &dialog);
        yesBtn->setDefault(true);
        btnLayout->addStretch();
        btnLayout->addWidget(yesBtn);
        btnLayout->addWidget(noBtn);
        btnLayout->addStretch();
        layout->addLayout(btnLayout);

        dialog.setStyleSheet(
            "QDialog {"
            "  background-color: #F0E9E1;"
            "  border: 1px solid #E8E0D8;"
            "}"
            "QLabel {"
            "  color: #4A3A2A;"
            "  font-size: 13px;"
            "  font-family: 'SimSun';"
            "}"
            "QPushButton {"
            "  background-color: transparent;"
            "  border: 1px solid black;"
            "  border-radius: 8px;"
            "  color: #2A2A2A;"
            "  padding: 6px 16px;"
            "  min-width: 70px;"
            "  font-family: 'SimSun';"
            "}"
            "QPushButton:hover {"
            "  background-color: rgba(0, 0, 0, 0.05);"
            "}"
            "QPushButton:default {"
            "  background-color: #D2B48C;"
            "  border-color: black;"
            "  color: white;"
            "}"
            "QPushButton:default:hover {"
            "  background-color: #C4A882;"
            "}"
            );

        connect(yesBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
        connect(noBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            DiaryWindow *diaryWin = new DiaryWindow(nullptr);
            diaryWin->setAttribute(Qt::WA_DeleteOnClose);
            diaryWin->loadDiary(dateStr);
            diaryWin->show();
        }
    }
}
