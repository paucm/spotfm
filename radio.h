#ifndef _RADIO_H
#define _RADIO_H

#include <QObject>
#include <QMutex>
#include <QQueue>
#include <QBuffer>
#include <QWaitCondition>

#include <alsa/asoundlib.h>

class SoundFeeder;

class Radio : public QObject {
  Q_OBJECT
  public:
    Radio();
    ~Radio();
  
    enum State { 
      Stopped,
      Playing
    };
    State state() const { return m_state; }
    
    snd_pcm_t * pcmHandle() const { return m_snd; }
    QMutex &pcmMutex() { return m_pcmMutex; }
    QMutex &dataMutex() { return m_dataMutex; }
    QWaitCondition &pcmWaitCondition() { return m_pcmWaitCondition; }
    QWaitCondition &playCondition() { return m_playCondition; }
    
  public slots:
    void play();
    void stop();
    void skip();
    
  private:
    void initSound();
    
    
    snd_pcm_t            *m_snd;
    QMutex                m_pcmMutex;
    QMutex                m_dataMutex;
    QWaitCondition        m_pcmWaitCondition;
    QWaitCondition        m_playCondition;
    QQueue<Chunk>         m_data;
    SoundFeeder          *m_soundFeeder;
    
    Radio::State m_state;
};





#endif

