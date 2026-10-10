/**
 * @file        audio/silent_audio_driver.cpp
 * @brief       Audio driver that plays nothing, at the pace of a real device.
 *
 * @copyright   Copyright (c) 2026 the ReXGlue SDK contributors (Ridge Racer 6
 *              recompilation fork)
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#include <rex/audio/silent_audio_driver.h>

#include <algorithm>
#include <chrono>

namespace rex::audio {

namespace {

// One frame is 256 samples per channel at 48 kHz, as in the SDL driver.
constexpr std::chrono::nanoseconds kFrameDuration{256LL * 1'000'000'000LL / 48'000};

}  // namespace

SilentAudioDriver::SilentAudioDriver(memory::Memory* memory, rex::thread::Semaphore* semaphore)
    : AudioDriver(memory), semaphore_(semaphore) {
  thread_ = std::thread([this] { Run(); });
}

SilentAudioDriver::~SilentAudioDriver() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    stop_ = true;
  }
  wake_.notify_all();
  if (thread_.joinable()) {
    thread_.join();
  }
}

void SilentAudioDriver::SubmitFrame(uint32_t samples_ptr) {
  (void)samples_ptr;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    ++submitted_;
  }
  wake_.notify_all();
}

// Plays the submitted frames back to back in real time: each one is "done"
// kFrameDuration after the previous one, or after it arrived if the device ran
// dry. The time is kept from a fixed start, so coarse sleeps (15.6 ms timer
// ticks on Windows) do not slow the pace down; several frames are then
// released at once.
void SilentAudioDriver::Run() {
  rex::thread::set_current_thread_name("Silent Audio");
  using Clock = std::chrono::steady_clock;
  uint64_t played = 0;
  Clock::time_point next_done{};
  std::unique_lock<std::mutex> lock(mutex_);
  for (;;) {
    wake_.wait(lock, [this, played] { return stop_ || submitted_ > played; });
    if (stop_) {
      return;
    }
    const Clock::time_point now = Clock::now();
    if (next_done < now - kFrameDuration) {
      next_done = now + kFrameDuration;  // ran dry: start again from now
    }
    if (wake_.wait_until(lock, next_done, [this] { return stop_; })) {
      return;
    }
    uint32_t done = 0;
    const Clock::time_point after = Clock::now();
    while (played < submitted_ && next_done <= after) {
      ++played;
      ++done;
      next_done += kFrameDuration;
    }
    if (done) {
      lock.unlock();
      semaphore_->Release(static_cast<int>(done), nullptr);
      lock.lock();
    }
  }
}

}  // namespace rex::audio
