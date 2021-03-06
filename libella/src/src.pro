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
           track.cpp \
           tag.cpp \
           util.cpp

HEADERS += ../include/ella/ws.h \
           ../include/ella/xmlquery.h \
           ../include/ella/artist.h \
           ../include/ella/track.h \
           ../include/ella/tag.h \
           ../include/ella/util.h

INCLUDEPATH += ../include
