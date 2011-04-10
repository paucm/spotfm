#ifndef SUGGEST_H
#define SUGGEST_H

#include <QObject>
#include <QLineEdit>
#include <QStringList>

class QTreeWidget;
class QTimer;
class QEvent;

class SuggestCompletion : public QObject
{
    Q_OBJECT

    public:
        SuggestCompletion(QLineEdit *parent = 0);
        ~SuggestCompletion();

        bool eventFilter(QObject *obj, QEvent *ev);
        void showCompletion(const QStringList &choices);

    public slots:
        void doneCompletion();
        void preventSuggest();
        virtual void autoSuggest() = 0;

    protected:
        QString text() const { return m_editor->text(); }

    private:
        QLineEdit *m_editor;
        QTreeWidget *m_popup;
        QTimer *m_timer;
};

class ArtistSuggest : public SuggestCompletion
{
    Q_OBJECT

    public:
        ArtistSuggest(QLineEdit *parent = 0);
        ~ArtistSuggest();

    public slots:
        void autoSuggest();

    private slots:
        void onGotSearch();
};

class TrackSuggest : public SuggestCompletion
{
    Q_OBJECT

    public:
        TrackSuggest(QLineEdit *parent = 0);
        ~TrackSuggest();

    public slots:
        void autoSuggest();

    private slots:
        void onGotSearch();
};

#endif //SUGGEST_H

