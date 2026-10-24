#pragma once
#include "audio/ISoundPlayer.h"
#include <string>

struct SoundFiles {
    std::string searching;
    std::string found;
};

// Plays WAV files with Windows PlaySound. Only one sound at a time, which is enough because the search
// sound stops when the goal is found. Stays silent if a file is missing (SND_NODEFAULT).
class WinSoundPlayer : public ISoundPlayer {
public:
    explicit WinSoundPlayer(SoundFiles files);

    void playLoop(SoundId sound) override;
    void playOnce(SoundId sound) override;
    void stop() override;

private:
    const std::string& fileOf(SoundId sound) const;

    SoundFiles files_;
};
