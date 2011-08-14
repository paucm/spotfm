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
