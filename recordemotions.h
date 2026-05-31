#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QDateEdit>

#include <QPointer>
#include "diarywindow.h"

class DiaryWindow;

class RecordWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RecordWindow(QWidget *parent = nullptr);
    ~RecordWindow();

    QVector3D getEmotionByDate(const QString &date);

private slots:
    void onSliderChanged();
    void onSaveButtonClicked();

private:
    void initDatabase();
    void loadEmotion(const QString &date);
    void updatePreview();
    void saveCurrentEmotion();

    QSlider *m_sliderP;
    QSlider *m_sliderA;
    QSlider *m_sliderD;
    QLabel *m_previewLabel;
    QLabel *m_valueLabel;
    QDateEdit *m_dateEdit;
    QPushButton *m_saveBtn;

    QPushButton *m_diaryBtn;
    QPointer<DiaryWindow> m_diaryWindow;
};

#endif // RECORDWINDOW_H
