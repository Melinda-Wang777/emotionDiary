#include "mainwindow.h"

#include <QApplication>
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
#include <QtSql/QSqlDataBase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
