#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>

#include "aboutdialog.h"

static inline QLabel* label( const QString& text, Qt::WidgetAttribute size = Qt::WA_MacSmallSize )
{
    QLabel* l = new QLabel( text );
    l->setAttribute( size );
    l->setOpenExternalLinks( true );
    return l;
}

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    const char *authors = "Pau Capella <pcapellam@gmail.com>";
    QLabel* lauthors;

    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( new QLabel( "<b>" + qApp->applicationName() ) );
    v->addWidget( label( qApp->applicationVersion() ) );
    v->addSpacing( 10 );
    v->addWidget(label("<a href='https://github.com/paucm/spotfm'>github.com/paucm/spotfm</a>"));
    v->addSpacing( 10 );
    v->addWidget( lauthors = label( authors, Qt::WA_MacMiniSize ) );
    v->addSpacing( 10 );
    v->addWidget(new QLabel("This product uses SPOTIFY CORE but is not endorsed,\ncertified or otherwise approved in any way by Spotify.\nSpotify is the registered trade mark of the Spotify Group"));
    v->addWidget(new QLabel(QString("<html><img src=\"%1\"></html>").arg(":/icons/icons/spotify-core-logo-128x128.png")));
    

    v->setSizeConstraint( QLayout::SetFixedSize );
    v->setSpacing( 2 );
    lauthors->setTextInteractionFlags( Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse );
    setWindowTitle(tr("About"));
}
