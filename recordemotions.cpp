#include "recordemotions.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDate>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector3D>

RecordWindow::RecordWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("记录情绪");
    setWindowFlags(Qt::Window);
    resize(1000,600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *dateLayout = new QHBoxLayout();
    dateLayout->addWidget(new QLabel("日期:", this));
    m_dateEdit = new QDateEdit(QDate::currentDate(), this);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    dateLayout->addWidget(m_dateEdit);
    mainLayout->addLayout(dateLayout);

    QGroupBox *groupP = new QGroupBox("愉悦度 (P) - 难过 ← → 开心", this);
    QVBoxLayout *layoutP = new QVBoxLayout(groupP);
    m_sliderP = new QSlider(Qt::Horizontal, this);
    m_sliderP->setRange(0, 255);
    m_sliderP->setValue(128);
    layoutP->addWidget(m_sliderP);
    mainLayout->addWidget(groupP);

    QGroupBox *groupA = new QGroupBox("唤醒度 (A) - 平静 ← → 兴奋", this);
    QVBoxLayout *layoutA = new QVBoxLayout(groupA);
    m_sliderA = new QSlider(Qt::Horizontal, this);
    m_sliderA->setRange(0, 255);
    m_sliderA->setValue(128);
    layoutA->addWidget(m_sliderA);
    mainLayout->addWidget(groupA);

    QGroupBox *groupD = new QGroupBox("支配度 (D) - 无力 ← → 掌控", this);
    QVBoxLayout *layoutD = new QVBoxLayout(groupD);
    m_sliderD = new QSlider(Qt::Horizontal, this);
    m_sliderD->setRange(0, 255);
    m_sliderD->setValue(128);
    layoutD->addWidget(m_sliderD);
    mainLayout->addWidget(groupD);

    QHBoxLayout *previewLayout = new QHBoxLayout();
    m_previewLabel = new QLabel(this);
    m_previewLabel->setFixedSize(100, 100);
    m_previewLabel->setStyleSheet("border: 1px solid black; background-color: rgb(128,128,128);");
    previewLayout->addWidget(m_previewLabel);
    m_valueLabel = new QLabel("P:128 A:128 D:128", this);
    previewLayout->addWidget(m_valueLabel);
    mainLayout->addLayout(previewLayout);

    m_saveBtn = new QPushButton("保存今日情绪", this);
    m_diaryBtn = new QPushButton("去写日记📝", this);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_saveBtn);
    btnLayout->addWidget(m_diaryBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_sliderP, &QSlider::valueChanged, this, &RecordWindow::onSliderChanged);
    connect(m_sliderA, &QSlider::valueChanged, this, &RecordWindow::onSliderChanged);
    connect(m_sliderD, &QSlider::valueChanged, this, &RecordWindow::onSliderChanged);
    connect(m_saveBtn, &QPushButton::clicked, this, &RecordWindow::onSaveButtonClicked);
    connect(m_dateEdit, &QDateEdit::dateChanged, [this]() {
        loadEmotion(m_dateEdit->date().toString("yyyy-MM-dd"));
    });

    loadEmotion(QDate::currentDate().toString("yyyy-MM-dd"));

    this->setStyleSheet(
        "QWidget {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 #E8F4F8, stop:1 #F0E8F4);"
        "}"
        "QGroupBox {"
        "  font-weight: bold;"
        "  font-size: 13px;"
        "  border: 2px solid #A8D8EA;"
        "  border-radius: 12px;"
        "  margin-top: 14px;"
        "  padding-top: 10px;"
        "  background-color: rgba(255,255,255,0.6);"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 12px;"
        "  padding: 0 8px;"
        "  color: #E8D0B0;"
        "}"
        "QSlider::groove:horizontal {"
        "  height: 8px;"
        "  background: #D8F0D8;"
        "  border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: qradialgradient(cx:0.5, cy:0.5, radius:0.5,"
        "    stop:0 white, stop:1 #F5C0C0);"
        "  width: 18px;"
        "  height: 18px;"
        "  margin: -5px 0;"
        "  border-radius: 9px;"
        "}"
        "QSlider::handle:horizontal:hover {"
        "  background: qradialgradient(cx:0.5, cy:0.5, radius:0.5,"
        "    stop:0 white, stop:1 #F0A8A8);"
        "}"
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #F08080, stop:1 #FAA0A0);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 8px;"
        "  padding: 10px 20px;"
        "  font-weight: bold;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #E07070, stop:1 #Ea9090);"
        "}"
        "QPushButton:pressed {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #D06060, stop:1 #D08080);"
        "}"
        "QLabel {"
        "  color: #555;"
        "  font-size: 12px;"
        "}"
        "QLabel#valueLabel {"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "  color: #4A90D9;"
        "  background-color: rgba(255,255,255,0.7);"
        "  border-radius: 8px;"
        "  padding: 5px 10px;"
        "}"
        "QDateEdit {"
        "  border: 2px solid #A8D8EA;"
        "  border-radius: 8px;"
        "  padding: 6px 10px;"
        "  background-color: white;"
        "  color: #333;"
        "  font-size: 13px;"
        "}"
        "QDateEdit:hover {"
        "  border-color: #4A90D9;"
        "}"

        "QCalendarWidget {"
        "  background-color: white;"
        "}"
        "QCalendarWidget QWidget {"
        "  background-color: white;"
        "}"
        "QCalendarWidget QTableView {"
        "  background-color: white;"
        "  selection-background-color: #4A90D9;"
        "  selection-color: white;"
        "}"
        "QCalendarWidget QToolButton {"
        "  color: black;"
        "  background-color: #D0E8F0;"
        "  border: none;"
        "  border-radius: 4px;"
        "  font-weight: bold;"
        "}"
        "QCalendarWidget QToolButton:hover {"
        "  background-color: #B8D8E8;"
        "}"
        "QCalendarWidget QMenu {"
        "  color: black;"
        "  background-color: white;"
        "}"
        "QCalendarWidget QMenu::item:selected {"
        "  color: white;"
        "}"
        "QCalendarWidget QSpinBox {"
        "  color: black;"
        "  background-color: white;"
        "  border: 1px solid #A8D8EA;"
        "  border-radius: 4px;"
        "}"
        );

    connect(m_diaryBtn, &QPushButton::clicked, this, [this]() {
        if (m_diaryWindow.isNull() || !m_diaryWindow->isVisible()) {
            if (!m_diaryWindow.isNull())
                delete m_diaryWindow;
            m_diaryWindow = new DiaryWindow(nullptr);
            m_diaryWindow->setAttribute(Qt::WA_DeleteOnClose);
            m_diaryWindow->loadDiary(m_dateEdit->date().toString("yyyy-MM-dd"));
            m_diaryWindow->show();
        } else {
            m_diaryWindow->raise();
            m_diaryWindow->activateWindow();
        }
    });

    connect(m_dateEdit, &QDateEdit::dateChanged, this, [this](const QDate &date) {
        if (!m_diaryWindow.isNull() && m_diaryWindow->isVisible()) {
            m_diaryWindow->loadDiary(date.toString("yyyy-MM-dd"));
        }
    });

    if (!MainWindow::isDatabaseInitialized()) {
        QMessageBox::warning(this, "警告", "数据库未初始化");
    }
}

RecordWindow::~RecordWindow()
{
    if (!m_diaryWindow.isNull())
    {
        m_diaryWindow->close();
    }
}

void RecordWindow::loadEmotion(const QString &date)
{
    QSqlDatabase db = MainWindow::getDatabase();
    if (!db.isOpen()) {
        db = MainWindow::getDatabase();
        if (!db.isOpen()) {
            qDebug() << "Database still not open";
            return;
        }
    }

    QSqlQuery query(db);
    query.prepare("SELECT p, a, d FROM daily_emotions WHERE date = ?");
    query.addBindValue(date);

    if (query.exec() && query.next()) {
        m_sliderP->setValue(query.value(0).toInt());
        m_sliderA->setValue(query.value(1).toInt());
        m_sliderD->setValue(query.value(2).toInt());
    } else {
        m_sliderP->setValue(128);
        m_sliderA->setValue(128);
        m_sliderD->setValue(128);
    }
    updatePreview();
}

void RecordWindow::onSliderChanged()
{
    updatePreview();
}

void RecordWindow::updatePreview()
{
    int p = m_sliderP->value();
    int a = m_sliderA->value();
    int d = m_sliderD->value();

    m_valueLabel->setText(QString("P:%1  A:%2  D:%3").arg(p).arg(a).arg(d));
    QString colorStyle = QString("background-color: rgb(%1, %2, %3); border: 1px solid black;")
                             .arg(p).arg(a).arg(d);
    m_previewLabel->setStyleSheet(colorStyle);
}

void RecordWindow::onSaveButtonClicked()
{
    saveCurrentEmotion();
}

void RecordWindow::saveCurrentEmotion()
{
    QString date = m_dateEdit->date().toString("yyyy-MM-dd");
    int p = m_sliderP->value();
    int a = m_sliderA->value();
    int d = m_sliderD->value();

    QSqlDatabase& db = MainWindow::getDatabase();
    if (!db.isOpen()) {
        QMessageBox::warning(this, "错误", "数据库未打开");
        return;
    }

    //此版本会在第一次修改情绪的时候完全替换旧日期下内容，如果此前写了日记则日记会消失。

    //QSqlQuery query(db);
    //query.prepare("INSERT OR REPLACE INTO daily_emotions (date, p, a, d) VALUES (?, ?, ?, ?)");
    //query.addBindValue(date);
    //query.addBindValue(p);
    //query.addBindValue(a);
    //query.addBindValue(d);

    //if (query.exec()) {
    //    QMessageBox::information(this, "成功", "情绪已保存");
    //} else {
    //    QMessageBox::warning(this, "错误", "保存失败: " + query.lastError().text());
    //}

    //尝试更新已有的情绪记录
    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE daily_emotions SET p = ?, a = ?, d = ? WHERE date = ?");
    updateQuery.addBindValue(p);
    updateQuery.addBindValue(a);
    updateQuery.addBindValue(d);
    updateQuery.addBindValue(date);
    updateQuery.exec();//如果该日期下没有记录，则执行该行不会发生变化，进入if语句

    if (updateQuery.numRowsAffected() == 0)
    {
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO daily_emotions (date, p, a, d, diary) VALUES (?, ?, ?, ?, NULL)");
        insertQuery.addBindValue(date);
        insertQuery.addBindValue(p);
        insertQuery.addBindValue(a);
        insertQuery.addBindValue(d);
        insertQuery.exec();
    }

    QMessageBox::information(this, "成功", "情绪已保存");
}

QVector3D RecordWindow::getEmotionByDate(const QString &date)
{
    QSqlDatabase& db = MainWindow::getDatabase();
    if (!db.isOpen()) return QVector3D(128, 128, 128);

    QSqlQuery query(db);

    query.prepare("SELECT p, a, d FROM daily_emotions WHERE date = ?");
    query.addBindValue(date);

    if (query.exec() && query.next()) {
        return QVector3D(query.value(0).toInt(),
                         query.value(1).toInt(),
                         query.value(2).toInt());
    }
    return QVector3D(128, 128, 128);
}
