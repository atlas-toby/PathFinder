#pragma once

enum class SoundId { Searching, Found };

class ISoundPlayer {
public:
    virtual ~ISoundPlayer() = default;
    virtual void playLoop(SoundId sound) = 0;
    virtual void playOnce(SoundId sound) = 0;
    virtual void stop() = 0;
};
