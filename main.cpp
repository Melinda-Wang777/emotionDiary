#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDate>
#include <QMessageBox>
#include <QSqlQuery>
#include <QVector3D>
#include <QSqlError>
#include <QPushButton>
#include <QTextDocument>
#include <QSqlDatabase>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int fontId = QFontDatabase::addApplicationFont(":/resources/mingchao.otf");
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            QString fontFamily = fontFamilies.at(0);
            QFont defaultFont = a.font();
            defaultFont.setFamily(fontFamily);
            a.setFont(defaultFont);
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}