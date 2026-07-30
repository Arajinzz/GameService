#pragma once

namespace sCore
{
  template <typename Class>
  class SingletonClass
  {
  public:
    static Class& Get()
    {
      static Class instance;
      return instance;
    }
  
  protected:
    SingletonClass()
      : m_mutex()
    {
#ifdef _DEBUG
      std::lock_guard<std::mutex> lock(m_mutex);
      Instances.push_back(this);
#endif
    }

    virtual ~SingletonClass()
    {
#ifdef _DEBUG
      std::lock_guard<std::mutex> lock(m_mutex);
      auto it = std::find(Instances.begin(), Instances.end(), this);
      Instances.erase(it);
#endif
    }

  protected:
#ifdef _DEBUG
    inline static std::vector<SingletonClass*> Instances;
#endif

  private:
    std::mutex m_mutex;

  public:
    SingletonClass(SingletonClass const&) = delete;
    void operator=(SingletonClass const&) = delete;
  };

  // helper
  template <typename Class>
  inline Class& Singleton()
  {
    return SingletonClass<Class>::Get();
  }

} // namespace sCore
