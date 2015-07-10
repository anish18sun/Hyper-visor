#-----------------------------------------------------
# Space Invaders game .pro file
# AUTHOR : ANISH SINGH
#-----------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET   = SpaceInvaders
TEMPLATE = app

SOURCES += main.cpp GameWidget.cpp \
        Bullet.cpp \
        MainShip.cpp \
        EnemyShip.cpp

HEADERS += GameWidget.h \
        Bullet.h \
        MainShip.h \
        EnemyShip.h

RESOURCES += images.qrc \
    sounds.qrc
