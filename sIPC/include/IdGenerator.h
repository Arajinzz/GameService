#include <atomic>
#include <cstdint>

namespace sIPC
{
  class IdGenerator
  {
  public:
    static uint64_t next()
    {
      return counter.fetch_add(1, std::memory_order_relaxed);
    }

  private:
    inline static std::atomic<uint64_t> counter{ 1 };
  };

} // namespace sIPC
