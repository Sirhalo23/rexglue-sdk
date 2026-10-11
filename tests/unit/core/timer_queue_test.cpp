#include <chrono>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <rex/thread/timer_queue.h>

namespace {
using namespace std::chrono_literals;
using rex::thread::QueueTimerOnce;
using rex::thread::QueueTimerRecurring;
using Clock = rex::thread::TimerQueueWaitItem::clock;

struct ScopedTimer {
  std::weak_ptr<rex::thread::TimerQueueWaitItem> item;
  ~ScopedTimer() {
    if (auto live = item.lock()) live->Disarm();
  }
};
}

TEST_CASE("timer insertion interrupts an idle or later deadline wait", "[timer_queue]") {
  ScopedTimer later;
  SECTION("empty queue") {}
  SECTION("later deadline") {
    later.item = QueueTimerOnce([](void*) {}, nullptr, Clock::now() + 2s);
  }
  // Let the dispatcher enter its idle wait or its later deadline wait.
  std::this_thread::sleep_for(20ms);
  auto promise = std::make_shared<std::promise<Clock::time_point>>();
  auto result = promise->get_future();
  auto due = Clock::now() + 10ms;
  ScopedTimer timer{QueueTimerOnce(
      [promise](void*) { promise->set_value(Clock::now()); }, nullptr, due)};
  auto status = result.wait_for(500ms);
  CHECK(status == std::future_status::ready);
  if (status == std::future_status::ready) CHECK(result.get() >= due);
}

TEST_CASE("a recurring timer can disarm itself", "[timer_queue]") {
  auto item = std::make_shared<std::promise<std::weak_ptr<rex::thread::TimerQueueWaitItem>>>();
  auto assigned = item->get_future().share();
  auto called = std::make_shared<std::promise<void>>();
  auto result = called->get_future();
  ScopedTimer timer{QueueTimerRecurring(
      [assigned, called](void*) {
        if (auto live = assigned.get().lock()) live->Disarm();
        called->set_value();
      }, nullptr, Clock::now(), 1ms)};
  item->set_value(timer.item);
  CHECK(result.wait_for(500ms) == std::future_status::ready);
  // A second callback would set the promise twice and fail the process.
  std::this_thread::sleep_for(20ms);
}

TEST_CASE("disarming waits for an active timer callback", "[timer_queue]") {
  auto entered = std::make_shared<std::promise<void>>();
  auto entered_result = entered->get_future();
  auto release = std::make_shared<std::promise<void>>();
  auto release_result = release->get_future().share();
  ScopedTimer timer{QueueTimerOnce(
      [entered, release_result](void*) {
        entered->set_value();
        release_result.wait();
      }, nullptr, Clock::now())};
  auto status = entered_result.wait_for(500ms);
  CHECK(status == std::future_status::ready);
  if (status != std::future_status::ready) {
    release->set_value();
    return;
  }
  auto live = timer.item.lock();
  REQUIRE(live);
  auto disarm_started = std::make_shared<std::promise<void>>();
  auto started_result = disarm_started->get_future();
  auto completed = std::async(std::launch::async, [live, disarm_started] {
    disarm_started->set_value();
    live->Disarm();
  });
  started_result.wait();
  CHECK(completed.wait_for(20ms) == std::future_status::timeout);
  release->set_value();
  CHECK(completed.wait_for(500ms) == std::future_status::ready);
}

TEST_CASE("timer queue wakes concurrent producers through ring wraparound", "[timer_queue]") {
  constexpr size_t producers = 4;
  constexpr size_t per_producer = 300;
  struct State {
    std::mutex mutex;
    std::condition_variable cv;
    size_t count = 0;
  };
  auto state = std::make_shared<State>();
  std::vector<std::thread> threads;
  for (size_t producer = 0; producer < producers; ++producer) {
    threads.emplace_back([state] {
      for (size_t i = 0; i < per_producer; ++i) {
        QueueTimerOnce([state](void*) {
          std::lock_guard lock(state->mutex);
          ++state->count;
          state->cv.notify_all();
        }, nullptr, Clock::now());
      }
    });
  }
  for (auto& thread : threads) thread.join();
  std::unique_lock lock(state->mutex);
  CHECK(state->cv.wait_for(lock, 2s, [&] {
    return state->count == producers * per_producer;
  }));
}
