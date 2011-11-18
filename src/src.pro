TEMPLATE = app
TARGET = spotfm 
DEPENDPATH += .
INCLUDEPATH += . ../libella/include
LIBS += -L../build -lella
QT = core gui xml network
DESTDIR = ../build
PRE_TARGETDEPS = ../build/libella.a

SOURCES += radio.cpp \
           spotifysession.cpp \
           spotifyquery.cpp \
           main.cpp \
           track.cpp \
           mainwindow.cpp \
           audiocontroller.cpp \
           logindialog.cpp \
           metadatafetcher.cpp \
           aboutdialog.cpp \
           metadatawidget.cpp \
           stationwidget.cpp \
           playlistresolver.cpp \
           suggest.cpp \
           openal-audio.cpp \
           spotfmapp.cpp \
           settingsdialog.cpp \
           extensions/lastfm/lastfmwidget.cpp \
           extensions/lastfm/lastfmextension.cpp \
           extensions/lastfm/lastfmservice.cpp

HEADERS += spotifysession.h \
           appkey.h \
           radio.h \
           spotifyquery.h \
           chunk.h \
		   audiocontroller.h \
           track.h \
           mainwindow.h \
           logindialog.h \
           metadatafetcher.h \
           aboutdialog.h \
           metadatawidget.h \
           stationwidget.h \
           playlistresolver.h \
           spinnerlabel.h \
           suggest.h \
           searchbox.h \
           openal-audio.h \
           util.h \
           spotfmapp.h \
           settingsdialog.h \
           extension.h \
           extensions/lastfm/lastfmwidget.h \
           extensions/lastfm/lastfmextension.h \
           extensions/lastfm/lastfmservice.h

FORMS += mainwindow.ui \
         logindialog.ui \
         metadatawidget.ui \
         stationwidget.ui \
         extensions/lastfm/lastfmwidget.ui

RESOURCES += spotfm.qrc 

win32 {
    INCLUDEPATH += ../../libspotify-0.0.7-win32/include \
                   ../../openal-soft-1.13-bin/include
    LIBS += ../../libspotify-0.0.7-win32/lib/libspotify.lib \
            -L ../../openal-soft-1.13-bin/lib/Win32 \
            -lOpenAL32.dll
}

unix {
    LIBS += -L/usr/local/lib \
            -lspotify \
            -lopenal
}
