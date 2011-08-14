#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QLineEdit>
#include "suggest.h"

class SuggestCompletion;

class SearchBox : public QLineEdit
{
    Q_OBJECT

    public:
        SearchBox(QWidget *parent = 0)
            : QLineEdit(parent)
        {
            m_completer = new ArtistSuggest(this);
        }

        void onCompleted()
        {
            m_completer->preventSuggest();
        }

    private:
        SuggestCompletion *m_completer;
};

#endif //SEARCHBOX_H

