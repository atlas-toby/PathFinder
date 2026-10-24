#include "audio/WinSoundPlayer.h"
#include <windows.h>
#include <mmsystem.h>

namespace {
constexpr DWORD kBaseFlags = SND_FILENAME | SND_ASYNC | SND_NODEFAULT;
}

WinSoundPlayer::WinSoundPlayer(SoundFiles files) : files_(std::move(files)) {}

void WinSoundPlayer::playLoop(SoundId sound) { PlaySoundA(fileOf(sound).c_str(), nullptr, kBaseFlags | SND_LOOP); }

void WinSoundPlayer::playOnce(SoundId sound) { PlaySoundA(fileOf(sound).c_str(), nullptr, kBaseFlags); }

void WinSoundPlayer::stop() { PlaySoundA(nullptr, nullptr, 0); }

const std::string& WinSoundPlayer::fileOf(SoundId sound) const {
    return sound == SoundId::Searching ? files_.searching : files_.found;
}
