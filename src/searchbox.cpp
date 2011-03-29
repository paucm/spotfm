

#include "searchbox.h"
#include "suggest.h"

SearchBox::SearchBox(QWidget *parent)
    : QLineEdit(parent)
{
    m_completer = new ArtistSuggest(this);
}

void SearchBox::onCompleted()
{
    m_completer->preventSuggest();
}

void SearchBox::onSuggestChanged(const QString &text)
{
    m_completer->preventSuggest();
    delete m_completer;
    if(text == "Artist") {
        m_completer = new ArtistSuggest(this);
    }
    else {
        m_completer = new TagSuggest(this);
    }
}
