/**
 * @file        rex/audio/silent_audio_driver.h
 * @brief       Audio driver that plays nothing, at the pace of a real device.
 *
 * @copyright   Copyright (c) 2026 the ReXGlue SDK contributors (Ridge Racer 6
 *              recompilation fork)
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */
#pragma once

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>

#include <rex/audio/audio_driver.h>
#include <rex/thread.h>

namespace rex::audio {

// Used when no audio device can be opened (none present, or the system's sound
// service is not running). A game registers its audio client expecting that to
// succeed, as it always does on the console; failing it leaves games with a
// null audio object and they crash. This driver accepts every frame and hands
// the client's semaphore back one frame (256 samples at 48 kHz) at a time, as
// a device playing the sound would, so the game runs normally without sound.
class SilentAudioDriver : public AudioDriver {
 public:
  SilentAudioDriver(memory::Memory* memory, rex::thread::Semaphore* semaphore);
  ~SilentAudioDriver() override;

  void SubmitFrame(uint32_t samples_ptr) override;

 private:
  void Run();

  rex::thread::Semaphore* semaphore_;
  std::mutex mutex_;
  std::condition_variable wake_;
  uint64_t submitted_ = 0;  // frames given to the driver
  bool stop_ = false;
  std::thread thread_;
};

}  // namespace rex::audio
