#include "mainwindow.h"
#include "calendarview.h"
#include "recordemotions.h"
#include "linecharts.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDate>
#include<QMessageBox>
#include<QSqlQuery>
#include<QVector3D>
#include<QSqlError>
#include <QPushButton>
#include <QTextDocument>
#include "diaryviewwindow.h"
#include "basewindow.h"
#include <QPainter>
#include <QLinearGradient>

static bool dbInitialized = false;

QSqlDatabase& MainWindow::getDatabase()
{
    static QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    return db;
}

bool MainWindow::isDatabaseInitialized()
{
    return dbInitialized;
}

void MainWindow::initDatabase()
{
    QSqlDatabase& db = getDatabase();
    db.setDatabaseName("emotions.db");

    if (!db.open()) {
        return;
    }

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS daily_emotions ("
               "date TEXT PRIMARY KEY,"
               "p INTEGER NOT NULL,"
               "a INTEGER NOT NULL,"
               "d INTEGER NOT NULL)");


    QSqlQuery checkQuery(db);
    checkQuery.exec("PRAGMA table_info(daily_emotions)");
    bool hasDiary = false;
    while (checkQuery.next())
    {
        if (checkQuery.value(1).toString() == "diary")
        {
            hasDiary = true;
            break;
        }
    }
    if (!hasDiary)
    {
        query.exec("ALTER TABLE daily_emotions ADD COLUMN diary TEXT");
    }

    query.exec("CREATE TABLE IF NOT EXISTS capsule_shown ("
               "last_shown_date TEXT PRIMARY KEY)");


    dbInitialized = true;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("情绪日记");
    setFixedSize(400, 600);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    mainLayout->addStretch();

    QLabel *titleLabel = new QLabel(this);
    titleLabel->setText("Emotion Diary");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Rage Italic", 50, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet(
        "QLabel {"
        "  color: #2C2C2C;"
        "  font-size: 50px;"
        "  font-weight: bold;"
        "  background: transparent;"
        "  padding: 20px;"
        "}"
        );
    mainLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel(this);
    subtitleLabel->setText("—— 记录你每一天的心情 ——");
    subtitleLabel->setAlignment(Qt::AlignCenter);

    QFont subtitleFont("KaiTi", 14);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet(
        "QLabel {"
        "  color: #5A4A3A;"
        "  background: transparent;"
        "  padding: 5px;"
        "}"
        );
    mainLayout->addWidget(subtitleLabel);

    mainLayout->addStretch();

    m_recordBtn = new QPushButton("记 录 每 日 情 绪️", this);
    m_recordBtn->setFixedHeight(40);
    m_recordBtn->setFixedWidth(250);
    mainLayout->addWidget(m_recordBtn, 0, Qt::AlignCenter);

    m_calendarBtn = new QPushButton("查 看 月 度 情 绪", this);
    m_calendarBtn->setFixedHeight(40);
    m_calendarBtn->setFixedWidth(250);
    mainLayout->addWidget(m_calendarBtn, 0, Qt::AlignCenter);

    m_lineChartsBtn = new QPushButton("查 看 月 度 趋 势", this);
    m_lineChartsBtn->setFixedHeight(40);
    m_lineChartsBtn->setFixedWidth(250);
    mainLayout->addWidget(m_lineChartsBtn, 0, Qt::AlignCenter);

    mainLayout->addStretch();

    connect(m_recordBtn, &QPushButton::clicked, this, &MainWindow::openRecordWindow);
    connect(m_calendarBtn, &QPushButton::clicked, this, &MainWindow::openCalendarWindow);
    connect(m_lineChartsBtn, &QPushButton::clicked, this, &MainWindow::openLineChartsWindow);

    central->setStyleSheet(
        "QPushButton {"
        "  background-color: black;"
        "  color: #FFFFFF;"
        "  border: 4px solid #2C2C2C;"
        "  font-family: 'SimSun';"
        "  font-size: 18px;"
        "  font-weight: normal;"
        "  padding: 10px 20px;"
        "  min-height: 40px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(44, 44, 44, 0.9);"
        "}"
        );

    initDatabase();
}

MainWindow::~MainWindow()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

void MainWindow::openRecordWindow()
{
    if (m_recordWindow.isNull() || m_recordWindow->isHidden()) {
        if (!m_recordWindow.isNull()) {
            delete m_recordWindow;
        }
        m_recordWindow = new RecordWindow(this);
        m_recordWindow->setAttribute(Qt::WA_DeleteOnClose);
        m_recordWindow->show();

        checkCapsule();
    } else {
        m_recordWindow->raise();
        m_recordWindow->activateWindow();
    }
}

void MainWindow::openCalendarWindow()
{
    if (m_calendarWindow.isNull() || m_calendarWindow->isHidden()) {
        if (!m_calendarWindow.isNull()) {
            delete m_calendarWindow;
        }
        m_calendarWindow = new CalendarWidget(this);
        m_calendarWindow->setAttribute(Qt::WA_DeleteOnClose);
        m_calendarWindow->show();

        checkCapsule();
    } else {
        m_calendarWindow->raise();
        m_calendarWindow->activateWindow();
    }
}

void MainWindow::openLineChartsWindow()
{
    if (m_lineChartsWindow.isNull() || m_lineChartsWindow->isHidden()) {
        if (!m_lineChartsWindow.isNull()) {
            delete m_lineChartsWindow;
        }
        m_lineChartsWindow = new LineChartsWindow(this);
        m_lineChartsWindow->setAttribute(Qt::WA_DeleteOnClose);
        m_lineChartsWindow->show();

        checkCapsule();
    } else {
        m_lineChartsWindow->raise();
        m_lineChartsWindow->activateWindow();
    }
}


QVector<QPair<QString, QVector3D>> MainWindow::getMonthData(int year, int month)
{
    QVector<QPair<QString, QVector3D>> result;
    QSqlDatabase& db = getDatabase();

    if (!db.isOpen()) {
        return result;
    }

    QSqlQuery query(db);

    QString dateStart = QString("%1-%2-01").arg(year).arg(month, 2, 10, QChar('0'));
    QString dateEnd = QString("%1-%2-31").arg(year).arg(month, 2, 10, QChar('0'));

    query.prepare("SELECT date, p, a, d FROM daily_emotions WHERE date BETWEEN ? AND ? ORDER BY date");
    query.addBindValue(dateStart);
    query.addBindValue(dateEnd);

    if (query.exec()) {
        while (query.next()) {
            QString date = query.value(0).toString();
            int p = query.value(1).toInt();
            int a = query.value(2).toInt();
            int d = query.value(3).toInt();
            result.append({date, QVector3D(p, a, d)});
        }
    }
    return result;
}

QVector3D MainWindow::getEmotionByDate(const QString &date)
{
    QSqlQuery query;
    query.prepare("SELECT p, a, d FROM daily_emotions WHERE date = ?");
    query.addBindValue(date);

    if (query.exec() && query.next()) {
        return QVector3D(query.value(0).toInt(),
                         query.value(1).toInt(),
                         query.value(2).toInt());
    }
    return QVector3D(128, 128, 128);
}

void MainWindow::checkCapsule()
{
    QSqlDatabase &db = getDatabase();
    if (!db.isOpen()) return;

    QDate today = QDate::currentDate();
    QDate lastMonthSameDay = today.addMonths(-1);

    if (lastMonthSameDay.month() != today.month() - 1 &&
        !(today.month() == 1 && lastMonthSameDay.month() == 12))
        return;

    QString todayStr = today.toString("yyyy-MM-dd");

    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT last_shown_date FROM capsule_shown WHERE last_shown_date = ?");
    checkQuery.addBindValue(todayStr);
    if (checkQuery.exec() && checkQuery.next()) {
        return; //今天弹过，不用再弹
    }

    //查上个月同一天有没有日记
    QString dateStr = lastMonthSameDay.toString("yyyy-MM-dd");
    QSqlQuery diaryQuery(db);
    diaryQuery.prepare("SELECT diary FROM daily_emotions WHERE date = ?");
    diaryQuery.addBindValue(dateStr);
    if (!diaryQuery.exec() || !diaryQuery.next())
        return;

    QString diaryContent = diaryQuery.value(0).toString();
    if (diaryContent.isEmpty())
        return;

    QTextDocument doc;
    doc.setHtml(diaryContent);
    if (doc.isEmpty())
        return;

    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO capsule_shown (last_shown_date) VALUES (?)");
    insertQuery.addBindValue(todayStr);
    insertQuery.exec();

    // 弹窗
    QVector3D emotion = getEmotionByDate(dateStr);
    QColor color(emotion.x(), emotion.y(), emotion.z());
    QMessageBox msgBox;
    QPixmap pixmap(32, 32);
    pixmap.fill(color);
    msgBox.setIconPixmap(pixmap);
    msgBox.setText("叮咚~ 收到一个时光胶囊💊\n\n来自 " + lastMonthSameDay.toString("yyyy年M月d日") + " 的日记");
    QPushButton *viewBtn = msgBox.addButton("哇！去看看", QMessageBox::AcceptRole);
    msgBox.addButton("知道啦", QMessageBox::RejectRole);
    msgBox.setDefaultButton(viewBtn);
    msgBox.exec();

    if (msgBox.clickedButton() == viewBtn) {
        DiaryViewWindow *viewWin = new DiaryViewWindow(this);
        viewWin->setAttribute(Qt::WA_DeleteOnClose);
        viewWin->loadDiary(dateStr, diaryContent);
        viewWin->show();
    }
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QPixmap bg(":/images/bg_main.jpg");
    if (!bg.isNull()) {
        QPixmap scaled = bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, scaled);
    } else {
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(248, 245, 240));
        gradient.setColorAt(1, QColor(235, 225, 215));
        painter.fillRect(rect(), gradient);
    }

    QMainWindow::paintEvent(event);
}
