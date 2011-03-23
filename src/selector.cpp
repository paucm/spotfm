#include <QMutableMapIterator>
#include <QList>
#include <QTime>

#include "selector.h"


Selector::Selector(QMap<int, QString> items)
{
    m_items = items;
    m_totalWeight = totalWeight();

    //init random generator
    qsrand(QTime::currentTime().msec());
}

Selector::~Selector()
{
}

unsigned int Selector::totalWeight()
{
    QList<int> keys = m_items.keys();
    QListIterator<int> i(keys);
    unsigned int total = 0;
    while (i.hasNext())
        total += i.next();
    return total;
}

void Selector::removeValue(const QString &value)
{
    QMutableMapIterator<int, QString> i(m_items);
    while (i.hasNext()) {
        i.next();
        if (i.value() == value) {
            i.remove();
            break;
        }
    }
    m_totalWeight = totalWeight();
}

QString Selector::getItem()
{
    unsigned int weight = (qrand() % m_totalWeight);
    unsigned int value = 0;

    QMapIterator<int, QString> i(m_items);
    while (i.hasNext()) {
        i.next();
        value += i.key();
        if (value > weight) {
            qDebug("Item selected: %s (%d)", 
                    i.value().toLocal8Bit().constData(), 
                    i.key());
            return i.value();
        }
    }
    return QString();
}

