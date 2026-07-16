#pragma once

#include <vector>
#include <mutex>
#include <assert.h>

namespace sCore
{
  template <typename Class>
  class SingleInstance
  {
  public:
    SingleInstance()
      : m_mutex()
    {
      if (Instance)
        throw std::runtime_error("attempted to create a single instance twice");

      Instance = reinterpret_cast<Class*>(this);
      assert(Instance);

#ifdef _DEBUG
      std::lock_guard<std::mutex> lock(m_mutex);
      Instances.push_back(this);
#endif
    }

    ~SingleInstance()
    {
#ifdef _DEBUG
      std::lock_guard<std::mutex> lock(m_mutex);
      auto it = std::find(Instances.begin(), Instances.end(), this);
      Instances.erase(it);
#endif
      Instance = nullptr;
    }

    static Class* Get()
    {
      return Instance;
    }

  protected:
    inline static Class* Instance;
#ifdef _DEBUG
    inline static std::vector<SingleInstance*> Instances;
#endif

  private:
    std::mutex m_mutex;

  public:
    SingleInstance(SingleInstance const&) = delete;
    void operator=(SingleInstance const&) = delete;
  };
  
  // helper
  template <typename Class>
  inline Class* Instance()
  {
    return SingleInstance<Class>::Get();
  }

} // namespace sCore

