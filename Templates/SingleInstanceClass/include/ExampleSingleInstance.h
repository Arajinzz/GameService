#pragma once

namespace WiRdr
{
  class Example
  {
  public:
    static Example* Instance;

    Example();
    virtual ~Example();

  public:
    Example(Example const&) = delete;
    void operator=(Example const&) = delete;
  };

} // namespace WiRdr
