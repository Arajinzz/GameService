#pragma once

namespace Ex
{
  class Singleton
  {
  public:
    static Singleton& Get()
    {
      static Singleton instance;
      return instance;
    }
  
  private:
    Singleton();
    virtual ~Singleton();

  public:
    Singleton(Singleton const&) = delete;
    void operator=(Singleton const&) = delete;
  };

} // namespace Ex
