#include <QEvent>
#include <QKeyEvent>
#include <QTreeWidget>
#include <QTimer>
#include <QHeaderView>
#include <QNetworkReply>

#include <ella/artist.h>
#include <ella/track.h>

#include "suggest.h"

SuggestCompletion::SuggestCompletion(QLineEdit *parent)
    : QObject(parent)
    , m_editor(parent)
{
    m_popup = new QTreeWidget;
    m_popup->setWindowFlags(Qt::Popup);
    m_popup->setFocusPolicy(Qt::NoFocus);
    m_popup->setFocusProxy(parent);
    m_popup->setMouseTracking(true);

    m_popup->setColumnCount(1);
    m_popup->setUniformRowHeights(true);
    m_popup->setRootIsDecorated(false);
    m_popup->setEditTriggers(QTreeWidget::NoEditTriggers);
    m_popup->setSelectionBehavior(QTreeWidget::SelectRows);
    m_popup->setFrameStyle(QFrame::Box | QFrame::Plain);
    m_popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_popup->header()->hide();

    m_popup->installEventFilter(this);

    connect(m_popup, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            SLOT(doneCompletion()));

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(500);
    connect(m_timer, SIGNAL(timeout()), SLOT(autoSuggest()));

    connect(m_editor, SIGNAL(textEdited(QString)), m_timer, SLOT(start()));
}

SuggestCompletion::~SuggestCompletion()
{
    delete m_popup;
}

bool SuggestCompletion::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj != m_popup)
        return false;

    if (ev->type() == QEvent::MouseButtonPress) {
        m_popup->hide();
        m_editor->setFocus();
        return true;
    }

    if (ev->type() == QEvent::KeyPress) {

        bool consumed = false;
        int key =
            static_cast<QKeyEvent*>(ev)->key();
        switch (key) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
                doneCompletion();
                consumed = true;

            case Qt::Key_Escape:
                m_editor->setFocus();
                m_popup->hide();
                consumed = true;

            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Home:
            case Qt::Key_End:
            case Qt::Key_PageUp:
            case Qt::Key_PageDown:
                break;

            default:
                m_editor->setFocus();
                m_editor->event(ev);
                m_popup->hide();
                break;
        }
        return consumed;
    }
    return false;
}

void SuggestCompletion::showCompletion(const QStringList &choices)
{
    if (choices.isEmpty()) return;

    m_popup->setUpdatesEnabled(false);
    m_popup->clear();
    for (int i=0; i < choices.count(); ++i) {
        QTreeWidgetItem *item;
        item = new QTreeWidgetItem(m_popup);
        item->setText(0, choices[i]);
    }
    m_popup->setCurrentItem(m_popup->topLevelItem(0));
    m_popup->resizeColumnToContents(0);
    m_popup->adjustSize();
    m_popup->setUpdatesEnabled(true);

    int h = m_popup->sizeHintForRow(0) * qMin(10,choices.count()) + 3;
    m_popup->resize(m_popup->width(), h);

    m_popup->move(m_editor->mapToGlobal(QPoint(0, m_editor->height())));
    m_popup->setFocus();
    m_popup->show();
}

void SuggestCompletion::doneCompletion()
{
    m_timer->stop();
    m_popup->hide();
    m_editor->setFocus();
    QTreeWidgetItem *item = m_popup->currentItem();
    if (item) {
        m_editor->setText(item->text(0));
        QMetaObject::invokeMethod(m_editor, "returnPressed");
    }
}

void SuggestCompletion::preventSuggest()
{
    m_timer->stop();
}

/*** ArtistSuggest ***/

ArtistSuggest::ArtistSuggest(QLineEdit *parent)
    : SuggestCompletion(parent)
{
}

ArtistSuggest::~ArtistSuggest()
{
}

void ArtistSuggest::autoSuggest()
{
    QNetworkReply *reply = ella::Artist::search(text());
    connect(reply, SIGNAL(finished()), SLOT(onGotSearch()));
}

void ArtistSuggest::onGotSearch()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }
    QList<ella::Artist> artists = ella::Artist::list(reply);
    QStringList choices;
    for (int i=0; i < artists.size(); ++i) {
        choices << artists[i].name();
    }
    showCompletion(choices);
}

/*** TrackSuggest ***/

TrackSuggest::TrackSuggest(QLineEdit *parent)
    : SuggestCompletion(parent)
{
}

TrackSuggest::~TrackSuggest()
{
}

void TrackSuggest::autoSuggest()
{
    QNetworkReply *reply = ella::Track::search(text());
    connect(reply, SIGNAL(finished()), SLOT(onGotSearch()));
}

void TrackSuggest::onGotSearch()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }
    QList<ella::Track> tracks = ella::Track::list(reply);
    QStringList choices;
    for (int i = 0; i < tracks.size(); ++i) {
        ella::Track track = tracks[i];
        choices << QString("%1 - %2").arg(track.artistName()).arg(track.title());
    }
    showCompletion(choices);
}
