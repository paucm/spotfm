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
           soundfeeder.cpp \
           track.cpp \
           mainwindow.cpp \
           logindialog.cpp \
           metadatafetcher.cpp \
           aboutdialog.cpp \
           metadatawidget.cpp \
           stationwidget.cpp \
           playlistresolver.cpp \
           suggest.cpp \
           openal-audio.cpp \
           spotfmapp.cpp 

HEADERS += spotifysession.h \
           appkey.h \
           radio.h \
           soundfeeder.h \
           spotifyquery.h \
           chunk.h \
		   audio.h \
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
           spotfmapp.h \

FORMS += mainwindow.ui \
         logindialog.ui \
         metadatawidget.ui \
         stationwidget.ui

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
