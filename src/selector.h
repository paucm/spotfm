#ifndef SELECTOR_H
#define SELECTOR_H

#include <QMap>
#include <QString>

class Selector
{
    public:
        Selector(QMap<int, QString> items);
        ~Selector();

        QString getItem();
        void removeValue(const QString &value);
        unsigned int size() const { return m_items.size(); }

    private:
        unsigned int totalWeight();

        QMap<int, QString> m_items;
        unsigned int m_totalWeight;
};

#endif

