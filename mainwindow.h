#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QSqlDatabase>
#include<QSlider>
#include<QLabel>
#include<QPushButton>
#include<QDateEdit>
#include<QPointer>
#include<calendarview.h>
#include<recordemotions.h>
#include<linecharts.h>
#include<basewindow.h>

class MainWindow:public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget*parent=nullptr);
    ~MainWindow();
    QVector<QPair<QString,QVector3D>>getMonthData(int year,int month);
    QVector3D getEmotionByDate(const QString &date);

    static QSqlDatabase& getDatabase();
    static bool isDatabaseInitialized();
private slots:
    void openRecordWindow();
    void openCalendarWindow();
    void openLineChartsWindow();
    void toggleSidebar();
    void onCapsuleToggled();
private:
    void initDatabase();
    void loadEmotion(const QString& date );
    void updatePreview();
    void saveCurrentEmotion();
    void checkCapsule();
    void paintEvent(QPaintEvent* event) override;
    QPushButton *m_menuBtn;
    QWidget *m_sidebar;
    bool m_sidebarVisible;
    QPushButton *m_capsuleToggle;
    bool isCapsuleEnabled();

    QPointer<RecordWindow> m_recordWindow;
    QPointer<CalendarWidget> m_calendarWindow;
    QPointer<LineChartsWindow> m_lineChartsWindow;
    QPushButton*m_recordBtn;
    QPushButton*m_calendarBtn;
    QPushButton*m_lineChartsBtn;
    QSqlDatabase m_db;
};
#endif
