#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

#include <ella/util.h>

#include "playlistcontrol.h"

PlaylistControl *PlaylistControl::get(const QString &name)
{
    PlaylistControl *ctrl = 0;
    if (name == "Artist")
        ctrl = new ArtistControl();
    else if (name == "Mood")
        ctrl = new MoodControl();
    else if (name == "Genre")
        ctrl = new GenreControl();
    else if (name == "Speed")
        ctrl = new SpeedControl();
    else
        ctrl = new ArtistControl();
    ctrl->init();
    return ctrl;
}

QStringList PlaylistControl::controls()
{
    return QStringList() << "Artist" << "Mood" << "Genre" << "Speed";
}

ArtistControl::ArtistControl()
    : PlaylistControl("Artist")
{
}

void ArtistControl::init() {

    m_match = new QComboBox();
    QComboBox *matchCb = dynamic_cast<QComboBox *>(m_match);
    matchCb->addItem(tr("Similar To"));
    matchCb->addItem(tr("Limit To"));

    m_input =  new QLineEdit();
    QLineEdit *inputLe = dynamic_cast<QLineEdit *>(m_input);
    #if QT_VERSION >= 0x040700
    inputLe->setPlaceholderText(tr("Artist name"));
    #endif
    inputLe->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
}

ella::Track::SearchParamEntry ArtistControl::toParam() const
{
    QLineEdit *inputLe = dynamic_cast<QLineEdit *>(m_input);
    return ella::Track::SearchParamEntry(ella::Track::ArtistName, inputLe->text());
}

QString ArtistControl::needSimilarityQuery(bool *ok) const
{
    QComboBox *matchCb = dynamic_cast<QComboBox *>(m_match);
    *ok = matchCb->currentText() == "Similar To";
    QLineEdit *inputLe = dynamic_cast<QLineEdit *>(m_input);
    return inputLe->text();
}


/*** Mood Control ***/

MoodControl::MoodControl()
    : PlaylistControl("Mood")
{
}

void MoodControl::init()
{
    m_match = new QLabel(tr("is"));
    m_input = new QComboBox();
    QComboBox *inputCb =  dynamic_cast<QComboBox *>(m_input);
    inputCb->addItem(tr("Party"), ella::Util::Party);
    inputCb->addItem(tr("Happy"), ella::Util::Happy);
    inputCb->addItem(tr("Furious"), ella::Util::Furious);
    inputCb->addItem(tr("Acoustic"), ella::Util::Acoustic);
    inputCb->addItem(tr("Relax"), ella::Util::Relax);
    inputCb->addItem(tr("Blue"), ella::Util::Blue);
}

ella::Track::SearchParamEntry MoodControl::toParam() const
{
    QComboBox *inputCb =  dynamic_cast<QComboBox *>(m_input);
    int index = inputCb->currentIndex();
    return ella::Track::SearchParamEntry(ella::Track::Mood, inputCb->itemData(index));
}

/*** Genre Control ***/

GenreControl::GenreControl()
    : PlaylistControl("Mood")
{
}

void GenreControl::init()
{
    m_match = new QLabel(tr("is"));
    m_input = new QComboBox();
    QComboBox *inputCb =  dynamic_cast<QComboBox *>(m_input);
    inputCb->addItem(tr("Alternative"));
    inputCb->addItem(tr("Blues"));
    inputCb->addItem(tr("Country"));
    inputCb->addItem(tr("Disco"));
    inputCb->addItem(tr("Funk"));
    inputCb->addItem(tr("Metal"));
    inputCb->addItem(tr("Rap"));
    inputCb->addItem(tr("House"));
    inputCb->addItem(tr("Jazz"));
    inputCb->addItem(tr("Pop"));
    inputCb->addItem(tr("Punk"));
    inputCb->addItem(tr("Reggae"));
    inputCb->addItem(tr("Rock"));
    inputCb->addItem(tr("Soul"));
    inputCb->addItem(tr("Techno"));
}

ella::Track::SearchParamEntry GenreControl::toParam() const
{
    QComboBox *inputCb =  dynamic_cast<QComboBox *>(m_input);
    QString genre = inputCb->currentText().replace(" ", "");
    return ella::Track::SearchParamEntry(ella::Track::Genre, genre);
}

/*** Speed control ***/

SpeedControl::SpeedControl()
 : PlaylistControl("Speed")
{
}

void SpeedControl::init()
{
    m_match = new QLabel(tr("is"));
    m_input = new QComboBox();
    QComboBox *inputCb =  dynamic_cast<QComboBox *>(m_input);
    inputCb->addItem(tr("Fast"), ella::Util::Fast);
    inputCb->addItem(tr("Medium"), ella::Util::Medium);
    inputCb->addItem(tr("Slow"), ella::Util::Slow);
}

ella::Track::SearchParamEntry SpeedControl::toParam() const
{
    QComboBox *inputCb =  dynamic_cast<QComboBox *>(m_input);
    int index = inputCb->currentIndex();
    return ella::Track::SearchParamEntry(ella::Track::Speed, inputCb->itemData(index));
}
