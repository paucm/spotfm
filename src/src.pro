TEMPLATE = app
TARGET = spotfm 
DEPENDPATH += .
INCLUDEPATH += . ../libella/include
LIBS += -L../build -lella
QT = core gui xml network
DESTDIR = ../build

SOURCES += radio.cpp \
           spotifysession.cpp \
           spotifyquery.cpp \
           main.cpp \
           soundfeeder.cpp \
           track.cpp \
           station.cpp \
           mainwindow.cpp \
           logindialog.cpp \
           albumimagefetcher.cpp \
           selector.cpp \
           spotfmapp.cpp  

HEADERS += spotifysession.h \
           appkey.h \
           radio.h \
           soundfeeder.h \
           spotifyquery.h \
           chunk.h \
		   audio.h \
           track.h \
           station.h \
           mainwindow.h \
           logindialog.h \
           albumimagefetcher.h \
           selector.h \  
           spotfmapp.h

FORMS += mainwindow.ui \
         logindialog.ui

RESOURCES += spotfm.qrc 

win32 {
    SOURCES += openal-audio.cpp 
    HEADERS += openal-audio.h
    INCLUDEPATH += ../libspotify-0.0.7-win32/include \
                   ../openal-soft-1.13-bin/include
    LIBS += ../libspotify-0.0.7-win32/lib/libspotify.lib \
            -L ../openal-soft-1.13-bin/lib/Win32 \
            -lOpenAL32.dll
}

unix {
    SOURCES += alsa-audio.cpp
    HEADERS += alsa-audio.h
    LIBS += -L/usr/local/lib \
            -lspotify \
            -lasound 
}
