#ifndef SPOTFM_UTILS_H
#define SPOTFM_UTILS_H

enum RadioState
{
    Stopped = 0,
    Playing,
    Paused
};

enum RadioError
{
    InvalidStation,
    InvalidTrack,
    PlaylistEnded,
    PlaybackError
};

#endif

