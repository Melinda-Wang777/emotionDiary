#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H

#include <QWidget>
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QDate>
#include "basewindow.h"

class MainWindow;

class CalendarWidget : public BaseWindow
{
    Q_OBJECT

public:
    explicit CalendarWidget(MainWindow *mw, QWidget *parent = nullptr);

    void updateCalendarColors();

private:
    MainWindow *m_mainWindow;
    QCalendarWidget *m_calendar;

private slots:
    void onDateClicked(const QDate &date);
};

#endif // CALENDARVIEW_H
