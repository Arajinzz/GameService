#pragma once

#include <functional>
#include <chrono>
#include <thread>

namespace sService
{
  bool PollFor(std::function<bool()> pollingFor, std::chrono::milliseconds timeout = std::chrono::milliseconds(3000))
  {
    // this is a blocking call
    auto success = false;
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() <= timeout.count())
    {
      success = pollingFor();
      if (success)
        break;
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return success;
  }

} // namespace sService
