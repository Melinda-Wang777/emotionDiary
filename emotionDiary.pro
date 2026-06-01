QT += widgets core sql

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    basewindow.cpp \
    calendarview.cpp \
    diaryviewwindow.cpp \
    diarywindow.cpp \
    linecharts.cpp \
    main.cpp \
    mainwindow.cpp \
    recordemotions.cpp

HEADERS += \
    basewindow.h \
    calendarview.h \
    diaryviewwindow.h \
    diarywindow.h \
    linecharts.h \
    mainwindow.h \
    recordemotions.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc