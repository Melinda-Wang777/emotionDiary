#ifndef DIARYWINDOW_H
#define DIARYWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QFontComboBox>
#include <QComboBox>
#include <QPushButton>
#include <QToolBar>
#include <QDate>
#include <QCloseEvent>

class DiaryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DiaryWindow(QWidget *parent = nullptr);
    ~DiaryWindow();

    void loadDiary(const QString &date);
    void saveDiary();

    void closeEvent(QCloseEvent *event) override;

signals:
    void diarySaved();

private slots:
    void onBoldToggled(bool checked);
    void onItalicToggled(bool checked);
    void onUnderlineToggled(bool checked);
    void onStrikeOutToggled(bool checked);
    void onFontChanged(const QFont &font);
    void onFontSizeChanged(int index);
    void onInsertImage();

private:
    void setupToolBar();
    void mergeFormatOnSelection(const QTextCharFormat &format);

    bool m_isClosing;

    QTextEdit *m_textEdit;
    QToolBar *m_toolBar;
    QFontComboBox *m_fontCombo;
    QComboBox *m_fontSizeCombo;
    QPushButton *m_boldBtn;
    QPushButton *m_italicBtn;
    QPushButton *m_underlineBtn;
    QPushButton *m_strikeOutBtn;
    QPushButton *m_imageBtn;

    QPushButton *m_undoBtn;
    QPushButton *m_redoBtn;

    QString m_currentDate;
};

#endif // DIARYWINDOW_H
