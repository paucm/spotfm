#include "radio.h"

void Radio::initSound()
{
  int d = 0;
  snd_pcm_uframes_t periodSize = 1024;
  snd_pcm_uframes_t bufferSize = periodSize * 4;

  snd_pcm_hw_params_t *hwParams;
  snd_pcm_open(&m_snd, "default", SND_PCM_STREAM_PLAYBACK, 0);
  snd_pcm_hw_params_malloc(&hwParams);
  snd_pcm_hw_params_any(m_snd, hwParams);
  snd_pcm_hw_params_set_access(m_snd, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(m_snd, hwParams, SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_rate(m_snd, hwParams, 44100, 0);
  snd_pcm_hw_params_set_channels(m_snd, hwParams, 2);
  snd_pcm_hw_params_set_period_size_near(m_snd, hwParams, &periodSize, &d);
  snd_pcm_hw_params_set_buffer_size_near(m_snd, hwParams, &bufferSize);
  snd_pcm_hw_params(m_snd, hwParams);
  snd_pcm_hw_params_free(hwParams);

  snd_pcm_sw_params_t *swParams;
  snd_pcm_sw_params_malloc(&swParams);
  snd_pcm_sw_params_current(m_snd, swParams);
  snd_pcm_sw_params_set_avail_min(m_snd, swParams, periodSize);
  snd_pcm_sw_params_set_start_threshold(m_snd, swParams, 0);
  snd_pcm_sw_params(m_snd, swParams);
  snd_pcm_sw_params_free(swParams);

  snd_pcm_prepare(m_snd);
}
