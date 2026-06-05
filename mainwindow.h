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
#include <QPropertyAnimation>
#include <QPainter>

class ToggleSwitch : public QWidget
{
    Q_OBJECT
public:
    explicit ToggleSwitch(QWidget *parent = nullptr) : QWidget(parent)
    {
        setFixedSize(36, 22);
        m_anim = new QPropertyAnimation(this, "offset", this);
        m_anim->setDuration(200);
    }

    void setChecked(bool checked)
    {
        if (m_checked == checked) return;
        m_checked = checked;
        m_anim->stop();
        m_anim->setStartValue(offset());
        m_anim->setEndValue(checked ? 14 : 0);
        m_anim->start();
    }

    bool isChecked() const { return m_checked; }

signals:
    void toggled(bool checked);
    void offsetChanged();

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        QColor bg = m_checked ? QColor("#A0C4A0") : QColor("#CCC");
        p.setBrush(bg);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(rect(), 11, 11);

        p.setBrush(QColor("#FFF"));
        p.drawEllipse(QPointF(11 + m_offset, 11), 9, 9);
    }

    void mousePressEvent(QMouseEvent *) override
    {
        setChecked(!m_checked);
        emit toggled(m_checked);
    }

private:
    bool m_checked = false;
    int m_offset = 0;
    QPropertyAnimation *m_anim;

    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)

    int offset() const { return m_offset; }
    void setOffset(int val) {
        m_offset = val;
        emit offsetChanged();
        update();
    }
};

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
    ToggleSwitch *m_capsuleToggle;
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
