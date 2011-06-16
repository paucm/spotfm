#ifndef PLAYLISTCONTROL_H
#define PLAYLISTCONTROL_H

#include <QObject>
#include <QWidget>
#include <QStringList>

#include <ella/track.h>

class QWidget;

class PlaylistControl : public QObject {

    Q_OBJECT

    public:
        PlaylistControl(const QString &name)
            : m_name(name)
        {
            m_match = 0;
            m_input = 0;
        }

        virtual ~PlaylistControl()
        {
            if(m_match)
                delete m_match;
            if(m_input)
                delete m_input;
        }

        virtual void init() = 0;

        QString name() const { return m_name; }

        virtual QWidget *match() { return m_match; }
        virtual QWidget *input() { return m_input; }
        virtual ella::Track::SearchParamEntry toParam() const = 0;
        virtual QString needSimilarityQuery(bool *ok) const { *ok=false; return QString(); }

        static PlaylistControl *get(const QString &name = QString());
        static QStringList controls();



   protected:
        QWidget *m_input;
        QWidget *m_match;

   private:
        QString m_name;
};


class ArtistControl : public PlaylistControl {

    Q_OBJECT

    public:
        ArtistControl();
        ~ArtistControl() {}

        void init();
        ella::Track::SearchParamEntry toParam() const;
        QString needSimilarityQuery(bool *ok) const;
};


class MoodControl : public PlaylistControl {

    Q_OBJECT

    public:
        MoodControl();
        ~MoodControl() {};

        void init();
        ella::Track::SearchParamEntry toParam() const;
};


class GenreControl : public PlaylistControl {

    Q_OBJECT

    public:
        GenreControl();
        ~GenreControl() {}

        void init();
        ella::Track::SearchParamEntry toParam() const;
};


class SpeedControl : public PlaylistControl {

    Q_OBJECT

public:
    SpeedControl();
    ~SpeedControl() {}

    void init();
    ella::Track::SearchParamEntry toParam() const;
};

#endif
