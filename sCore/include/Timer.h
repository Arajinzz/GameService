#pragma once

#include <chrono>

namespace sCore
{
  class Timer
  {
  public:
    typedef std::chrono::high_resolution_clock Clock;

    Timer()
      : m_epoch()
      , m_deltaTime()
    {

    }

    void Start()
    {
      m_epoch = Clock::now();
    }

    void Tick()
    {
      m_deltaTime = Clock::now() - m_epoch;
      m_epoch = Clock::now();
    }

    double deltaTime() const
    {
      auto count = m_deltaTime.count();
      return count * 1e-9; // nanoseconds to seconds
    }

  private:
    Clock::time_point m_epoch;
    Clock::duration m_deltaTime;

  };

} // namespace sCore
