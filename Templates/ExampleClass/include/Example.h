#pragma once

namespace Ex
{
  class Example
  {
  public:
    explicit Example();
    virtual ~Example();

  private:
    // delete copy and assignment
    Example(const Example&) = delete;
    Example& operator=(const Example&) = delete;
  };

} // namespace Ex
