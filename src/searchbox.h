#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QLineEdit>

class SuggestCompletion;

class SearchBox : public QLineEdit
{
    Q_OBJECT

    public:
        SearchBox(QWidget *parent = 0);

        void onCompleted();

    public slots:
        void onSuggestChanged(const QString &text);

    private:
        SuggestCompletion *m_completer;
};

#endif //SEARCHBOX_H

