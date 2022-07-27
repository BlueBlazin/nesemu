#ifndef SRC_NES_AUDIO_STREAM_H_
#define SRC_NES_AUDIO_STREAM_H_

#include <cstdint>
#include <queue>
#include <vector>

#include "SFML/Audio.hpp"

namespace nes {

class AudioStream : public sf::SoundStream {
 public:
  AudioStream() { initialize(1, 44100); }

  void QueueAudio(std::vector<int16_t> samples) { q.push(std::move(samples)); }

 private:
  bool onGetData(Chunk& data) override {
    std::vector<int16_t> samples = q.front();
    q.pop();
    data.sampleCount = samples.size();
    data.samples = samples.data();

    return true;
  }

  void onSeek(sf::Time timeOffset) override {
    // nop
  }

  std::queue<std::vector<int16_t>> q;
};

}  // namespace nes

#endif  // SRC_NES_AUDIO_STREAM_H_