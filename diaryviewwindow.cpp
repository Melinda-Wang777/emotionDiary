#include "diaryviewwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDate>

DiaryViewWindow::DiaryViewWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("时光胶囊");
    setWindowFlags(Qt::Window);
    resize(550, 650);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 15, 20, 20);

    // 只读日记内容
    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setStyleSheet(
        "QTextEdit {"
        "  border: none;"
        "  background: white;"
        "  font-size: 16px;"
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
