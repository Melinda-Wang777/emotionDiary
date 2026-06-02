#include "diaryviewwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDate>
#include <QPalette>

DiaryViewWindow::DiaryViewWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("日记");
    setWindowFlags(Qt::Window);
    resize(600, 550);

    // 整体背景
    this->setStyleSheet(
        "QWidget {"
        "  background: #FDF9F5;"
        "}"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 25, 30, 30);

    // 只读日记内容
    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setStyleSheet(
        "QTextEdit {"
        "  background-color: rgba(255, 255, 255, 0.7);"
        "  border: 1px solid #E8E0D8;"
        "  border-radius: 20px;"
        "  padding: 24px;"
        "  font-size: 15px;"
        "  line-height: 1.6;"
        "}"
        );
    mainLayout->addWidget(m_textEdit);
}

void DiaryViewWindow::loadDiary(const QString &date, const QString &html)
{
    QDate d = QDate::fromString(date, "yyyy-MM-dd");
    QString dateText = QLocale(QLocale::Chinese).toString(d, "yyyy年M月d日 dddd");
    setWindowTitle("日记 - " + dateText);

    if (html.isEmpty())
        m_textEdit->clear();
    else
        m_textEdit->setHtml(html);
}