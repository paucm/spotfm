TEMPLATE = lib
CONFIG += warn_on thread qt staticlib

TARGET = ella

QT = core \
     xml \
     network 

DESTDIR = ../../build

SOURCES += ws.cpp \
           xmlquery.cpp \
           artist.cpp \
           tag.cpp

HEADERS += ../include/ella/ws.h \
           xmlquery.cpp \
           ../include/ella/artist.h \
           ../include/ella/tag.h 

INCLUDEPATH += ../include
