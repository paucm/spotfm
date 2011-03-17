#ifndef CHUNK_H
#define CHUNK_H

#include <QtCore/QMetaType>

struct Chunk
{
    void *m_data;
    int   m_dataFrames;
    int   m_rate;
    int   m_channels;
};

Q_DECLARE_METATYPE(Chunk)

#endif
