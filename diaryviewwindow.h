#ifndef DIARYVIEWWINDOW_H
#define DIARYVIEWWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>

class DiaryViewWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DiaryViewWindow(QWidget *parent = nullptr);
    void loadDiary(const QString &date, const QString &html);

private:
    QPushButton *m_closeBtn;
    QTextEdit *m_textEdit;
};

#endif // DIARYVIEWWINDOW_H
